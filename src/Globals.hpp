// for non-constant global variables
#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class ChallengeLayer;

namespace Challenge {
    inline bool skipButtonEnabled { false };
    inline int currentLevelIndex { 0 };

    inline ChallengeLayer* currentChallengeLayer { nullptr };
}
