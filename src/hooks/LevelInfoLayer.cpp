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

    if (Challenge::inCorrectLevel()) {
        auto levelStatus { DataManager::get().getLevelStatus(Challenge::currentLevelIndex) };
        auto skipButtonEnabled { DataManager::get().hasRemainingSkips() && levelStatus != LevelStatus::completed && levelStatus != LevelStatus::skipped };
        log::debug("Skip button enabled: {}", skipButtonEnabled);
        auto topSprite { CircleButtonSprite::create(
            CCLabelBMFont::create("Skip", "bigFont.fnt"),
            (skipButtonEnabled ? CircleBaseColor::Red : CircleBaseColor::Gray),
            CircleBaseSize::Medium
        ) };
        auto skipButton { CCMenuItemSpriteExtra::create(
            topSprite,
            this,
            menu_selector(ChallengeLevelInfoLayer::onSkipSelect)
        ) };
        skipButton->setEnabled(skipButtonEnabled);
        skipButton->setTag(Challenge::currentLevelIndex);
        skipButton->setID("skip-button"_spr);

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
    if (Challenge::inCorrectLevel() && !DataManager::get().isRunOver() && Challenge::currentLevelDownloadFailed && Challenge::legitDownloadFailed) {
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
