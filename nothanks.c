#include <iostream>
using namespace std;
#include "nothanks.h"

bool gamestate_t::has(int p, int j)
{
    return (this->cards[p] >> j) & 1;
}

bool small_gamestate_t::has(int p, int j)
{
    return (this->cards[p] >> j) & 1;
}

int gamestate_t::num_cards_left()
{
    return std::popcount(this->cards[2]);
}

int A073642(uint64_t n)
{
    return __popcount(n & 0xAAAAAAAAAAAAAAAA) +
           (__popcount(n & 0xCCCCCCCCCCCCCCCC) << 1) +
           (__popcount(n & 0xF0F0F0F0F0F0F0F0) << 2) +
           (__popcount(n & 0xFF00FF00FF00FF00) << 3) +
           (__popcount(n & 0xFFFF0000FFFF0000) << 4) +
           (__popcount(n & 0xFFFFFFFF00000000) << 5);
}

int score_of_hand(u64 hand)
{
    return A073642(hand & ~(hand << 1));
}

void gamestate_t::print() {
    printf("P1 ");
    printf("(pennies: %d) cards: ", this->pennies[0]);
    for (auto i = 3; i <= 35; i++)
    {
        if (this->has(0, i))
            printf("%d, ", i);
    }
    printf("\n");
    printf("P2 ");
    printf("(pennies: %d) cards: ", this->pennies[1]);
    for (auto i = 3; i <= 35; i++)
    {
        if (this->has(1, i))
            printf("%d, ", i);
    }
    printf("\n");
    printf("Offer (pennies: %d) card: %d\n", this->pennies[2], this->offer);
}

void small_gamestate_t::print()
{
    printf("P1 ");
    printf("(pennies: %d) cards: ", this->p1pennies);
    for (auto i = 3; i <= 35; i++)
    {
        if (this->has(0, i))
            printf("%d, ", i);
    }
    printf("\n");
    printf("P2 ");
    printf("(pennies: %d) cards: ", 22 - this->p1pennies);
    for (auto i = 3; i <= 35; i++)
    {
        if (this->has(1, i))
            printf("%d, ", i);
    }
    printf("\n");
}