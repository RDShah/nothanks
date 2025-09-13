#include <cstdint>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <random>
#include <complex>
#include <map>
#include <string>
#include <fstream>
#include <cassert>
#include <bitset>
#include "nothanks.h"
using namespace std;

#define DECK_SIZE (10)
#define BUF_SIZE (1 << (DECK_SIZE + 1))
#define TOTAL_PENNIES (2 * INITIAL_PENNIES)
#define SHUFFLE_COUNT (300)
#define SUBSET_COUNT (300)

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint8_t u8;
typedef int8_t i8;
typedef std::tuple<u8, u8, u8> u8triplet;

void expand(u64 (&cards)[2], u32 hand, uint8_t *deck)
{
    for (int i = 0; i < DECK_SIZE; i++)
        cards[(hand >> i) & 1] |= (1ul << deck[i]);
}

const u32 mask = 0b111111111111111111111111;

int estimate_penny_requirement(u32 hand, u8 num, u8 (&deck)[33])
{
    float total = 0;
    for (int ss = 0; ss < SHUFFLE_COUNT; ss++)
    {
        // if(ss < 3){
        // for(int i = 0; i < 33; i++) printf("%d ", deck[i]);
        // printf("\n");}
        shuffle(deck + num, deck + 33, default_random_engine(rand()));
        for (int _ = 0; _ < SUBSET_COUNT; _++)
        {
            u32 completion = hand | (~((1 << num) - 1)) & mask & rand();
            u64 cards[2] = {0ul, 0ul};
            expand(cards, completion, deck);
            auto diff = score_of_hand(cards[0]) - score_of_hand(cards[1]);
            auto p = (diff + TOTAL_PENNIES) / 2 + 1;
            auto val = p <= 0 ? 0 : p > TOTAL_PENNIES + 1 ? TOTAL_PENNIES + 1
                                                          : p;

            total += val;
        }
    }

    return total / (SHUFFLE_COUNT * SUBSET_COUNT);
}

int compute_bid(u32 hand, u8 num, u8 (&deck)[33])
{
    int a = estimate_penny_requirement(hand | (1 << num), num + 1, deck);
    int b = estimate_penny_requirement(hand, num + 1, deck);
    if (a < b)
        return 0;
    return (b - a + 1) / 2;
}

bool contains(u8 *s, u8 *t, u8 v)
{
    for (; s < t; s++)
        if (*s == v)
            return true;

    return false;
}

bool bot(gamestate_t gs, int p)
{
    if (p == 1)
    {
        gs = (gamestate_t){
            .cards = {gs.cards[1], gs.cards[0], gs.cards[2]},
            .pennies = {gs.pennies[1], gs.pennies[0], gs.pennies[2]},
            .offer = gs.offer};
    }

    u8 deck[33];
    int idx = 0;
    // int num = 33 - gs.num_cards_left();
    u32 hand = 0;
    for (int i = 3; i <= 35; i++)
    {
        if (gs.has(1, i))
            hand |= (1 << idx);
        if (!gs.has(2, i))
            deck[idx++] = i;
    }
    int num = idx;
    deck[idx++] = gs.offer;
    for (int i = 3; i <= 35; i++)
    {
        if (!contains(deck, deck + idx, i))
            deck[idx++] = i;
    }

    int req_to_give = estimate_penny_requirement(hand | (1 << num), num + 1, deck);
    int req_to_take = estimate_penny_requirement(hand, num + 1, deck);

    // printf("%d < %d\n", req_to_give, req_to_take);

    for (int pretend = gs.pennies[0]; pretend <= TOTAL_PENNIES + 1; pretend++)
    {
        if (gs.pennies[2] + pretend >= req_to_take)
            return YES_PLEASE;

        if (pretend > req_to_give)
            return NO_THANKS;
    }

    assert(false);
    return NO_THANKS;
}

// int main()
// {
//     std::random_device rd;
//     u8 deck[33];
//     for (auto i = 0; i < 33; i++)
//         deck[i] = i + 3;
//     shuffle(deck + 0, deck + 33, default_random_engine(-1));

//     gamestate_t gs = {
//         .cards = {0, 0, 0b111111111111111111111111111111111000ul},
//         .pennies = {11, 11, 0},
//     };

//     cout << bot(gs, 0) << "\n";
// }