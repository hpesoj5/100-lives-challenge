#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;
class $modify(ChallengeLevelInfoLayer, LevelInfoLayer) {
public:
    bool init(GJGameLevel* level, bool challenge);

    void onSkipSelect(CCObject* sender);

    void levelDownloadFinished(GJGameLevel* level);
    void levelDownloadFailed(int response);
    void onEnterTransitionDidFinish();
    void onPlay(CCObject* sender);
    void onBack(CCObject* sender);
};
