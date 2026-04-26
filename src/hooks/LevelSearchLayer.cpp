#include "../ChallengeLayer.hpp"
#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>

using namespace geode::prelude;

class $modify(ChallengeLevelSearchLayer, LevelSearchLayer) {
    bool init(int type) {
        if (!LevelSearchLayer::init(type)) {
            log::error("Failed to initialise LevelSearchLayer");
            return false;
        }

        auto sRecentIconSpr { CCSprite::createWithSpriteFrameName("GJ_sRecentIcon_001.png") };
        auto challengeButton { CCMenuItemSpriteExtra::create(CircleButtonSprite::create(
            CCLabelBMFont::create("100", "bigFont.fnt"),
            CircleBaseColor::Pink, CircleBaseSize::Small),
            this, menu_selector(ChallengeLevelSearchLayer::onChallengeLayer)
        ) };
        challengeButton->setID("challenge-button"_spr);

        auto menu { this->getChildByID("other-filter-menu") };
        menu->addChild(challengeButton);
        menu->updateLayout();

        return true;
    }

    void onChallengeLayer(CCObject*) {
       auto scene { CCScene::create() };
       scene->addChild(ChallengeLayer::create());

       CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, scene));
    }
};
