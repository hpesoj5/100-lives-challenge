#include "../Globals.hpp"
#include "../DataManager.hpp"
#include "PlayerObject.hpp"

void ChallengePlayerObject::playerDestroyed(bool noEffects) {
    if (Challenge::currentChallengeLayer && Challenge::currentLevelID.top() == Challenge::correctLevelID) {
        DataManager::get().addLives(-1);
        log::debug("{} lives remaining", DataManager::get().getLives());
    }
    PlayerObject::playerDestroyed(noEffects);
}
