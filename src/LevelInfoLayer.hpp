#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class ChallengeLevelInfoLayer final : public LevelInfoLayer {
public:
    static ChallengeLevelInfoLayer* create(GJGameLevel* level, bool challenge, CCObject* target, int levelIndex, bool enabled);

    static CCScene* scene(GJGameLevel* level, bool challenge, CCObject* target, int levelIndex, bool enabled);

    bool init(GJGameLevel* level, bool challenge, CCObject* target, int levelIndex, bool enabled);

    void onSkipSelect(CCObject* sender);

    void levelDownloadFailed(int response) override {
        LevelInfoLayer::levelDownloadFailed(response);
        log::debug("Level download failed: {}", response);
    }

private:
    CCObject* m_challengeLayer;
};
