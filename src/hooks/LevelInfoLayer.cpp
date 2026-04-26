#include "../ChallengeLayer.hpp"
#include "../DataManager.hpp"
#include "../Globals.hpp"
#include "LevelInfoLayer.hpp"

bool ChallengeLevelInfoLayer::init(GJGameLevel* level, bool challenge) {
    if (!LevelInfoLayer::init(level, challenge)) return false;

    if (Challenge::currentChallengeLayer) {
        if (!Challenge::isPlaying) Challenge::currentLevelID.push(level->m_levelID);
        Challenge::isPlaying = false;
    }

    if (Challenge::currentChallengeLayer && Challenge::currentLevelID.top() == Challenge::correctLevelID) {
        auto topSprite { CircleButtonSprite::create(
            CCLabelBMFont::create("Skip", "bigFont.fnt"),
            (Challenge::skipButtonEnabled ? CircleBaseColor::Red : CircleBaseColor::Gray),
            CircleBaseSize::Medium
        ) };

        auto skipButton { CCMenuItemSpriteExtra::create(
            topSprite,
            this,
            menu_selector(ChallengeLevelInfoLayer::onSkipSelect)
        ) };
        skipButton->setEnabled(Challenge::skipButtonEnabled);
        skipButton->setTag(Challenge::currentLevelIndex);

        getChildByID("back-menu")->addChild(skipButton);
        getChildByID("back-menu")->updateLayout();
    }

    return true;
}

void ChallengeLevelInfoLayer::onSkipSelect(CCObject* sender) {
    createQuickPopup(
        "Skip Level",
        "Are you sure you want to skip this level?",
        "No", "Yes",
        [this, sender](auto, bool btn2) {
            if (btn2) {
                Challenge::currentLevelID.pop();
                Challenge::currentChallengeLayer->updateStats();
                Challenge::currentChallengeLayer->onLevelSkip(sender);
            }
        }
    );
}

void ChallengeLevelInfoLayer::levelDownloadFinished(GJGameLevel* level) {
    LevelInfoLayer::levelDownloadFinished(level);
    Challenge::currentLevelDownloadFailed = false;
}

void ChallengeLevelInfoLayer::levelDownloadFailed(int response) {
    LevelInfoLayer::levelDownloadFailed(response);
    if (Challenge::currentChallengeLayer && Challenge::currentLevelID.top() == Challenge::correctLevelID) {
        Challenge::currentLevelDownloadFailed = true;
    }
}

void ChallengeLevelInfoLayer::onEnterTransitionDidFinish() {
    LevelInfoLayer::onEnterTransitionDidFinish();
    if (Challenge::currentChallengeLayer && Challenge::currentLevelID.top() == Challenge::correctLevelID && Challenge::currentLevelDownloadFailed) {
        if (DataManager::get().rewardLevelSkip(Challenge::currentLevelIndex)) {
            queueInMainThread([](){ FLAlertLayer::create(
                "Download Failed",
                "Because the level could not be downloaded, you will receive an extra skip.",
                "OK"
            )->show(); });
        }
    }
}

void ChallengeLevelInfoLayer::onPlay(CCObject* sender) {
    if (Challenge::currentChallengeLayer) Challenge::isPlaying = true;
    LevelInfoLayer::onPlay(sender);
}

void ChallengeLevelInfoLayer::onBack(CCObject* sender) {
    if (Challenge::currentChallengeLayer) {
        Challenge::currentLevelID.pop();
        Challenge::currentChallengeLayer->updateStats();
    }
    LevelInfoLayer::onBack(sender);
}
