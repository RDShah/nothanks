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
#include "negamax.cpp"

std::tuple<int, int> evaluate_game(gamestate_t gs)
{
    int p0score = score_of_hand(gs.cards[0]) - gs.pennies[0];
    int p1score = score_of_hand(gs.cards[1]) - gs.pennies[1];
    return std::make_tuple(p0score, p1score);
}

std::tuple<int, int> play(Strategy fp0, Strategy fp1)
{
    std::random_device rd;
    std::mt19937 g(rd());

    gamestate_t gs = {
        .cards = {0, 0, 0b111111111111111111111111111111111000ul},
        .pennies = {11, 11, 0},
    };

    std::array<uint8_t, 33> deck;
    std::iota(deck.begin(), deck.end(), 3);
    std::ranges::shuffle(deck, g);

    int player = PLAYER_A;
    int idx = 32;

    while (idx > 8)
    {
        gs.offer = deck[idx];
        auto decision = (*(player ? fp1 : fp0))(gs, player);
        if (decision == NO_THANKS && gs.pennies[player])
        {
            gs.pennies[player] -= 1;
            gs.pennies[IN_CENTER] += 1;
            player = player == PLAYER_A ? PLAYER_B : PLAYER_A;
        }
        else
        {
            gs.cards[player] ^= (1ul << gs.offer);
            gs.cards[IN_CENTER] ^= (1ul << gs.offer);
            gs.pennies[player] += gs.pennies[IN_CENTER];
            gs.pennies[IN_CENTER] = 0;
            idx--;
        }
    }

    return evaluate_game(gs);
}

float compare(Strategy fp0, Strategy fp1, int games = 10000)
{
    int n = games;
    int score = 0;
    while (games--)
    {
        auto [a, b] = play(fp0, fp1);
        score += a < b;
    }
    return (float)score / n;
}

int main(int argc, char **argv)
{
    // auto [a, b] = play(ratio_3_strat, human);
    // printf("p1 score: %d\n", a);
    // printf("p2 score: %d\n", b);

    for (auto [n0, p0] : strats)
    {
        // auto score = compare(hand_crafted, p0);
        // printf("hand_crafted beats %s w.p. %f\n", n0, score);
        // score = compare(hand_crafted2, p0);
        // printf("hand_crafted2 beats %s w.p. %f\n", n0, score);
        for (auto [n1, p1] : strats)
        {
            auto score = compare(p0, p1);
            printf("%s beats %s w.p. %f\n", n0, n1, score);
        }
    }

    return 0;
}
