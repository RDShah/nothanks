#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <tuple>
#include <array>

typedef uint64_t u64;

struct gamestate_t
{
    u64 cards[3];
    uint8_t pennies[4];
};

typedef bool Strategy(gamestate_t, int, int);

bool ratio_2_strat(gamestate_t gs, int player, int card)
{
    return card > 2 * gs.pennies[2];
}

bool ratio_3_strat(gamestate_t gs, int player, int card)
{
    return card > 3 * gs.pennies[2];
}

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
        .cards = {0, 0, 0},
        .pennies = {11, 11, 0},
    };

    for (int i = 3; i <= 35; i++)
        gs.cards[2] ^= 1 << i;

    int deck[] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
                  14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
                  25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35};
    std::shuffle((int *)deck, (int *)(deck + 33), g);

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

    return std::tuple<int, int>(score(gs.cards[0]), score(gs.cards[1]));
}

int main(int argc, char **argv)
{
    auto [a, b] = play(ratio_2_strat, ratio_3_strat);
    printf("%d %d\n", a, b);
}