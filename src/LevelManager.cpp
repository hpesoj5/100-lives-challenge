#include "Constants.hpp"
#include "LevelManager.hpp"
#include "ChallengeLayer.hpp"

ChallengeLevelManager::ChallengeLevelManager() : m_sender {}, m_pageCount {} {
    GameLevelManager::sharedState()->m_levelManagerDelegate = this;
}

ChallengeLevelManager::~ChallengeLevelManager() {
    GameLevelManager::sharedState()->m_levelManagerDelegate = nullptr;
}

void ChallengeLevelManager::loadLevels(CCObject* sender, int page) {
    m_sender = sender;
    m_pageCount = page;
    auto searchObject = GJSearchObject::create((SearchType::Recent))->getPageObject(page);
    GameLevelManager::sharedState()->getOnlineLevels(searchObject);
}

void ChallengeLevelManager::loadLevelsFinished(cocos2d::CCArray* levels, char const* key) {
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
    else static_cast<ChallengeLayer*>(m_sender)->onLoadLevelsFinished();
}

void ChallengeLevelManager::loadLevelsFailed(char const* key) {
    log::error("Failed to load levels from: {}", key);
}

GJGameLevel* ChallengeLevelManager::getLevel(size_t index) {
    return m_levels.at(index).data();
}

std::string const& ChallengeLevelManager::getLevelName(size_t index) const {
    return m_levels.at(index)->m_levelName;
}
