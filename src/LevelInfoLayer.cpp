#include "ChallengeLayer.hpp"
#include "LevelInfoLayer.hpp"

ChallengeLevelInfoLayer* ChallengeLevelInfoLayer::create(GJGameLevel* level, bool challenge, CCObject* target, int levelIndex, bool enabled) {
    auto layer { new ChallengeLevelInfoLayer };
    if (layer && layer->init(level, challenge, target, levelIndex, enabled)) {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

CCScene* ChallengeLevelInfoLayer::scene(GJGameLevel* level, bool challenge, CCObject* target, int levelIndex, bool enabled) {
    auto scene { CCScene::create() };
    auto node { create(level, challenge, target, levelIndex, enabled) };
    node->setPosition(0.f, 0.f);
    scene->setAnchorPoint({ 0.f, 0.f });
    scene->addChild(node);
    return scene;
}

bool ChallengeLevelInfoLayer::init(GJGameLevel* level, bool challenge, CCObject* target, int levelIndex, bool enabled) {
    if (!LevelInfoLayer::init(level, challenge)) return false;
    m_challengeLayer = target;

    auto topSprite { CircleButtonSprite::create(CCLabelBMFont::create("Skip", "bigFont.fnt"), (enabled ? CircleBaseColor::Red : CircleBaseColor::Gray), CircleBaseSize::Medium) };
    auto skipButton { CCMenuItemSpriteExtra::create(
        topSprite,
        this,
        menu_selector(ChallengeLevelInfoLayer::onSkipSelect)
    ) };
    skipButton->setEnabled(enabled);
    skipButton->setTag(levelIndex);

    getChildByID("back-menu")->addChild(skipButton);
    getChildByID("back-menu")->updateLayout();

    return true;
}

void ChallengeLevelInfoLayer::onSkipSelect(CCObject* sender) {
    createQuickPopup(
        "Skip Level",
        "Are you sure you want to skip this level?",
        "No", "Yes",
        [this, sender](auto, bool btn2) {
            if (btn2) {
                (static_cast<ChallengeLayer*>(m_challengeLayer))->onLevelSkip(sender);
            }
        }
    );
}
