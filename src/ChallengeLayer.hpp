#pragma once

#include "DataManager.hpp"
#include "LevelManager.hpp"
#include "MenuBuilder.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class ChallengeLayer final : public CCLayer, public BoomScrollLayerDelegate, public LevelManagerDelegate {
public:
    static ChallengeLayer* create();

    bool init() override;
    void onEnter() override;
    void onExit() override;
    void onExitToMenu(CCObject*);
    void keyBackClicked() override;
    void keyDown(enumKeyCodes key, double) override;
    void changePage(int page);

    void onNewChallenge(CCObject*);
    void onLoadLevelsFinished();
    void onEnterLevel(CCObject* sender);
    void drawLevels(bool levelsLoaded);


private:
    ChallengeLayer() = default;

    BoomScrollLayer* m_scrollLayer;
    ChallengeLevelManager m_levelManager;
    DataManager m_dataManager;

    MenuBuilder m_actionMenuBuilder;
    MenuBuilder m_exitMenuBuilder;
};
