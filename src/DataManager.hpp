#pragma once

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

struct SaveData {
    std::vector<int> levels {};
    std::vector<int> levelStatus {};
    int completedLevels {};
    int lives {};
    int skips {};
    int bestScore {};
};

template<>
struct matjson::Serialize<SaveData> {
    static Result<SaveData> fromJson(matjson::Value const& value) {
        GEODE_UNWRAP_INTO(std::vector<int> levels, value["levels"].as<std::vector<int>>());
        GEODE_UNWRAP_INTO(std::vector<int> levelStatus, value["levelStatus"].as<std::vector<int>>());
        GEODE_UNWRAP_INTO(int completedLevels, value["completedLevels"].asInt());
        GEODE_UNWRAP_INTO(int lives, value["lives"].asInt());
        GEODE_UNWRAP_INTO(int skips, value["skips"].asInt());
        GEODE_UNWRAP_INTO(int bestScore, value["bestScore"].asInt());
        return Ok(SaveData{ levels, levelStatus, completedLevels, lives, skips, bestScore });
    }

    static matjson::Value toJson(SaveData const& value) {
        auto obj = matjson::Value();
        obj["levels"] = value.levels;
        obj["levelStatus"] = value.levelStatus;
        obj["completedLevels"] = value.completedLevels;
        obj["lives"] = value.lives;
        obj["skips"] = value.skips;
        obj["bestScore"] = value.bestScore;
        return obj;
    }
};

class DataManager final : public LevelManagerDelegate, LevelDownloadDelegate {
public:
    DataManager();
    ~DataManager();

    void loadLevels(CCObject* sender, int page);
    void loadLevelsFinished(CCArray* levels, char const* key) override;
    void loadLevelsFailed(char const* key) override;

    void levelDownloadFinished(GJGameLevel* level) override;
    void levelDownloadFailed(int response) override;

    GJGameLevel* getLevel(size_t index) { return m_levels[index].data(); }
    std::string const& getLevelName(size_t index) const { return m_levels[index]->m_levelName; }

    std::vector<Ref<GJGameLevel>>& get() { return m_levels; }
    size_t count() const { return m_levels.size(); }

    int getCompletedLevels() const { return m_data.completedLevels; }
    int getLives() const { return m_data.lives; }
    int getSkips() const { return m_data.skips; }
    bool isRunOver() const { return m_data.lives <= 0; }

    void incrementCompletedLevels() { ++m_data.completedLevels; }
    void addLives(int x) { m_data.lives += x; }
    void decrementSkips() { --m_data.skips; }
    void saveToDisk();
    void restoreFromDisk();
    void notifyLevelsRestored(bool restored) const;

    void setChallengeLayer(CCObject* challengeLayer) { m_challengeLayer = challengeLayer; }

private:
    SaveData m_data;
    std::unordered_map<int, int> m_levelsToDownload;
    std::vector<Ref<GJGameLevel>> m_levels;
    CCObject* m_sender;
    CCObject* m_challengeLayer;
    LevelManagerDelegate* m_prevLMD;
    LevelDownloadDelegate* m_prevLDD;
    int m_pageCount;
};
