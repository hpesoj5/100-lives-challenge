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
        inline const std::string MAIN_MENU_PREFIX { "main-menu-page-"s };
        inline const std::string BG_MENU_PREFIX { "bg-menu-page-"s };
        inline const std::string BG_ISLAND_PREFIX { "bg-island-page-"s };
        inline const std::string LEVEL_BTN_PREFIX { "level-button-"s };
        inline const std::string LEVEL_LABEL_PREFIX { "level-label-"s };

        inline constexpr ccColor3B BG_COLOR { 76, 76, 76, };
        inline constexpr float ACTION_MENU_POSITION_PERCENT { 0.2f };

        inline constexpr float CORNER_PADDING{ 36.f };
        inline constexpr float EXIT_PADDING { 24.f };
        inline constexpr float EXIT_HEIGHT { 125.f };

        inline constexpr float STATS_LABEL_SCALE { 0.5f };
        // inline constexpr float LEVEL_SPACING_PERCENT { 0.15f };
        inline constexpr float LEVEL_LABEL_SPACING_PERCENT { 0.2f };  // as a percentage of the button's content height
        inline constexpr float LABEL_SCALE_BIG { 0.5f };
        inline constexpr float LABEL_SCALE_SMALL { 0.3f };
        inline constexpr size_t LABEL_THRESHOLD { 10 };

        inline constexpr float PAGES_INDICATOR_POSITION_PERCENT { 0.075f };

        inline constexpr float BG_ISLAND_YPOSITION { 0.4f };
        inline constexpr std::array<Point, 5> LEVEL_BTN_POSITION {
            Point{ 0.2, BG_ISLAND_YPOSITION + 0.05f },
            Point{ 0.36f, BG_ISLAND_YPOSITION + 0.01f} ,
            Point{ 0.475f, BG_ISLAND_YPOSITION + 0.15f },
            Point{ 0.61f, BG_ISLAND_YPOSITION + 0.03f },
            Point{ 0.78f, BG_ISLAND_YPOSITION + 0.12f }
        };

        // image is 1729 x 129
        inline constexpr float TITLE_SCALE { 1.1f };
        inline constexpr float TITLE_YPOSITION { 0.82f };

        inline constexpr float STATS_MENU_YPOSITION { 0.08f };
        inline constexpr float STATS_MENU_LABEL_GAP { 0.08f };
    }

    namespace Challenge {
        inline constexpr int NUM_LEVELS { 100 };
        inline constexpr size_t NUM_PAGES { 20 };
        inline constexpr int NUM_LIVES { 100 };
        // inline constexpr int NUM_SKIPS { 3 };
        inline constexpr int NUM_SKIPS { 100 };
    }
}
