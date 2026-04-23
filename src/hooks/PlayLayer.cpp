#include "../DataManager.hpp"
#include "../Globals.hpp"
#include "PlayLayer.hpp"

void ChallengePlayLayer::levelComplete() {
    if (Challenge::currentChallengeLayer && Challenge::currentLevelID.top() == Challenge::correctLevelID && !m_isPracticeMode) {
        log::debug("Level {} complete", Challenge::currentLevelIndex + 1);
        DataManager::get().setLevelComplete(Challenge::currentLevelIndex);
    }
    PlayLayer::levelComplete();
}

