#include "ChallengeLayer.hpp"
#include "Constants.hpp"

ChallengeLayer* ChallengeLayer::create() {
    auto challengeLayer { new ChallengeLayer };
    if (challengeLayer && challengeLayer->init()) {
        challengeLayer->autorelease();
        return challengeLayer;
    }
    CC_SAFE_DELETE(challengeLayer);
    return nullptr;
}

ChallengeLayer::~ChallengeLayer() {
    // add a reset button to clear save in the future

    if (m_saveExists) m_dataManager.saveToDisk();
}

bool ChallengeLayer::init() {
    if (!CCLayer::init()) {
        log::error("Failed to initialise CCLayer");
        return false;
    }

    CCTextureCache::sharedTextureCache()->addImage("WorldSheet.png", false);
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("WorldSheet.plist");
    m_dataManager.setChallengeLayer(this);

    auto winSize { CCDirector::sharedDirector()->getWinSize() };

    auto background { createLayerBG() };
    addChild(background, -2);

    m_exitMenuBuilder
        .layout(ColumnLayout::create())
        .position(Constants::Menu::EXIT_PADDING, winSize.height - Constants::Menu::EXIT_PADDING)
        .ID("exit-menu")
        .zOrder(1)
        .parent(this);

    m_actionMenuBuilder
        .layout(RowLayout::create())
        .anchor(0.5f, 0.5f)
        .position(winSize.width / 2.f, winSize.height * Constants::Menu::ACTION_MENU_POSITION_PERCENT)
        .ID("action-menu")
        .zOrder(1)
        .parent(this);


    auto backButton { CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png"),
        this,
        menu_selector(ChallengeLayer::onExitToMenu)
    ) };
    backButton->setID("back-button");
    m_exitMenuBuilder.child(backButton, 0);


    auto newChallengeBtn { CCMenuItemSpriteExtra::create(
        ButtonSprite::create("New Challenge"),
        this,
        menu_selector(ChallengeLayer::onNewChallenge)
    ) };
    newChallengeBtn->setID("new-challenge-button");
    m_actionMenuBuilder.child(newChallengeBtn, 0);


    auto pages { CCArray::createWithCapacity(Constants::Challenge::NUM_PAGES) };

    for (auto i { 0uz }; i < Constants::Challenge::NUM_PAGES; ++i) {
        auto page { CCLayer::create() };
        MenuBuilder mainMenu;
        mainMenu
            .contentSize(winSize)
            .anchor(0.5f, 0.5f)
            .ID(Constants::Menu::MAIN_MENU_PREFIX + std::to_string(i))
            .position(0.f, 0.f)
            .zOrder(1)
            .parent(page);

        pages->addObject(page);
    }

    m_scrollLayer = BoomScrollLayer::create(pages, 0, false);

    m_scrollLayer->setPagesIndicatorPosition({ winSize.width / 2.f, winSize.height * Constants::Menu::PAGES_INDICATOR_POSITION_PERCENT });
    m_scrollLayer->setKeypadEnabled(false);

    drawLevels(false);
    addChild(m_scrollLayer, -1);
    m_dataManager.restoreFromDisk();

    return true;
}

void ChallengeLayer::onLevelsRestored(bool restored) {
    drawLevels(restored);
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

// doesn't work on its own for some reason, so added case to keyDown
void ChallengeLayer::keyBackClicked() {
    onExitToMenu(nullptr);
}

// taken from undefined068655, which is taken from LevelSelectLayer
void ChallengeLayer::changePage(int page) {
    if (m_scrollLayer->m_pageMoving) {
        m_scrollLayer->m_pageMoving = false;
        m_scrollLayer->m_extendedLayer->stopActionByTag(2);
        m_scrollLayer->m_extendedLayer->setPosition(m_scrollLayer->m_position);
        m_scrollLayer->moveToPageEnded();
    }
    m_scrollLayer->moveToPage(page);
}

void ChallengeLayer::keyDown(enumKeyCodes key, double) {
    if (key == enumKeyCodes::KEY_Left || key == enumKeyCodes::KEY_ArrowLeft || key == enumKeyCodes::CONTROLLER_Left || key == enumKeyCodes::CONTROLLER_LTHUMBSTICK_LEFT) {
        changePage(m_scrollLayer->m_page - 1);
    }
    else if (key == enumKeyCodes::KEY_Right || key == enumKeyCodes::KEY_ArrowRight || key == enumKeyCodes::CONTROLLER_Right || key == enumKeyCodes::CONTROLLER_LTHUMBSTICK_RIGHT) {
        changePage(m_scrollLayer->m_page + 1);
    }
    else if (key == enumKeyCodes::KEY_Escape || key == enumKeyCodes::CONTROLLER_B) keyBackClicked();
}

void ChallengeLayer::onNewChallenge(CCObject*) {
    createQuickPopup(
        "New Challenge",
        "Start new challenge?\n(Current score will be erased!)",
        "NO", "YES",
        [this](auto, bool btn2) {
            if (btn2) {
                m_dataManager.get().clear();
                m_dataManager.get().reserve(Constants::Challenge::NUM_LEVELS);
                m_dataManager.loadLevels(this, 0);
            }
        }
    );
}

void ChallengeLayer::onLoadLevelsFinished() {
    for (auto i { 0uz }; i < m_dataManager.count(); ++i) {
        log::debug("Level {}: {}", i, m_dataManager.getLevelName(i));
    }

    m_scrollLayer->instantMoveToPage(0);
    m_saveExists = true;
    drawLevels(true);
}

void ChallengeLayer::onEnterLevel(CCObject* sender) {
    auto btn { static_cast<CCMenuItemSpriteExtra*>(sender) };

    CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, LevelInfoLayer::scene(m_dataManager.getLevel(btn->getTag()), false)));
}

void ChallengeLayer::drawLevels(bool levelsLoaded) {
    auto size { Constants::Challenge::NUM_LEVELS };

    for (auto i { 0uz }; i < Constants::Challenge::NUM_PAGES; ++i) {
        m_scrollLayer->getPage(i)->getChildByID(Constants::Menu::MAIN_MENU_PREFIX + std::to_string(i))->removeAllChildren();
    }

    CCMenu* mainMenu {};

    for (auto i { 0uz }; i < size; ++i)
    {
        auto page { i / 5 };

        // Reseat mainMenu to the correct page
        if (i % 5 == 0) mainMenu = static_cast<CCMenu*>(m_scrollLayer->getPage(page)->getChildByID(Constants::Menu::MAIN_MENU_PREFIX + std::to_string(page)));

        auto levelBtn { CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName(levelsLoaded && i <= m_dataManager.getCompletedLevels() ? "worldLevelBtn_001.png" : "worldLevelBtn_locked_001.png"),
            this,
            (levelsLoaded ? menu_selector(ChallengeLayer::onEnterLevel) : nullptr)
        ) };

        std::string ID { Constants::Menu::LEVEL_BTN_PREFIX + std::to_string(page) + '-' + std::to_string(i) };

        levelBtn->setID(ID);
        levelBtn->setTag(i);
        auto contentSize { mainMenu->getContentSize() };
        levelBtn->setPosition(contentSize.width * Constants::Menu::LEVEL_BTN_POSITION[(i / 5) % 2][i % 5].x, contentSize.height * Constants::Menu::LEVEL_BTN_POSITION[(i / 5) % 2][i % 5].y);
        mainMenu->addChild(levelBtn, 5);

        if (levelsLoaded && i <= m_dataManager.getCompletedLevels()) {
            std::string const& levelName { m_dataManager.getLevelName(i) };

            auto levelLabel { CCLabelBMFont::create(
                levelName.c_str(),
                "bigFont.fnt"
            ) };

            levelLabel->setScale(levelName.size() > Constants::Menu::LABEL_THRESHOLD ? Constants::Menu::LABEL_SCALE_SMALL : Constants::Menu::LABEL_SCALE_BIG);
            levelLabel->setID(Constants::Menu::LEVEL_LABEL_PREFIX + std::to_string(page) + '-' + std::to_string(i));
            levelLabel->setPosition(levelBtn->getContentWidth() / 2.f, levelBtn->getContentHeight() * (1.f + Constants::Menu::LEVEL_LABEL_SPACING_PERCENT));

            levelBtn->addChild(levelLabel);
        }
        else levelBtn->setEnabled(false);

    }
    for (auto i { 0uz }; i < Constants::Challenge::NUM_PAGES; ++i) {
        m_scrollLayer->getPage(i)->getChildByID(Constants::Menu::MAIN_MENU_PREFIX + std::to_string(i))->updateLayout();
    }
}
