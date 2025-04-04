#pragma once
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <ranges>
#include <tuple>
#include <iostream>
using namespace std;

/**
 * a hand (a set of cards) is represented as a 64-bit integer
 * bit j is set iff card #j is in the hand
 * eg:
 * hand_has_card_10 = (hand >> 10) & 1;
 */
typedef uint64_t u64;

struct gamestate_t
{

    /**
     * cards[0] = hand of player 0
     * cards[1] = hand of player 1
     * cards[2] = set of cards still in deck
     */
    u64 cards[3];

    /**
     * pennies[0] = number of pennies held by player 0
     * pennies[1] = number of pennies held by player 1
     * pennies[2] = number of pennies in the middle
     */
    uint8_t pennies[3];

    /**
     * the card being offered
     */
    uint8_t offer;

    bool has(int p, int j);
    void print();
    int num_cards_left();
};

struct small_gamestate_t
{
    u64 cards[2];
    uint8_t p1pennies;
    bool has(int p, int j);
    void print();
};

int score_of_hand(u64 hand);

#define NO_THANKS (true)
#define YES_PLEASE (false)

#define IN_CENTER (2)
#define PLAYER_A (0) 
#define PLAYER_B (1)

    typedef bool Strategy(gamestate_t, int);
