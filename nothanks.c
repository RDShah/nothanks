#include <iostream>
#include <tuple>
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

std::tuple<int, int> evaluate_game(gamestate_t gs)
{
    int p0score = score_of_hand(gs.cards[0]) - gs.pennies[0];
    int p1score = score_of_hand(gs.cards[1]) - gs.pennies[1];
    return std::make_tuple(p0score, p1score);
}

bool gamestate_t::operator==(const gamestate_t &other) const
{
    return other.cards[0] == cards[0] && other.cards[1] == cards[1] && other.pennies[0] == pennies[0]; // & victim == other.victim;
}

void gamestate_t::print() {
    for(int p = 0; p < 2; p++) {
        int cs = score_of_hand(this->cards[p]);
        printf("P%d:: cs: %d | ts: %d | p: %d | c: ",
            p + 1,
            cs,
            cs - this->pennies[p],
            this->pennies[p]
        );
        for (auto i = 3; i <= 35; i++)
        {
            if (this->has(p, i))
                printf("%d, ", i);
        }
        printf("\n");
    }  
    printf("Offer p: %d | c: %d\n", this->pennies[2], this->offer);
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