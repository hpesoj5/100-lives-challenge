#include "../Globals.hpp"
#include "../DataManager.hpp"
#include "PlayerObject.hpp"

void ChallengePlayerObject::playerDestroyed(bool noEffects) {
    if (Challenge::currentChallengeLayer && Challenge::currentLevelID.top() == Challenge::correctLevelID && isPlayer1()) {
        DataManager::get().addLives(-1);
        log::debug("{} lives remaining", DataManager::get().getLives());
    }
    PlayerObject::playerDestroyed(noEffects);
}
