#pragma once

#include "DataManager.hpp"
#include "MenuBuilder.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class ChallengeLayer final : public CCLayer, public BoomScrollLayerDelegate, public LevelManagerDelegate {
public:
    static ChallengeLayer* create();

    bool init() override;
    void onEnter() override;
    void onExit() override;
    void onExitToMenu(CCObject*) { CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade); }
    void keyBackClicked() override { onExitToMenu(nullptr); }
    void keyDown(enumKeyCodes key, double) override;
    void changePage(int page);

    void onPreviousPage(CCObject*);
    void onNextPage(CCObject*);
    void onNewChallenge(CCObject*);
    void onLoadLevelsFinished();
    void onLoadLevelsFailed();
    void onEnterLevel(CCObject* sender);
    void onLevelSkip(CCObject* sender);
    void onLevelsRestored(bool restored) { drawLevels(restored); }
    void drawLevels(bool levelsLoaded);

    void unlockButton(size_t n);

private:
    ChallengeLayer() = default;
    ~ChallengeLayer();

    DataManager m_dataManager;

    MenuBuilder m_actionMenuBuilder;
    MenuBuilder m_exitMenuBuilder;
    MenuBuilder m_leftMenuBuilder;
    MenuBuilder m_rightMenuBuilder;

    BoomScrollLayer* m_scrollLayer;
    LevelManagerDelegate* m_prevLMD;
    LevelDownloadDelegate* m_prevLDD;
    bool m_saveExists;
};
