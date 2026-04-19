#pragma once

#include <Geode/Geode.hpp>
#include <vector>

using namespace geode::prelude;

class ChallengeLevelManager : public LevelManagerDelegate {
public:
    ChallengeLevelManager();
    ~ChallengeLevelManager();

    void loadLevels(CCObject* sender);
    void loadLevelsFinished(cocos2d::CCArray* levels, char const* key) override;
    void loadLevelsFailed(char const* key) override;
    GJGameLevel* getLevel(size_t index);
    std::string const& getLevelName(size_t index) const;
    
    std::vector<Ref<GJGameLevel>>& get() { return m_levels; }
    size_t count() const { return m_levels.size(); }

private:
    std::vector<Ref<GJGameLevel>> m_levels;
    CCObject* m_sender;
    int m_page;
};
