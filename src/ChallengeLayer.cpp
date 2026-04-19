#include "ChallengeLayer.hpp"
#include "Constants.hpp"
#include <Geode/binding/BoomScrollLayer.hpp>

ChallengeLayer* ChallengeLayer::create() {
    auto challengeLayer { new ChallengeLayer };
    if (challengeLayer && challengeLayer->init()) {
        challengeLayer->autorelease();
        return challengeLayer;
    }
    CC_SAFE_DELETE(challengeLayer);
    return nullptr;
}

bool ChallengeLayer::init() {
    if (!CCLayer::init()) {
        log::error("Failed to initialise CCLayer");
        return false;
    }

    CCTextureCache::sharedTextureCache()->addImage("WorldSheet.png", false);
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("WorldSheet.plist");

    auto winSize { CCDirector::sharedDirector()->getWinSize() };

    auto background { createLayerBG() };
    this->addChild(background, -2);

    m_exitMenuBuilder
        .layout(ColumnLayout::create())
        .position(Constants::Menu::EXIT_PADDING, winSize.height - Constants::Menu::EXIT_PADDING)
        .ID("exit-menu")
        .zOrder(1)
        .parent(this);
    
    m_mainMenuBuilder
        .layout(RowLayout::create())
        .anchor(0.5f, 0.5f)
        .position(winSize.width / 2.f, winSize.height * 0.4f)
        .ID("main-menu")
        .zOrder(1)
        .parent(this);

    m_actionMenuBuilder
        .layout(RowLayout::create())
        .anchor(0.5f, 0.5f)
        .position(winSize.width / 2.f, winSize.height * 0.2f)
        .ID("action-menu")
        .zOrder(1)
        .parent(this);

    auto backButton { CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png"),
        this,
        menu_selector(ChallengeLayer::onExitToMenu)
    ) };
    backButton->setID("back-button");

    auto newChallengeSprite { ButtonSprite::create("New Challenge") };
    auto newChallengeBtn { CCMenuItemSpriteExtra::create(
        newChallengeSprite,
        this,
        menu_selector(ChallengeLayer::onNewChallenge)
    ) };
    newChallengeBtn->setID("new-challenge-button");

    drawLevels(false);

    m_exitMenuBuilder.child(backButton, 0);
    m_actionMenuBuilder.child(newChallengeBtn, 0);

    return true;
}

void ChallengeLayer::onEnter() {
    CCLayer::onEnter();
    
    setTouchEnabled(true);
    setKeyboardEnabled(true);
    setKeypadEnabled(true);
    setMouseEnabled(true);
}

void ChallengeLayer::onExit() {
    CCLayer::onExit();

    setTouchEnabled(false);
    setKeyboardEnabled(false);
    setKeypadEnabled(false);
    setMouseEnabled(false);
}

void ChallengeLayer::onExitToMenu(CCObject*) {
    CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}

void ChallengeLayer::keyBackClicked() {
    onExitToMenu(nullptr);
}

void ChallengeLayer::onNewChallenge(CCObject*) {
    createQuickPopup(
        "New Challenge",
        "Start new challenge?\n(Current score will be erased!)",
        "NO", "YES",
        [this](auto, bool btn2) {
            if (btn2) {
                m_levelManager.get().clear();
                m_levelManager.get().reserve(Constants::Challenge::NUM_LEVELS);
                m_levelManager.loadLevels(this);
            }
        }
    );
}

void ChallengeLayer::onLoadLevelsFinished() {
    m_mainMenuBuilder.removeAllChildren();
    log::debug("Number of levels (onNewChallenge): {}", m_levelManager.count());
    // auto levelBtn { CCMenuItemSpriteExtra::create(
    //     CCSprite::createWithSpriteFrameName("worldLevelBtn_001.png"),
    //     this,
    //     menu_selector(ChallengeLayer::onEnterLevel)
    // ) };
    // levelBtn->setID("level-button0");
    // levelBtn->setTag(0);
    // m_mainMenuBuilder.child(levelBtn, 0);
    drawLevels(true);
}

void ChallengeLayer::onEnterLevel(CCObject* sender) {
    auto btn { static_cast<CCMenuItemSpriteExtra*>(sender) };
    log::debug("Number of levels on button press: {}", m_levelManager.count());
    log::debug("Level: {}", m_levelManager.getLevel(btn->getTag()));
    CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, LevelInfoLayer::scene(m_levelManager.getLevel(btn->getTag()), false)));
}

void ChallengeLayer::drawLevels(bool levelsLoaded) {
    m_mainMenuBuilder.removeAllChildren();
    auto size { m_levelManager.count() };
    for (auto i { 0uz }; i < 1; ++i)
    {
        auto levelBtn { CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName(levelsLoaded ? "worldLevelBtn_001.png" : "worldLevelBtn_locked_001.png"),
            this,
            (levelsLoaded ? menu_selector(ChallengeLayer::onEnterLevel) : nullptr)
        ) };
        std::string ID { "level-button-" + std::to_string(i) };
        levelBtn->setID(ID);
        levelBtn->setTag(i);
        m_mainMenuBuilder.child_fast(levelBtn, 0);
        float posX { m_mainMenuBuilder.getContentWidth() * (0.5f + i * Constants::Menu::LEVEL_SPACING_PERCENT) };
        float posY { m_mainMenuBuilder.getContentHeight() / 2.f };
        levelBtn->setPosition(posX, posY);
        levelBtn->setAnchorPoint({ 0.5f, 0.f });

        if (levelsLoaded) {
            log::debug("Level Name: {}", m_levelManager.getLevelName(i));
            std::string const& levelName { m_levelManager.getLevelName(i) };
            auto levelLabel { CCLabelBMFont::create(
                levelName.c_str(),
                "bigFont.fnt"
            ) };
            levelLabel->setScale(levelName.size() > Constants::Menu::LABEL_THRESHOLD ? Constants::Menu::LABEL_SCALE_SMALL : Constants::Menu::LABEL_SCALE_BIG);
            levelLabel->setID("level-label-" + std::to_string(i));
            levelLabel->setPosition(levelBtn->getContentWidth() / 2.f, levelBtn->getContentHeight() * (1.f + Constants::Menu::LEVEL_LABEL_SPACING_PERCENT));
            levelBtn->addChild(levelLabel);
        }
        else levelBtn->setEnabled(false);
    }
    m_mainMenuBuilder.updateLayout();
}
