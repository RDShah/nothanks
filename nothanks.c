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

int score_of_hand(u64 hand)
{
    hand = hand & ~(hand << 1);
    int total = 0;
    for (int i = 3; i <= 35; i++)
        if ((hand >> i) & 1)
            total += i;

    return total;
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