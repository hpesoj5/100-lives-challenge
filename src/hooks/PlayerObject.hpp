#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

class $modify(ChallengePlayerObject, PlayerObject) {
public:
    void playerDestroyed(bool noEffects);
};
