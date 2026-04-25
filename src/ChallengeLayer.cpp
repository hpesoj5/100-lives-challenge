#include "ChallengeLayer.hpp"
#include "Constants.hpp"
#include "DataManager.hpp"
#include "Globals.hpp"
#include "MenuBuilder.hpp"
#include "hooks/LevelInfoLayer.hpp"
#include <Geode/ui/LazySprite.hpp>

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

    auto background { CCSprite::create("game_bg_08_001.png") };
    background->setScale(1.2f);
    background->setColor(Constants::Menu::BG_COLOR);
    background->setID("background");
    background->setPosition(winSize / 2.f);
    addChild(background, -5);

    MenuBuilder titleMenuBuilder;
    titleMenuBuilder
        .ID("title-menu")
        .position(0.f, 0.f)
        .contentSize(winSize)
        .parent(this)
        .zOrder(-4);

    auto bottomLeftCorner{ CCSprite::createWithSpriteFrameName("gauntletCorner_001.png") };
    bottomLeftCorner->setID("bottom-left-corner");
    bottomLeftCorner->setAnchorPoint({ 0.f, 0.f });
    bottomLeftCorner->setPosition({ 0.f, 0.f });

    auto bottomRightCorner{ CCSprite::createWithSpriteFrameName("gauntletCorner_001.png") };
    bottomRightCorner->setID("bottom-right-corner");
    bottomRightCorner->setAnchorPoint({ 1.f, 0.f });
    bottomRightCorner->setPosition({ winSize.width, 0.f });
    bottomRightCorner->setFlipX(true);

    auto topLeftCorner{ CCSprite::createWithSpriteFrameName("gauntletCorner_001.png") };
    topLeftCorner->setID("top-left-corner");
    topLeftCorner->setAnchorPoint({ 0.f, 1.f });
    topLeftCorner->setPosition({ 0.f, winSize.height });
    topLeftCorner->setFlipY(true);

    auto topRightCorner{ CCSprite::createWithSpriteFrameName("gauntletCorner_001.png") };
    topRightCorner->setID("top-right-corner");
    topRightCorner->setAnchorPoint({ 1.f, 1.f });
    topRightCorner->setPosition(winSize);
    topRightCorner->setFlipX(true);
    topRightCorner->setFlipY(true);

    auto titleSprite { CCSprite::create("title.png"_spr) };
    titleSprite->setScale(Constants::Menu::TITLE_SCALE);
    titleSprite->setPosition({ winSize.width / 2.f, winSize.height * Constants::Menu::TITLE_YPOSITION });

    titleMenuBuilder
        .child(topLeftCorner, -3)
        .child(topRightCorner, -3)
        .child(bottomLeftCorner, -3)
        .child(bottomRightCorner, -3)
        .child(titleSprite, -3);

    MenuBuilder exitMenuBuilder;
    exitMenuBuilder
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
    exitMenuBuilder.child(backButton, 0);

    MenuBuilder actionMenuBuilder;
    actionMenuBuilder
        .anchor(0.5f, 0.5f)
        .position(winSize.width - Constants::Menu::CORNER_PADDING, Constants::Menu::CORNER_PADDING)
        .ID("action-menu")
        .zOrder(1)
        .parent(this);

    auto newChallengeBtn { CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_newBtn_001.png"),
        this,
        menu_selector(ChallengeLayer::onNewChallenge)
    ) };
    newChallengeBtn->setSizeMult(0.9);
    newChallengeBtn->setID("new-challenge-button");
    actionMenuBuilder.child(newChallengeBtn, 0);

    MenuBuilder leftMenuBuilder;
    leftMenuBuilder
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
    leftMenuBuilder.child(previousPageBtn, 0);

    MenuBuilder rightMenuBuilder;
    rightMenuBuilder
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
    rightMenuBuilder.child(nextPageBtn, 0);

    MenuBuilder statsMenuBuilder;
    statsMenuBuilder
        .layout(RowLayout::create()->setGap(winSize.width * Constants::Menu::STATS_MENU_LABEL_GAP)->setDefaultScaleLimits(0.f, 0.75f))
        .position(winSize.width / 2.f, winSize.height * Constants::Menu::STATS_MENU_YPOSITION)
        .ID("stats-menu")
        .zOrder(1)
        .parent(this);

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

        MenuBuilder backgroundMenu;
        backgroundMenu
            .contentSize(winSize)
            .anchor(0.5f, 0.5f)
            .ID(Constants::Menu::BG_MENU_PREFIX + std::to_string(i))
            .position(0.f, 0.f)
            .zOrder(-4)
            .parent(page);

        auto island { CCSprite::createWithSpriteFrameName( "worldIsland_01.png") };
        island->setID(Constants::Menu::BG_ISLAND_PREFIX + std::to_string(i));
        island->setPosition({ backgroundMenu.getContentWidth() / 2.f, backgroundMenu.getContentHeight() * Constants::Menu::BG_ISLAND_YPOSITION });
        backgroundMenu.child(island, -3);

        pages->addObject(page);
    }

    m_scrollLayer = BoomScrollLayer::create(pages, 0, false);

    m_scrollLayer->setPagesIndicatorPosition({ winSize.width / 2.f, winSize.height * Constants::Menu::PAGES_INDICATOR_POSITION_PERCENT });
    m_scrollLayer->setDotScale(0.f);
    m_scrollLayer->togglePageIndicators(false);
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
    updateStats();

    log::debug("drawLevels({})", levelsLoaded);
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
        levelBtn->setPosition(contentSize.width * Constants::Menu::LEVEL_BTN_POSITION[i % 5].x, contentSize.height * Constants::Menu::LEVEL_BTN_POSITION[i % 5].y);

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
    updateStats();

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

void ChallengeLayer::updateStats() {
    auto statsMenu { getChildByID("stats-menu") };
    statsMenu->removeAllChildren();

    std::string scoreString { "Score: " + std::to_string(DataManager::get().getCompletedLevels()) };
    auto scoreLabel { CCLabelBMFont::create(
        scoreString.c_str(),
        "bigFont.fnt"
    ) };
    scoreLabel->setScale(Constants::Menu::STATS_LABEL_SCALE);
    // scoreLabel->setAnchorPoint({ 0.f, 0.f });
    // scoreLabel->setPosition(0.f, statsMenu->getContentWidth() * 0.06f);

    std::string livesString { "Lives: " + std::to_string(DataManager::get().getLives()) };
    auto livesLabel { CCLabelBMFont::create(
        livesString.c_str(),
        "bigFont.fnt"
    ) };
    livesLabel->setScale(Constants::Menu::STATS_LABEL_SCALE);
    // livesLabel->setAnchorPoint({ 0.f, 0.f });
    // livesLabel->setPosition(0.f, statsMenu->getContentWidth() * 0.03f);

    std::string skipsString { "Skips: " + std::to_string(DataManager::get().getSkips()) };
    auto skipsLabel { CCLabelBMFont::create(
        skipsString.c_str(),
        "bigFont.fnt"
    ) };
    skipsLabel->setScale(Constants::Menu::STATS_LABEL_SCALE);
    // skipsLabel->setAnchorPoint({ 0.f, 0.f });
    // skipsLabel->setPosition(0.f, statsMenu->getContentWidth() * 0.f);

    statsMenu->addChild(scoreLabel);
    statsMenu->addChild(livesLabel);
    statsMenu->addChild(skipsLabel);

    scoreLabel->setScale(Constants::Menu::STATS_LABEL_SCALE);
    livesLabel->setScale(Constants::Menu::STATS_LABEL_SCALE);
    skipsLabel->setScale(Constants::Menu::STATS_LABEL_SCALE);

    statsMenu->updateLayout();
}
