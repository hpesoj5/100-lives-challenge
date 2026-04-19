#pragma once

#include "DataManager.hpp"
#include "Geode/cocos/script_support/CCScriptSupport.h"
#include "LevelManager.hpp"
#include "MenuBuilder.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;


class ChallengeLayer final : public CCLayer, public LevelManagerDelegate {
public:
    static ChallengeLayer* create();

    bool init() override;
    void onEnter() override;
    void onExit() override;
    void onExitToMenu(CCObject*);
    // void onExit() override;
    void keyBackClicked() override;

    void onNewChallenge(CCObject*);
    void onLoadLevelsFinished();
    void onEnterLevel(CCObject* sender);
    void drawLevels(bool levelsLoaded);

private:
    ChallengeLayer() = default;

    ChallengeLevelManager m_levelManager;
    DataManager m_dataManager;

    MenuBuilder m_mainMenuBuilder;
    MenuBuilder m_actionMenuBuilder;
    MenuBuilder m_exitMenuBuilder;
};
