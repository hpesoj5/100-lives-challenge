#pragma once

#include <Geode/Geode.hpp>
#include <array>
#include <string>

using namespace geode::prelude;
using namespace std::string_literals;

struct Point {
    float x {};
    float y {};
};

struct LevelInfo : public CCObject {
    size_t index;
    int ID;

    LevelInfo(size_t _index, int _ID) : index { _index }, ID { _ID } {
        autorelease();
    }
};
namespace Constants {
    namespace Menu {
        inline constexpr std::string MAIN_MENU_PREFIX { "main-menu-page-"s };
        inline constexpr std::string LEVEL_BTN_PREFIX { "level-button-"s };
        inline constexpr std::string LEVEL_LABEL_PREFIX { "level-label-"s };

        inline constexpr float ACTION_MENU_POSITION_PERCENT { 0.2f };

        inline constexpr float EXIT_PADDING { 24.f };
        inline constexpr float EXIT_HEIGHT { 125.f };

        inline constexpr float LEVEL_SPACING_PERCENT { 0.15f };
        inline constexpr float LEVEL_LABEL_SPACING_PERCENT { 0.2f };  // as a percentage of the button's content height
        inline constexpr float LABEL_SCALE_BIG { 0.4f };
        inline constexpr float LABEL_SCALE_SMALL { 0.2f };
        inline constexpr size_t LABEL_THRESHOLD { 10 };

        inline constexpr float PAGES_INDICATOR_POSITION_PERCENT { 0.075f };

        inline constexpr std::array<std::array<Point, 5>, 2> LEVEL_BTN_POSITION {
            std::array<Point, 5>{
                Point{ 0.5f - 2 * LEVEL_SPACING_PERCENT, 0.4f },
                Point{ 0.5f - LEVEL_SPACING_PERCENT, 0.4f },
                Point{ 0.5f, 0.4f },
                Point{ 0.5f + LEVEL_SPACING_PERCENT, 0.4f },
                Point{ 0.5f + 2 * LEVEL_SPACING_PERCENT, 0.4f },
            },
            std::array<Point, 5>{
                Point{ 0.5f - 2 * LEVEL_SPACING_PERCENT, 0.4f },
                Point{ 0.5f - LEVEL_SPACING_PERCENT, 0.4f },
                Point{ 0.5f, 0.4f },
                Point{ 0.5f + LEVEL_SPACING_PERCENT, 0.4f },
                Point{ 0.5f + 2 * LEVEL_SPACING_PERCENT, 0.4f },
            },
        };
    }

    namespace Challenge {
        inline constexpr int NUM_LEVELS { 100 };
        inline constexpr size_t NUM_PAGES { 20 };
        inline constexpr int NUM_LIVES { 100 };
        inline constexpr int NUM_SKIPS { 3 };
    }
}
