#include "ChallengeLayer.hpp"
#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

class $modify(ChallengeMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) {
            log::error("Failed to initialise MenuLayer");
            return false;
        }

        auto sRecentIconSpr { CCSprite::createWithSpriteFrameName("GJ_sRecentIcon_001.png") };
        auto challengeButton { CCMenuItemSpriteExtra::create(CircleButtonSprite::create(sRecentIconSpr, CircleBaseColor::Green, CircleBaseSize::SmallAlt), this, menu_selector(ChallengeMenuLayer::onChallengeLayer)) };
        challengeButton->setID("challenge-button");

        auto menu { this->getChildByID("right-side-menu") };
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
