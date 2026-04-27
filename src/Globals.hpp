// for non-constant global variables
#pragma once

#include <Geode/Geode.hpp>
#include <stack>
#include <vector>

using namespace geode::prelude;

class ChallengeLayer;

namespace Challenge {
    inline int currentLevelIndex { 0 };
    inline std::stack<int, std::vector<int>> currentLevelID {};
    inline int correctLevelID { 0 };
    inline bool currentLevelDownloadFailed { false };
    inline bool isPlaying { false };

    inline ChallengeLayer* currentChallengeLayer { nullptr };

    inline bool inCorrectLevel() { return Challenge::currentChallengeLayer && currentLevelID.size() == 1 && currentLevelID.top() == correctLevelID; }
}
