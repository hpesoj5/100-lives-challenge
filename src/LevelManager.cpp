#include "Constants.hpp"
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/binding/GJSearchObject.hpp>
#include "LevelManager.hpp"
#include "ChallengeLayer.hpp"

ChallengeLevelManager::ChallengeLevelManager() : m_sender {}, m_page {} {
    GameLevelManager::sharedState()->m_levelManagerDelegate = this;
}

ChallengeLevelManager::~ChallengeLevelManager() {
    GameLevelManager::sharedState()->m_levelManagerDelegate = nullptr;
}

void ChallengeLevelManager::loadLevels(CCObject* sender) {
    m_sender = sender;
    auto searchObject = GJSearchObject::create((SearchType::Recent));
    searchObject->setTag(0);
    GameLevelManager::sharedState()->getOnlineLevels(searchObject);
    searchObject = searchObject->getNextPageObject();
}

void ChallengeLevelManager::loadLevelsFinished(cocos2d::CCArray* levels, char const* key) {
    auto size { levels->count() };
    for (auto i { 0u }; i < size; ++i) {
        m_levels.push_back(
            Ref<GJGameLevel>(
                static_cast<GJGameLevel*>(
                    levels->objectAtIndex(i)
                )
            )
        );
    }
    static_cast<ChallengeLayer*>(m_sender)->onLoadLevelsFinished();
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
