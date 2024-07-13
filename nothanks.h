#pragma once
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <ranges>
#include <tuple>

typedef uint64_t u64;

struct gamestate_t {
    u64 cards[3];
    uint8_t pennies[4];
};

typedef bool Strategy(gamestate_t, int, int);
