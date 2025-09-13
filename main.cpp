#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <ranges>
#include <tuple>
#include <sys/resource.h>
#include "nothanks.h"
#include "strats.cpp"
#include "negamax.cpp"
#include "minimax.cpp"
#include "bot.cpp"

std::tuple<int, int> play(Strategy fp0, Strategy fp1)
{
    std::random_device rd;
    std::mt19937 g(rd());

    gamestate_t gs = {
        .cards = {0, 0, 0b111111111111111111111111111111111000ul},
        .pennies = {INITIAL_PENNIES, INITIAL_PENNIES, 0},
    };

    std::array<uint8_t, 33> deck;
    std::iota(deck.begin(), deck.end(), 3);
    std::ranges::shuffle(deck, g);

    int player = PLAYER_A;
    int idx = 32;

    while (idx > 8)
    {
        gs.offer = deck[idx];
        printf("\n");
        gs.print();
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

float compare(Strategy fp0, Strategy fp1, int games = 20)
{
    int n = games;
    int score = 0;
    while (games--)
    {
        printf("games left %d\n", games);
        auto [a, b] = play(fp0, fp1);
        score += a < b;
    }
    // printf("\n");
    return (float)score / n;
}

int main(int argc, char **argv)
{
    const rlim_t kStackSize = 32 * 1024 * 1024; // min stack size = 16 MB
    struct rlimit rl;
    int result;

    result = getrlimit(RLIMIT_STACK, &rl);
    if (result == 0)
    {
        if (rl.rlim_cur < kStackSize)
        {
            rl.rlim_cur = kStackSize;
            result = setrlimit(RLIMIT_STACK, &rl);
            if (result != 0)
            {
                fprintf(stderr, "setrlimit returned result = %d\n", result);
            }
        }
    }

    // auto [a, b] = play(bot, hand_crafted);
    auto [a, b] = play(hand_crafted, bot2);
    printf("p1 score: %d\n", a);
    printf("p2 score: %d\n", b);

    // auto score = compare(hand_crafted, bot2);
    // printf("%s beats %s w.p. %f\n", "hand", "bot", score);

    // for (auto [n0, p0] : strats)
    // {
    //     float score;
    //     score = compare(bot, p0);
    //     printf("bot beats %s w.p. %f\n", n0, score);
    //     score = compare(p0, bot);
    //     printf("%s beats bot w.p. %f\n", n0, score);
    //     // score = compare(hand_crafted2, p0);
    //     // printf("hand_crafted2 beats %s w.p. %f\n", n0, score);
    //     // for (auto [n1, p1] : strats)
    //     // {
    //     //     auto score = compare(p0, p1);
    //     //     printf("%s beats %s w.p. %f\n", n0, n1, score);
    //     // }
    // }

    return 0;
}
