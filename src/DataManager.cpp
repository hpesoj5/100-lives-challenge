#include "ChallengeLayer.hpp"
#include "Constants.hpp"
#include "DataManager.hpp"
#include <algorithm>

DataManager::DataManager() : m_sender {}, m_challengeLayer {}, m_pageCount {} {
    m_prevLMD = this;
    m_prevLDD = this;
    std::swap(m_prevLMD, GameLevelManager::sharedState()->m_levelManagerDelegate);
    std::swap(m_prevLDD, GameLevelManager::sharedState()->m_levelDownloadDelegate);
}

DataManager::~DataManager() {
    std::swap(m_prevLMD, GameLevelManager::sharedState()->m_levelManagerDelegate);
    std::swap(m_prevLDD, GameLevelManager::sharedState()->m_levelDownloadDelegate);
}

void DataManager::loadLevels(CCObject* sender, int page){
    m_sender = sender;
    m_pageCount = page;
    auto searchObject = GJSearchObject::create((SearchType::Recent))->getPageObject(page);
    GameLevelManager::sharedState()->getOnlineLevels(searchObject);
}

void DataManager::loadLevelsFinished(cocos2d::CCArray* levels, char const* key) {
    auto size { levels->count() };
    for (auto i { 0u }; i < size; ++i) {
        if (m_levels.size() < Constants::Challenge::NUM_LEVELS) m_levels.push_back(
            Ref<GJGameLevel>(
                static_cast<GJGameLevel*>(
                    levels->objectAtIndex(i)
                )
            )
        );
    }
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
    static_cast<ChallengeLayer*>(m_sender)->onLoadLevelsFailed();
}

void DataManager::resetChallengeData() {
    m_data.levelStatus.clear();
    m_data.levelStatus.resize(Constants::Challenge::NUM_LEVELS, levelStatusToInt(LevelStatus::locked));
    m_data.levelStatus.front() = levelStatusToInt(LevelStatus::inProgress);
    m_data.skips = Constants::Challenge::NUM_SKIPS;
    m_data.lives = Constants::Challenge::NUM_LIVES;
    m_data.completedLevels = 0;
}

void DataManager::deleteAllLevels() {
    for (auto& level : m_levels) { 
        if (level) GameLevelManager::sharedState()->deleteLevel(level.data());
    }
}

void DataManager::saveToDisk() {
    m_data.levels.clear();
    for (auto const& level : m_levels) {
        auto id { level.data()->m_levelID.value() };
        GameLevelManager::sharedState()->saveLevel(level.data());
        m_data.levels.push_back(id);
    }

    Mod::get()->setSavedValue<bool>("saveExists", true);
    Mod::get()->setSavedValue<int>("bestScore", m_bestScore);
    Mod::get()->setSavedValue<ChallengeData>("challengeData", m_data);
}

void DataManager::restoreFromDisk() {
    if (!Mod::get()->getSavedValue<bool>("saveExists")) {
        notifyLevelsRestored(false);
        return;
    }

    m_bestScore = Mod::get()->getSavedValue<int>("bestScore");
    m_data = Mod::get()->getSavedValue<ChallengeData>("challengeData");
    
    m_levels.clear();
    m_levelsToDownload.clear();

    auto size { m_data.levels.size() };
    for (auto i { 0uz }; i < size; ++i) {
        auto level { m_data.levels[i] };
        auto savedLevel { GameLevelManager::sharedState()->getSavedLevel(level) };

        if (savedLevel && !savedLevel->m_creatorName.empty()) m_levels.push_back(Ref<GJGameLevel>(savedLevel));
        else {
            GameLevelManager::sharedState()->downloadLevel(level, false, 0);
            m_levels.push_back(Ref<GJGameLevel>());
            m_levelsToDownload[level] = i;
        }
    }

    if (m_levelsToDownload.empty()) notifyLevelsRestored(true);
}

void DataManager::levelDownloadFinished(GJGameLevel* level) {
    if (!m_levelsToDownload.contains(level->m_levelID)) return;

    m_levels[m_levelsToDownload[level->m_levelID]] = level;
    m_levelsToDownload.erase(level->m_levelID);

    if (m_levelsToDownload.empty()) notifyLevelsRestored(true);
}

void DataManager::levelDownloadFailed(int response) {
    log::error("Level download failed. Response: {}", response);

    // add an alert layer here

    notifyLevelsRestored(false);
}

void DataManager::notifyLevelsRestored(bool restored) const {
    static_cast<ChallengeLayer*>(m_challengeLayer)->onLevelsRestored(restored);
}

void DataManager::setLevelComplete(size_t n) {
    int nInt { static_cast<int>(n) };

    auto tmp { intToLevelStatus(m_data.levelStatus[n]) };
    if (tmp == LevelStatus::completed) return;
    if (tmp == LevelStatus::skipped) {
        m_data.levelStatus[n] = levelStatusToInt(LevelStatus::completed);
        return;
    }

    m_data.levelStatus[n] = levelStatusToInt(LevelStatus::completed);
    m_data.completedLevels = std::max(m_data.completedLevels, nInt + 1);
    
    if (m_data.completedLevels > nInt && m_data.completedLevels < Constants::Challenge::NUM_LEVELS) {
        m_data.levelStatus[static_cast<size_t>(m_data.completedLevels)] = levelStatusToInt(LevelStatus::inProgress);
    }

    saveToDisk();

    if (m_data.completedLevels > nInt && m_data.completedLevels < Constants::Challenge::NUM_LEVELS) {
        static_cast<ChallengeLayer*>(m_challengeLayer)->unlockButton(static_cast<size_t>(m_data.completedLevels));
    }
}

void DataManager::setLevelSkipped(size_t n) {
    if (!hasRemainingSkips()) return;
    int nInt { static_cast<int>(n) };

    decrementSkips();
    if (intToLevelStatus(m_data.levelStatus[n]) == LevelStatus::completed) return;

    m_data.levelStatus[n] = levelStatusToInt(LevelStatus::skipped);
    m_data.completedLevels = std::max(m_data.completedLevels, nInt + 1);

    if (m_data.completedLevels > nInt && m_data.completedLevels < Constants::Challenge::NUM_LEVELS) {
        m_data.levelStatus[static_cast<size_t>(m_data.completedLevels)] = levelStatusToInt(LevelStatus::inProgress);
    }

    saveToDisk();

    if (m_data.completedLevels > nInt && m_data.completedLevels < Constants::Challenge::NUM_LEVELS) {
        static_cast<ChallengeLayer*>(m_challengeLayer)->unlockButton(static_cast<size_t>(m_data.completedLevels));
    }
}
