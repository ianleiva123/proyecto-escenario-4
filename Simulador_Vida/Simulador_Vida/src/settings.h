#pragma once
#include <cstdint>

constexpr int MAX_SPECIES = 4;
constexpr int MIN_SPECIES = 3;
constexpr int MIN_SIZE = 3;
constexpr int MAX_SIZE = 200;
constexpr int MAX_NAME_LENGTH = 24;
constexpr int MAX_NEIGHBORS = 8;
constexpr int RULE_COUNT = MAX_NEIGHBORS + 1;
constexpr int MIN_SPEED = 1;
constexpr int MAX_SPEED = 20;
constexpr int DEFAULT_HISTORY_LIMIT = 300;
constexpr int MAX_HISTORY_LIMIT = 1000;
constexpr int MAX_WORLDS = 32;
constexpr int MAX_SAVED_STATES = 10000;
constexpr long long MAX_SAVED_CELLS = 8000000;
constexpr int MAX_GENERATION = 1000000000;
constexpr int PERCENT_BASE = 100;
constexpr int PERCENT_STEP = 5;
constexpr int PATTERN_SIZE = 3;

enum class SpeciesId
{
    Empty,
    Worker,
    Scout,
    Guardian,
    Queen
};
enum class Shape
{
    Square,
    Circle,
    Triangle,
    Diamond
};

struct WorldSize
{
    int rows = 20;
    int columns = 25;
};
