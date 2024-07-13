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

int score_of_hand(u64 hand)
{
    hand = hand & ~(hand << 1);
    int total = 0;
    for (int i = 3; i < 35; i++)
        if ((hand >> i) & 1)
            total += i;

    return total;
}

std::tuple<int, int> evaluate_game(gamestate_t gs)
{
    int p0score = score_of_hand(gs.cards[0]) - gs.pennies[0];
    int p1score = score_of_hand(gs.cards[1]) - gs.pennies[1];
    return std::make_tuple(p0score, p1score);
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

    return evaluate_game(gs);
}

float compare(Strategy fp1, Strategy fp2, int games = 10000)
{
    int n = games;
    int score = 0;
    while (games--)
    {
        auto [a, b] = play(fp1, fp2);
        score += a < b;
        score -= a > b;
    }
    return (float)score / n;
}

int main(int argc, char **argv)
{
    for (auto [n1, p1] : strats)
        for (auto [n2, p2] : strats)
        {
            auto score = compare(p1, p2);
            printf("%s vs %s: %f\n", n1, n2, score);
        }
}