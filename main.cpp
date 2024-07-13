#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <ranges>
#include <tuple>
#include "nothanks.h"
#include "strats.cpp"

int score(u64 hand)
{
    int s = 0;
    for (int i = 3; i <= 35; i++)
    {
        bool hasi = (hand >> i) & 1;
        bool hasj = (hand >> (i - 1)) & 1;
        if (hasi && !hasj)
            s += i;
    }
    return s;
}

std::tuple<int, int> play(Strategy fp1, Strategy fp2)
{
    std::random_device rd;
    std::mt19937 g(rd());

    gamestate_t gs = {
        .cards = {0, 0, 0b111111111111111111111111111111111000L},
        .pennies = {11, 11, 0},
    };

    std::array<uint8_t, 33> deck;
    std::iota(deck.begin(), deck.end(), 3);
    std::ranges::shuffle(deck, g);

    int p = 0;
    int idx = 32;

    while (idx > 8)
    {
        int offer = deck[idx];
        bool nothanks = (*(p ? fp1 : fp2))(gs, p, offer);
        if (nothanks && gs.pennies[p])
        {
            gs.pennies[p] -= 1;
            gs.pennies[2] += 1;
        }
        else
        {
            gs.cards[p] ^= (1 << offer);
            gs.cards[2] ^= (1 << offer);
            gs.pennies[p] += gs.pennies[2];
            gs.pennies[2] = 0;
            idx--;
        }
        p ^= 1;
    }

    return std::make_tuple(score(gs.cards[0]), score(gs.cards[1]));
}

int compare(Strategy fp1, Strategy fp2, int games = 10000)
{
    int score = 0;
    while (games--)
    {
        auto [a, b] = play(ratio_2_strat, ratio_3_strat);
        score += a < b;
        score -= a > b;
    }
    return score;
}

int main(int argc, char **argv)
{
    int wins = 0;
    int ties = 0;
    int games = 10000;
    for (int i = 0; i < games; i++)
    {
        auto [a, b] = play(ratio_2_strat, ratio_3_strat);
        wins += a < b;
        ties += a == b;
    }
    printf("%d/%d (ties=%d)\n", wins, games, ties);
}