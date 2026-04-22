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

// i'm not setting them to nullptr ever again
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
        m_data.levelStatus.resize(Constants::Challenge::NUM_LEVELS, static_cast<int>(LevelStatus::locked));
        log::debug("Level Status: {}", m_data.levelStatus);
        m_data.levelStatus.front() = static_cast<int>(LevelStatus::inProgress);
        static_cast<ChallengeLayer*>(m_sender)->onLoadLevelsFinished();
    }
}

void DataManager::loadLevelsFailed(char const* key) {
    log::error("Failed to load levels from: {}", key);
}

void DataManager::saveToDisk() {
    m_data.levels.clear();
    for (auto const& level : m_levels) {
        auto id { level.data()->m_levelID.value() };
        GameLevelManager::sharedState()->saveLevel(level.data());
        m_data.levels.push_back(id);
    }

    Mod::get()->setSavedValue<bool>("saveExists", true);
    Mod::get()->setSavedValue<SaveData>("saveData", m_data);
}

void DataManager::restoreFromDisk() {
    if (!Mod::get()->getSavedValue<bool>("saveExists")) {
        notifyLevelsRestored(false);
        return;
    }

    m_data = Mod::get()->getSavedValue<SaveData>("saveData");
    m_levels.clear();
    m_levelsToDownload.clear();

    auto size { m_data.levels.size() };
    for (auto i { 0uz }; i < size; ++i) {
        auto level { m_data.levels[i] };
        auto savedLevel { GameLevelManager::sharedState()->getSavedLevel(level) };

        // log::debug("Level{}exists:{},CN:{}", i, static_cast<bool>(savedLevel), (savedLevel ? !savedLevel->m_creatorName.empty() : false));

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
    // log::debug("Level download successful. ID: {}", level->m_levelID);
    m_levels[m_levelsToDownload[level->m_levelID]] = level;
    m_levelsToDownload.erase(level->m_levelID);

    if (m_levelsToDownload.empty()) notifyLevelsRestored(true);
}

// i hope this doesn't permanently break the game
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
    log::debug("n: {}", n);
    log::debug("Level Status: {}", m_data.levelStatus);
    m_data.levelStatus[n] = static_cast<int>(LevelStatus::completed);
    m_data.completedLevels = std::max(m_data.completedLevels, nInt + 1);
    if (m_data.completedLevels > nInt && m_data.completedLevels < Constants::Challenge::NUM_LEVELS) {
        m_data.levelStatus[static_cast<size_t>(m_data.completedLevels)] = static_cast<int>(LevelStatus::inProgress);
        static_cast<ChallengeLayer*>(m_challengeLayer)->unlockButton(static_cast<size_t>(m_data.completedLevels));
    }
}
