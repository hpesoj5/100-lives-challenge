#include "ChallengeLayer.hpp"
#include "Constants.hpp"
#include "DataManager.hpp"
#include "Globals.hpp"
#include "hooks/LevelInfoLayer.hpp"

ChallengeLayer* ChallengeLayer::create() {
    auto challengeLayer { new ChallengeLayer };
    Challenge::currentChallengeLayer = challengeLayer;
    if (challengeLayer && challengeLayer->init()) {
        challengeLayer->autorelease();
        return challengeLayer;
    }
    CC_SAFE_DELETE(challengeLayer);
    Challenge::currentChallengeLayer = nullptr;
    return nullptr;
}

ChallengeLayer::~ChallengeLayer() {
    Challenge::currentChallengeLayer = nullptr;
    if (m_saveExists) DataManager::get().saveToDisk();
}

// add a reset button to clear save in the future
bool ChallengeLayer::init() {
    if (!CCLayer::init()) {
        log::error("Failed to initialise CCLayer");
        return false;
    }

    CCTextureCache::sharedTextureCache()->addImage("WorldSheet.png", false);
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("WorldSheet.plist");

    auto winSize { CCDirector::sharedDirector()->getWinSize() };

    auto background { createLayerBG() };
    addChild(background, -2);

    m_exitMenuBuilder
        .layout(ColumnLayout::create())
        .position(Constants::Menu::EXIT_PADDING, winSize.height - Constants::Menu::EXIT_PADDING)
        .ID("exit-menu")
        .zOrder(1)
        .parent(this);

    auto backButton { CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png"),
        this,
        menu_selector(ChallengeLayer::onExitToMenu)
    ) };
    backButton->setID("back-button");
    m_exitMenuBuilder.child(backButton, 0);

    m_actionMenuBuilder
        .layout(RowLayout::create())
        .anchor(0.5f, 0.5f)
        .position(winSize.width / 2.f, winSize.height * Constants::Menu::ACTION_MENU_POSITION_PERCENT)
        .ID("action-menu")
        .zOrder(1)
        .parent(this);

    auto newChallengeBtn { CCMenuItemSpriteExtra::create(
        ButtonSprite::create("New Challenge"),
        this,
        menu_selector(ChallengeLayer::onNewChallenge)
    ) };
    newChallengeBtn->setID("new-challenge-button");
    m_actionMenuBuilder.child(newChallengeBtn, 0);

    m_leftMenuBuilder
        .layout(ColumnLayout::create())
        .anchor(0.5f, 0.5f)
        .position(Constants::Menu::EXIT_PADDING, winSize.height / 2.f)
        .ID("left-menu")
        .zOrder(1)
        .parent(this);

    auto previousPageSprite { CCSprite::createWithSpriteFrameName("navArrowBtn_001.png") };
    previousPageSprite->setFlipX(true);
    auto previousPageBtn { CCMenuItemSpriteExtra::create(
        previousPageSprite,
        this,
        menu_selector(ChallengeLayer::onPreviousPage)
    ) };
    m_leftMenuBuilder.child(previousPageBtn, 0);

    m_rightMenuBuilder
        .layout(ColumnLayout::create())
        .anchor(0.5f, 0.5f)
        .position(winSize.width - Constants::Menu::EXIT_PADDING, winSize.height / 2.f)
        .ID("right-menu")
        .zOrder(1)
        .parent(this);

    auto nextPageBtn { CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("navArrowBtn_001.png"),
        this,
        menu_selector(ChallengeLayer::onNextPage)
    ) };
    m_rightMenuBuilder.child(nextPageBtn, 0);

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
    DataManager::get().restoreFromDisk();

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
    setTouchEnabled(false);
    setKeyboardEnabled(false);
    setKeypadEnabled(false);
    setMouseEnabled(false);

    CCLayer::onExit();
}

void ChallengeLayer::keyDown(enumKeyCodes key, double) {
    if (key == enumKeyCodes::KEY_Left || key == enumKeyCodes::KEY_ArrowLeft || key == enumKeyCodes::CONTROLLER_Left || key == enumKeyCodes::CONTROLLER_LTHUMBSTICK_LEFT) {
        changePage(m_scrollLayer->m_page - 1);
    }
    else if (key == enumKeyCodes::KEY_Right || key == enumKeyCodes::KEY_ArrowRight || key == enumKeyCodes::CONTROLLER_Right || key == enumKeyCodes::CONTROLLER_LTHUMBSTICK_RIGHT) {
        changePage(m_scrollLayer->m_page + 1);
    }
    else if (key == enumKeyCodes::KEY_Escape || key == enumKeyCodes::CONTROLLER_B) keyBackClicked();
    // placeholder
    else if (key == enumKeyCodes::KEY_C) DataManager::get().setLevelComplete(DataManager::get().getCompletedLevels());
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

void ChallengeLayer::onPreviousPage(CCObject*) {
    changePage(m_scrollLayer->m_page - 1);
}

void ChallengeLayer::onNextPage(CCObject*) {
    changePage(m_scrollLayer->m_page + 1);
}

void ChallengeLayer::onNewChallenge(CCObject*) {
    createQuickPopup(
        "New Challenge",
        "Start new challenge?\n(Current score will be erased!)",
        "No", "Yes",
        [this](auto, bool btn2) {
            if (btn2) {
                // try to delete previously saved levels
                DataManager::get().deleteAllLevels();
                DataManager::get().getLevelVector().clear();
                DataManager::get().getLevelVector().reserve(Constants::Challenge::NUM_LEVELS);

                DataManager::get().loadLevels(this, 0);
            }
        }
    );
}

void ChallengeLayer::onLoadLevelsFinished() {
    m_scrollLayer->instantMoveToPage(0);
    m_saveExists = true;
    drawLevels(true);
}

void ChallengeLayer::onLoadLevelsFailed() {
    // FLAlert
}

void ChallengeLayer::onEnterLevel(CCObject* sender) {
    auto btn { static_cast<CCMenuItemSpriteExtra*>(sender) };

    // not gonna make this prettier
    auto parameters { static_cast<LevelInfo*>(btn->getUserObject()) };
    Challenge::currentLevelIndex = parameters->index;
    Challenge::correctLevelID = parameters->ID;
    Challenge::skipButtonEnabled = DataManager::get().hasRemainingSkips();
    CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, ChallengeLevelInfoLayer::scene(DataManager::get().getLevel(parameters->index), false)));
}

void ChallengeLayer::onLevelSkip(CCObject* sender) {
    auto btn { static_cast<CCMenuItemSpriteExtra*>(sender) };
    auto levelIndex { btn->getTag() };

    DataManager::get().setLevelSkipped(levelIndex);
    CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
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
        if (i % 5 == 0) mainMenu = static_cast<CCMenu*>(m_scrollLayer->getPage(page)->getChildByID(Constants::Menu::MAIN_MENU_PREFIX + std::to_string(page)));

        auto contentSize { mainMenu->getContentSize() };

        auto levelBtn { CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName(levelsLoaded && i <= DataManager::get().getCompletedLevels() ? "worldLevelBtn_001.png" : "worldLevelBtn_locked_001.png"),
            this,
            (levelsLoaded ? menu_selector(ChallengeLayer::onEnterLevel) : nullptr)
        ) };

        levelBtn->setID(Constants::Menu::LEVEL_BTN_PREFIX + std::to_string(i));
        // levelBtn->setTag(i);
        levelBtn->setPosition(contentSize.width * Constants::Menu::LEVEL_BTN_POSITION[(i / 5) % 2][i % 5].x, contentSize.height * Constants::Menu::LEVEL_BTN_POSITION[(i / 5) % 2][i % 5].y);

        mainMenu->addChild(levelBtn, 5);

        if (levelsLoaded && i <= DataManager::get().getCompletedLevels()) {
            std::string const& levelName { DataManager::get().getLevelName(i) };

            auto levelLabel { CCLabelBMFont::create(
                levelName.c_str(),
                "bigFont.fnt"
            ) };

            levelLabel->setScale(levelName.size() > Constants::Menu::LABEL_THRESHOLD ? Constants::Menu::LABEL_SCALE_SMALL : Constants::Menu::LABEL_SCALE_BIG);
            levelLabel->setID(Constants::Menu::LEVEL_LABEL_PREFIX + std::to_string(i));
            levelLabel->setPosition(levelBtn->getContentWidth() / 2.f, levelBtn->getContentHeight() * (1.f + Constants::Menu::LEVEL_LABEL_SPACING_PERCENT));

            levelBtn->addChild(levelLabel);
            levelBtn->setUserObject(new LevelInfo { i, DataManager::get().getLevelID(i) });
        }
        else levelBtn->setEnabled(false);

    }

    for (auto i { 0uz }; i < Constants::Challenge::NUM_PAGES; ++i) {
        m_scrollLayer->getPage(i)->getChildByID(Constants::Menu::MAIN_MENU_PREFIX + std::to_string(i))->updateLayout();
    }
}

void ChallengeLayer::unlockButton(size_t n) {
    std::string ID { Constants::Menu::LEVEL_BTN_PREFIX + std::to_string(n) };
    auto page { n / 5 };
    std::string pageID { Constants::Menu::MAIN_MENU_PREFIX + std::to_string(page) };

    auto position { m_scrollLayer->getPage(page)->getChildByID(pageID)->getChildByID(ID)->getPosition() };

    auto levelBtn { CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("worldLevelBtn_001.png"),
        this,
        menu_selector(ChallengeLayer::onEnterLevel)
    ) };
    levelBtn->setID(ID);
    levelBtn->setUserObject(new LevelInfo{ n, DataManager::get().getLevelID(n) });
    levelBtn->setPosition(position);

    m_scrollLayer->getPage(page)->getChildByID(pageID)->removeChildByID(ID);
    m_scrollLayer->getPage(page)->getChildByID(pageID)->addChild(levelBtn, 5);

    std::string const& levelName { DataManager::get().getLevelName(n) };

    auto levelLabel { CCLabelBMFont::create(
        levelName.c_str(),
        "bigFont.fnt"
    ) };

    levelLabel->setScale(levelName.size() > Constants::Menu::LABEL_THRESHOLD ? Constants::Menu::LABEL_SCALE_SMALL : Constants::Menu::LABEL_SCALE_BIG);
    levelLabel->setID(Constants::Menu::LEVEL_LABEL_PREFIX + std::to_string(n));
    levelLabel->setPosition(levelBtn->getContentWidth() / 2.f, levelBtn->getContentHeight() * (1.f + Constants::Menu::LEVEL_LABEL_SPACING_PERCENT));

    levelBtn->addChild(levelLabel);
    m_scrollLayer->getPage(page)->getChildByID(pageID)->updateLayout();
}
