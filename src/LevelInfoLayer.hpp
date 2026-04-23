#pragma once

#include "Globals.hpp"
#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

// class ChallengeLevelInfoLayer final : public LevelInfoLayer {
// public:
//     static ChallengeLevelInfoLayer* create(GJGameLevel* level, bool challenge, CCObject* target, int levelIndex, bool enabled);

//     static CCScene* scene(GJGameLevel* level, bool challenge, CCObject* target, int levelIndex, bool enabled);

//     bool init(GJGameLevel* level, bool challenge, CCObject* target, int levelIndex, bool enabled);

//     void onSkipSelect(CCObject* sender);

//     void levelDownloadFailed(int response) override {
//         LevelInfoLayer::levelDownloadFailed(response);
//         log::debug("Level download failed: {}", response);
//     }

// private:
//     CCObject* m_challengeLayer;
// };

class $modify(ChallengeLevelInfoLayer, LevelInfoLayer) {
public:
    bool init(GJGameLevel* level, bool challenge);

    void onSkipSelect(CCObject* sender);
};
