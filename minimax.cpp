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

#define NUM_SAMPLES (5)

float prob_of_p1_win_with_offer(gamestate_t gs);
float prob_of_p1_win(gamestate_t gs);

struct StateHash
{
    auto operator()(const gamestate_t &p) const -> u64
    {
        u64 a = p.cards[0] * ((1 << 31) - 1);
        u64 b = p.cards[1] * ((1 << 19) - 1);
        u64 c = p.pennies[0];
        return ((a + b) << 5) | c;
    }
};

std::unordered_map<gamestate_t, float, StateHash> cache(1 << 20);

float prob_of_p1_win(gamestate_t gs)
{
    printf("prob_of_p1_win\n");
    gs.print();

    if (gs.num_cards_left() <= 9)
    {
        auto [a, b] = evaluate_game(gs);
        return (float)(a < b);
    }

    auto cached_it = cache.find(gs);
    if (cached_it != cache.end())
        return cache[gs];

    uint8_t a[33] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35};
    shuffle(a, a + 33, default_random_engine(1));

    uint8_t offers[NUM_SAMPLES];
    int i = 0;
    for (int j = 0; j < 33; j++)
    {
        if (gs.has(2, a[j]))
            offers[i++] = a[j];
        if (i == NUM_SAMPLES)
            break;
    }

    assert(i == NUM_SAMPLES);

    float total = 0;
    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        gs.offer = offers[i];
        total += prob_of_p1_win_with_offer(gs);
    }
    return total / NUM_SAMPLES;
}

float prob_of_p1_win_with_offer(gamestate_t gs)
{
    uint8_t p1 = gs.pennies[0];
    uint8_t p2 = gs.pennies[1];
    u64 h1 = gs.cards[0];
    u64 h2 = gs.cards[1];
    uint8_t offer = gs.offer;

    printf("prob_of_p1_win_with_offer\n");
    gs.print();
    // printf("testing bids up to min of %d %d @ depth %d\n", p1, p2 + 1, gs.num_cards_left());

    float prob = 0;
    
    int lo = 0;
    int hi = std::min((int)p1, (int)p2 + 1);

    float p2_nothanks;
    float p2_yespleas;

    while(lo < hi - 1)
    {
        int bid = (lo + hi)/2;
        gamestate_t case1_gs = {
            .cards = {h1 ^ (1 << offer), h2, gs.cards[2] ^ (1 << offer)},
            .pennies = {p1 + bid, p2 - bid, 0}};

        gamestate_t case2_gs = {
            .cards = {h2 ^ (1 << offer), h1, gs.cards[2] ^ (1 << offer)},
            .pennies = {p2 + bid, p1 - bid, 0}};

        printf("bid %d case 1\t", bid);
        p2_nothanks = bid <= p2 ? prob_of_p1_win(case1_gs) : 1.;
        printf("bid %d case 2\t", bid);
        p2_yespleas = 1. - prob_of_p1_win(case2_gs);
        printf("result %f %f\n", p2_nothanks, p2_yespleas);

        if (p2_nothanks > p2_yespleas)
            hi = bid;
        else
            lo = bid;
    }

    return std::min(p2_nothanks, p2_yespleas);

    // for (int bid = 0; bid <= std::min((int)p1, (int)p2 + 1); bid++)
    // {
    //     gamestate_t case1_gs = {
    //         .cards = {h1 ^ (1 << offer), h2, gs.cards[2] ^ (1 << offer)},
    //         .pennies = {p1 + bid, p2 - bid, 0}};

    //     gamestate_t case2_gs = {
    //         .cards = {h2 ^ (1 << offer), h1, gs.cards[2] ^ (1 << offer)},
    //         .pennies = {p2 + bid, p1 - bid, 0}};

    //     float p2_nothanks = bid <= p2 ? prob_of_p1_win(case1_gs) : 1.;
    //     float p2_yespleas = 1. - prob_of_p1_win(case2_gs);
    //     float a = std::min(p2_nothanks, p2_yespleas);
    //     prob = std::max(a, prob);
    // }
    // return prob;
}

int compute_good_bid(gamestate_t gs)
{
    assert(gs.pennies[2] == 0);
    uint8_t p1 = gs.pennies[0];
    uint8_t p2 = gs.pennies[1];

    float best_prob = 0;
    int best_bid = 0;
    for (int bid = 0; bid <= std::min((int)p1, (int)p2 + 1); bid++)
    {
        float prob = prob_of_p1_win_with_offer(gs);
        if (prob > best_prob)
        {
            best_bid = bid;
            best_prob = prob;
        }
    }
    return best_prob;
}

bool bot2(gamestate_t gs, int p)
{
    uint8_t pot = gs.pennies[2];
    if (pot % 2 == 0)
    {
        if (p == 1)
        {
            gs = (gamestate_t){
                .cards = {gs.cards[1], gs.cards[0], gs.cards[2]},
                .pennies = {gs.pennies[1] + pot / 2, gs.pennies[0] + pot / 2, 0},
                .offer = gs.offer};
        }
        else
        {
            gs = (gamestate_t){
                .cards = {gs.cards[0], gs.cards[1], gs.cards[2]},
                .pennies = {gs.pennies[0] + pot / 2, gs.pennies[1] + pot / 2, 0},
                .offer = gs.offer};
        }

        auto bid = compute_good_bid(gs);
        return bid * 2 > pot ? NO_THANKS : YES_PLEASE;
    }
    else
    {

        if (p == 0)
        {
            gs = (gamestate_t){
                .cards = {gs.cards[1], gs.cards[0], gs.cards[2]},
                .pennies = {gs.pennies[1] + pot / 2 + 1, gs.pennies[0] + pot / 2, 0},
                .offer = gs.offer};
        }
        else
        {
            gs = (gamestate_t){
                .cards = {gs.cards[0], gs.cards[1], gs.cards[2]},
                .pennies = {gs.pennies[0] + pot / 2 + 1, gs.pennies[1] + pot / 2, 0},
                .offer = gs.offer};
        }

        auto bid = compute_good_bid(gs);
        return bid * 2 > pot ? NO_THANKS : YES_PLEASE;
    }
}