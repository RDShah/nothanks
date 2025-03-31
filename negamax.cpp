#pragma once
#include <iostream>
using namespace std;
#include <cstdlib>
#include <tuple>
#include <cassert>
#include "nothanks.h"

/** 
 * P2 takes the card and the pennies
 * It becomes P2's turn so this also swaps p1 and p2
 */
small_gamestate_t p2_take(small_gamestate_t gs, uint8_t card, int pennies) {
    auto p1_cards = gs.cards[0];
    auto p2_cards = gs.cards[1];
    auto p2_new_cards = p2_cards | (1ul << card);
    gs.cards[0] = p2_new_cards;
    gs.cards[1] = p1_cards;

    gs.p1pennies = 22 - gs.p1pennies + pennies;
    return gs;
}

/**
 * P2 pays the pennies to decline the card
 */
small_gamestate_t p2_give(small_gamestate_t gs, uint8_t card, int pennies) {
    gs.cards[0] |= (1ul << card);
    gs.p1pennies += pennies;
    return gs;
}

/**
 * Finds the best move given the remaining deck (including face down cards)
 * returns [p1 wins, number of pennies p1 should be willing to pay to decline card]
 * assumes the end of the deck is marked with a 0
 */
std::tuple<bool, int> negamax(small_gamestate_t gs, uint8_t* deck) {
    assert( 0 <= gs.p1pennies && gs.p1pennies <= 22 );

    if(deck[0] == 0) {
        auto p1score = score_of_hand(gs.cards[0]) - gs.p1pennies;
        auto p2score = score_of_hand(gs.cards[1]) - 22 + gs.p1pennies;
        return std::tuple(p1score < p2score, 0);
    }

    int p2pennies = 22 - gs.p1pennies;
    int p1pennies = gs.p1pennies;
    int max_pennies = std::min(p1pennies, p2pennies + 1);

    int p2_should_take_this_or_more = max_pennies + 1;
    int p2_should_give_this_or_less = -1;

    while (p2_should_give_this_or_less + 2 <= p2_should_take_this_or_more)
    {
        // k is the candidate number of pennies p1 should be willing to pay
        int k = (p2_should_give_this_or_less + p2_should_take_this_or_more) / 2;

        // add heuristic for deciding which to do first?

        // k needs to be at least 1 for p2 to be able to take
        if (k > 0 && p2_should_give_this_or_less != k)
        {
            auto [p2_wins, _] = negamax(p2_take(gs, deck[0], k), deck + 1);
            if(p2_wins) p2_should_take_this_or_more = k;
        }

        // k needs to be at most p2pennies for p2 to be able to pay
        if(k <= p2pennies && p2_should_take_this_or_more != k) { 
            auto [p1_wins, _] = negamax(p2_give(gs, deck[0], k), deck + 1);
            if(!p1_wins) p2_should_give_this_or_less = k;
        }

        if (p2_should_take_this_or_more != k && p2_should_give_this_or_less != k)
            return std::tuple(true, k);
    }

    return std::tuple(false, 0);
}

void negamax_test() {
    std::random_device rd;
    std::mt19937 g(rd());

    uint8_t deck[34];
    for (auto i = 0; i < 33; i++)
        deck[i] = i + 3;
    deck[33] = 0;

    std::ranges::shuffle(deck, g);

    // this should be 24, not 14 because
    // thats how many cards are in the deck
    // but this function is only fast for
    // around 13-16 cards.
    for (auto i = 14; i < 33; i++)
        deck[i] = 0;

    printf("Deck: ");
    for (auto i = 0; i < 33; i++)
        printf("%d ", deck[i]);
    printf("\n");

    small_gamestate_t gs = {
        .cards = {0, 0},
        .p1pennies = 11,
    };

    // via a strategy stealing argument, p2 always wins
    // so we introduce asymmetry by making the first move manually
    gs = p2_give(gs, deck[0], deck[0] / 7);
    uint8_t *deck2 = ((uint8_t *)deck) + 1;

    gs.print();

    auto [p1w, move] = negamax(gs, deck2);

    printf("if p1 does %d, then %d\n", move, p1w);
}