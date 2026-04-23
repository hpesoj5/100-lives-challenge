#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

class $modify(ChallengePlayLayer, PlayLayer) {
public:
    void levelComplete();
};
