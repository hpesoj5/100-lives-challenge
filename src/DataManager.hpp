#pragma once

#include <cstdint>

class DataManager final {
public:
    DataManager() : m_completedLevels{}, m_lives{}, m_skips{} {}

    uint32_t getCompletedLevels() const { return m_completedLevels; }
    uint32_t getLives() const { return m_lives; }
    uint32_t getSkips() const { return m_skips; }
    bool isRunOver() const { return m_lives <= 0; }

    void incrementCompletedLevels() { ++m_completedLevels; }
    void addLives(int x) { m_lives += x; }
    void decrementSkips() { --m_skips; }
    void reset() {
        m_completedLevels = 0;
        m_lives = 0;
        m_skips = 0;
    }
    // void saveToDisk();
    // void restoreFromDisk();

private:
    uint32_t m_completedLevels;
    uint32_t m_lives;
    uint32_t m_skips;
};
