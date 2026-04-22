#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

class ChallengeLevelInfoLayer final : public LevelInfoLayer {
public:
    static ChallengeLevelInfoLayer* create(GJGameLevel* level, bool challenge, CCObject* target, SEL_MenuHandler callback, int levelIndex, bool enabled);

    static CCScene* scene(GJGameLevel* level, bool challenge, CCObject* target, SEL_MenuHandler callback, int levelIndex, bool enabled);

    bool init(GJGameLevel* level, bool challenge, CCObject* target, SEL_MenuHandler callback, int levelIndex, bool enabled);
};
