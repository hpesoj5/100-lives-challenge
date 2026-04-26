#include "../DataManager.hpp"
#include "../Globals.hpp"
#include "PlayLayer.hpp"

void ChallengePlayLayer::levelComplete() {
    PlayLayer::levelComplete();
    if (Challenge::currentChallengeLayer && Challenge::currentLevelID.top() == Challenge::correctLevelID && !m_isPracticeMode && !m_isTestMode) {
        auto areCoinsVerified { m_level->m_coinsVerified.value() };
        auto levelID = m_level->m_levelID;
        auto levelIDStr { std::to_string(levelID) };
        std::string levelIDKey { m_level->getCoinKey(levelID) };
        std::string weeklySuffix { levelIDKey.size() != 2 * levelIDStr.size() + 1 ? levelIDKey.substr(2 * levelIDStr.size() + 1, 7) : "" };
        auto coins { m_level->m_coins };
        int collectedCoins {};
        auto verifiedCoins { GameStatsManager::sharedState()->m_verifiedUserCoins };
        auto pendingCoins { GameStatsManager::sharedState()->m_pendingUserCoins };

        for (int i {}; i < coins; ++i) {
            if (areCoinsVerified) {
                if (verifiedCoins->objectForKey(levelIDStr + "_" + std::to_string(i + 1) + weeklySuffix)) ++collectedCoins;
            }
            else {
                if (pendingCoins->objectForKey(levelIDStr + "_" + std::to_string(i + 1) + weeklySuffix)) ++collectedCoins;
            }
        }

        log::debug("Level {} complete", Challenge::currentLevelIndex + 1);
        log::debug("Collected coins: {}", collectedCoins);
        DataManager::get().setLevelComplete(Challenge::currentLevelIndex, collectedCoins);
    }
}

