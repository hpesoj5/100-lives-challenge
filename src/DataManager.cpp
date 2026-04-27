#include "ChallengeLayer.hpp"
#include "Constants.hpp"
#include "DataManager.hpp"
#include "Globals.hpp"
#include <algorithm>

DataManager::DataManager() : m_sender {}, m_pageCount {} {}

void DataManager::loadLevels(CCObject* sender, int page){
    m_sender = sender;
    m_pageCount = page;

    prev_LMD = this;
    std::swap(GameLevelManager::sharedState()->m_levelManagerDelegate, prev_LMD);

    auto searchObject = GJSearchObject::create((SearchType::Recent))->getPageObject(page);
    GameLevelManager::sharedState()->getOnlineLevels(searchObject);
}

void DataManager::loadLevelsFinished(cocos2d::CCArray* levels, char const* key) {
    auto size { levels->count() };
    for (auto i { 0u }; i < size; ++i) {
        bool noDuplicate { true };
        auto ID { static_cast<GJGameLevel*>(levels->objectAtIndex(i))->m_levelID };
        for (const auto& level : m_levels) {
            if (level->m_levelID == ID) {
                noDuplicate = false;
                break;
            }
        }

        if (m_levels.size() < Constants::Challenge::NUM_LEVELS && noDuplicate) m_levels.push_back(
            Ref<GJGameLevel>(
                static_cast<GJGameLevel*>(
                    levels->objectAtIndex(i)
                )
            )
        );
    }

    std::swap(GameLevelManager::sharedState()->m_levelManagerDelegate, prev_LMD);

    if (m_levels.size() < Constants::Challenge::NUM_LEVELS) loadLevels(m_sender, m_pageCount + 1);
    else {
        log::info("{} levels loaded!", m_levels.size());
        resetChallengeData();
        saveToDisk();
        static_cast<ChallengeLayer*>(m_sender)->onLoadLevelsFinished();
    }
}

void DataManager::loadLevelsFailed(char const* key) {
    log::error("Failed to load levels from: {}", key);
    std::swap(GameLevelManager::sharedState()->m_levelManagerDelegate, prev_LMD);
    static_cast<ChallengeLayer*>(m_sender)->onLoadLevelsFailed();
}

void DataManager::resetChallengeData() {
    m_data = {};
}

void DataManager::deleteAllLevels() {
    prev_LMD = this;
    std::swap(GameLevelManager::sharedState()->m_levelManagerDelegate, prev_LMD);

    for (auto& level : m_levels) {
        if (level) GameLevelManager::sharedState()->deleteLevel(level);
    }

    std::swap(GameLevelManager::sharedState()->m_levelManagerDelegate, prev_LMD);
}

void DataManager::saveToDisk() {
    if (!runExists()) return;
    prev_LMD = this;
    std::swap(GameLevelManager::sharedState()->m_levelManagerDelegate, prev_LMD);

    m_data.levels.clear();
    for (auto const& level : m_levels) {
        auto id { level.data()->m_levelID.value() };
        GameLevelManager::sharedState()->saveLevel(level);
        m_data.levels.push_back(id);
    }

    Mod::get()->setSavedValue<bool>("runExists", m_runExists);
    Mod::get()->setSavedValue<int>("bestScore", m_bestScore);
    Mod::get()->setSavedValue<ChallengeData>("challengeData", m_data);

    std::swap(GameLevelManager::sharedState()->m_levelManagerDelegate, prev_LMD);
}

void DataManager::restoreFromDisk() {
    prev_LDD = this;
    prev_LMD = this;
    std::swap(GameLevelManager::sharedState()->m_levelDownloadDelegate, prev_LDD);
    std::swap(GameLevelManager::sharedState()->m_levelManagerDelegate, prev_LMD);

    m_bestScore = Mod::get()->getSavedValue<int>("bestScore");
    m_data = Mod::get()->getSavedValue<ChallengeData>("challengeData");
    m_runExists = Mod::get()->getSavedValue<bool>("runExists");

    if (!runExists()) {
        notifyLevelsRestored(false);
        return;
    }

    m_levels.clear();
    m_levelsToDownload.clear();

    std::vector<size_t> levelsToDownloadIndex;

    auto size { m_data.levels.size() };
    for (auto i { 0uz }; i < size; ++i) {
        auto level { m_data.levels[i] };
        // log::info("Retrieving level {} with ID {}", i, level);
        auto savedLevel { GameLevelManager::sharedState()->getSavedLevel(level) };

        if (savedLevel && !savedLevel->m_creatorName.empty()) m_levels.push_back(Ref<GJGameLevel>(savedLevel));
        else {
            log::debug("Could not retrieve level {} with ID {}. Downloading from online levels...", i, level);
            m_levelsToDownload[level] = i;
            levelsToDownloadIndex.push_back(level);
            m_levels.push_back(Ref<GJGameLevel>());
        }
    }

    if (m_levelsToDownload.empty()) notifyLevelsRestored(true);
    else {
        log::debug("Levels to download: {}", levelsToDownloadIndex.size());
        for (int level : levelsToDownloadIndex) {
            log::debug("Downloading level {}", level);
            GameLevelManager::sharedState()->downloadLevel(level, false, 0);
        }
    }
}

void DataManager::levelDownloadFinished(GJGameLevel* level) {
    if (!m_levelsToDownload.contains(level->m_levelID)) return;

    // log::info("Successfully downloaded level {}: {}", m_levelsToDownload[level->m_levelID], level);
    m_levels[m_levelsToDownload[level->m_levelID]] = level;
    m_levelsToDownload.erase(level->m_levelID);
    // log::debug("{} levels remaining to download", m_levelsToDownload.size());

    if (m_levelsToDownload.empty()) notifyLevelsRestored(true);
}

void DataManager::levelDownloadFailed(int response) {
    log::error("Level download failed. Response: {}", response);
    DataManager::get().updateBestScore(DataManager::get().getCompletedLevels());
    // add an alert layer here
    notifyLevelsRestored(false);
    queueInMainThread([](){ FLAlertLayer::create(
        "Error",
        "A level in the list could not be restored. As a result, the run will be reset, but your score will be saved.",
        "OK"
    )->show(); });
}

void DataManager::notifyLevelsRestored(bool restored) {
    setRunExists(restored);
    std::swap(GameLevelManager::sharedState()->m_levelDownloadDelegate, prev_LDD);
    std::swap(GameLevelManager::sharedState()->m_levelManagerDelegate, prev_LMD);

    Challenge::currentChallengeLayer->onLevelsRestored(restored);
}

void DataManager::setLevelComplete(size_t n, int numCoins) {
    if (n >= Constants::Challenge::NUM_LEVELS || m_data.isRunOver) return;
    int nInt { static_cast<int>(n) };

    addLives(numCoins - m_data.levelCoins[n]);
    m_data.levelCoins[n] = numCoins;
    auto tmp { intToLevelStatus(m_data.levelStatus[n]) };
    if (tmp == LevelStatus::completed) return;
    if (tmp == LevelStatus::skipped) {
        m_data.levelStatus[n] = levelStatusToInt(LevelStatus::completed);
        return;
    }

    m_data.levelStatus[n] = levelStatusToInt(LevelStatus::completed);
    m_data.completedLevels = std::max(m_data.completedLevels, std::min(Constants::Challenge::NUM_LEVELS, nInt + 1));
    updateBestScore(m_data.completedLevels);

    if (m_data.completedLevels > nInt && m_data.completedLevels < Constants::Challenge::NUM_LEVELS) {
        m_data.levelStatus[static_cast<size_t>(m_data.completedLevels)] = levelStatusToInt(LevelStatus::inProgress);
    }

    saveToDisk();

    if (m_data.completedLevels > nInt && m_data.completedLevels < Constants::Challenge::NUM_LEVELS) {
        Challenge::currentChallengeLayer->unlockButton(static_cast<size_t>(m_data.completedLevels));
        Challenge::currentChallengeLayer->instantChangePage(m_data.completedLevels / 5);
    }
    else if (m_data.completedLevels > nInt && m_data.completedLevels == Constants::Challenge::NUM_LEVELS) {
        Challenge::currentChallengeLayer->updateStats();
        m_data.isRunWon = true;
        saveToDisk();
    }
}

void DataManager::setLevelSkipped(size_t n) {
    if (n >= Constants::Challenge::NUM_LEVELS || m_data.isRunOver || !hasRemainingSkips()) return;
    int nInt { static_cast<int>(n) };

    auto tmp { intToLevelStatus(m_data.levelStatus[n]) };
    if (tmp == LevelStatus::completed || tmp == LevelStatus::skipped) return;
    decrementSkips();

    m_data.levelStatus[n] = levelStatusToInt(LevelStatus::skipped);
    m_data.completedLevels = std::max(m_data.completedLevels, std::min(Constants::Challenge::NUM_LEVELS, nInt + 1));
    updateBestScore(m_data.completedLevels);

    if (m_data.completedLevels > nInt && m_data.completedLevels < Constants::Challenge::NUM_LEVELS) {
        m_data.levelStatus[static_cast<size_t>(m_data.completedLevels)] = levelStatusToInt(LevelStatus::inProgress);
    }

    saveToDisk();

    if (m_data.completedLevels > nInt && m_data.completedLevels < Constants::Challenge::NUM_LEVELS) {
        Challenge::currentChallengeLayer->unlockButton(static_cast<size_t>(m_data.completedLevels));
        Challenge::currentChallengeLayer->instantChangePage(m_data.completedLevels / 5);
    }
    else if (m_data.completedLevels > nInt && m_data.completedLevels == Constants::Challenge::NUM_LEVELS) {
        Challenge::currentChallengeLayer->updateStats();
        m_data.isRunWon = true;
        saveToDisk();
    }
}

bool DataManager::rewardLevelSkip(size_t n) {
    if (intToLevelStatus(m_data.levelStatus[n]) == LevelStatus::completed || intToLevelStatus(m_data.levelStatus[n]) == LevelStatus::skipped) return false;
    if (m_data.skipGiven[n]) return false;

    ++m_data.skips;
    return m_data.skipGiven[n] = true;
}

void DataManager::updateBestScore(int score) {
    m_bestScore = std::max(m_bestScore, score);
}
