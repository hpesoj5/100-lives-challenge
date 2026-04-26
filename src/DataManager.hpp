#pragma once

#include "Constants.hpp"
#include <Geode/Geode.hpp>
#include <vector>

using namespace geode::prelude;

enum class LevelStatus {
    locked = 0,
    completed = 1,
    inProgress = 2,
    skipped = 3,
    null,
};

inline constexpr LevelStatus intToLevelStatus(int x) {
    if (x < 0 || x > 3) return LevelStatus::null;
    return static_cast<LevelStatus>(x);
}

inline constexpr int levelStatusToInt(LevelStatus status) {
    return static_cast<int>(status);
}

struct ChallengeData {
    std::vector<bool> skipGiven { std::vector<bool>(Constants::Challenge::NUM_LEVELS) };
    std::vector<int> levels { std::vector<int>(Constants::Challenge::NUM_LEVELS) };
    std::vector<int> levelStatus { std::vector<int>(Constants::Challenge::NUM_LEVELS, static_cast<int>(LevelStatus::locked)) };
    std::vector<int> levelCoins { std::vector<int>(Constants::Challenge::NUM_LEVELS) };
    int completedLevels {};
    int lives { Constants::Challenge::NUM_LIVES };
    int skips { Constants::Challenge::NUM_SKIPS };
    bool isRunOver {};
    bool isRunOverAlertShown {};
    bool isRunWon {};
    bool isRunWonAlertShown {};
};

template<>
struct matjson::Serialize<ChallengeData> {
    static Result<ChallengeData> fromJson(matjson::Value const& value) {
        GEODE_UNWRAP_INTO(std::vector<bool> skipGiven, value["skipGiven"].as<std::vector<bool>>());
        GEODE_UNWRAP_INTO(std::vector<int> levels, value["levels"].as<std::vector<int>>());
        GEODE_UNWRAP_INTO(std::vector<int> levelStatus, value["levelStatus"].as<std::vector<int>>());
        GEODE_UNWRAP_INTO(std::vector<int> levelCoins, value["levelCoins"].as<std::vector<int>>());
        GEODE_UNWRAP_INTO(int completedLevels, value["completedLevels"].asInt());
        GEODE_UNWRAP_INTO(int lives, value["lives"].asInt());
        GEODE_UNWRAP_INTO(int skips, value["skips"].asInt());
        GEODE_UNWRAP_INTO(bool isRunOver, value["isRunOver"].asBool());
        GEODE_UNWRAP_INTO(bool isRunOverAlertShown, value["isRunOverAlertShown"].asBool());
        GEODE_UNWRAP_INTO(bool isRunWon, value["isRunWon"].asBool());
        GEODE_UNWRAP_INTO(bool isRunWonAlertShown, value["isRunWonAlertShown"].asBool());
        return Ok(ChallengeData{
            skipGiven,
            levels,
            levelStatus,
            levelCoins,
            completedLevels,
            lives,
            skips,
            isRunOver,
            isRunOverAlertShown,
            isRunWon,
            isRunWonAlertShown
        });
    }

    static matjson::Value toJson(ChallengeData const& value) {
        auto obj = matjson::Value();
        obj["skipGiven"] = value.skipGiven;
        obj["levels"] = value.levels;
        obj["levelStatus"] = value.levelStatus;
        obj["levelCoins"] = value.levelCoins;
        obj["completedLevels"] = value.completedLevels;
        obj["lives"] = value.lives;
        obj["skips"] = value.skips;
        obj["isRunOver"] = value.isRunOver;
        obj["isRunOverAlertShown"] = value.isRunOverAlertShown;
        obj["isRunWon"] = value.isRunWon;
        obj["isRunWonAlertShown"] = value.isRunWonAlertShown;
        return obj;
    }
};

class DataManager final : public LevelManagerDelegate, public LevelDownloadDelegate {
public:
    static DataManager& get() { static DataManager dataManager; return dataManager; }

    void loadLevels(CCObject* sender, int page);
    void loadLevelsFinished(CCArray* levels, char const* key) override;
    void loadLevelsFailed(char const* key) override;

    void levelDownloadFinished(GJGameLevel* level) override;
    void levelDownloadFailed(int response) override;

    GJGameLevel* getLevel(size_t index) { return m_levels[index]; }
    std::string const& getLevelName(size_t index) const { return m_levels[index]->m_levelName; }
    int getLevelID(size_t index) const { return m_levels[index]->m_levelID.value(); }

    std::vector<Ref<GJGameLevel>>& getLevelVector() { return m_levels; }
    size_t count() const { return m_levels.size(); }

    int getBestScore() const { return m_bestScore; }
    int getCompletedLevels() const { return m_data.completedLevels; }
    int getLives() const { return m_data.lives; }
    int getSkips() const { return m_data.skips; }
    bool hasRemainingSkips() const { return m_data.skips > 0; }
    bool isRunOver() const { return m_data.isRunOver; }
    bool isRunOverAlertShown() const { return m_data.isRunOverAlertShown; }
    bool isRunWon() const { return m_data.isRunWon; }
    bool isRunWonAlertShown() const { return m_data.isRunWonAlertShown; }

    void resetChallengeData();
    void deleteAllLevels();
    void addLives(int x) {
        if (!m_data.isRunOver) m_data.lives += x;
        if (m_data.lives <= 0) {
            m_data.isRunOver = true;
            saveToDisk();
            log::info("Challenge is over!");
        }
    }
    void decrementSkips() { if (m_data.skips > 0) --m_data.skips; }
    void saveToDisk();
    void restoreFromDisk();
    void notifyLevelsRestored(bool restored);

    void setLevelComplete(size_t n ,int numCoins);
    void setLevelSkipped(size_t n);
    bool rewardLevelSkip(size_t n);  // returns operation success
    void updateBestScore(int score);
    void setRunOverAlertShown() { m_data.isRunOverAlertShown = true; }
    void setRunWonAlertShown() { m_data.isRunWonAlertShown = true; }

private:
    DataManager();

    ChallengeData m_data;
    std::unordered_map<int, int> m_levelsToDownload;
    std::vector<Ref<GJGameLevel>> m_levels;
    CCObject* m_sender;
    LevelManagerDelegate* prev_LMD;
    LevelDownloadDelegate* prev_LDD;
    int m_pageCount;
    int m_bestScore;
};
