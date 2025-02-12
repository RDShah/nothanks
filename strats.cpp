#include <iostream>
using namespace std;

#include "nothanks.h"

bool human(gamestate_t gs, int player)
{
    std::string x;
    gs.print_state();
    printf("Take the card? ");
    cin >> x;
    printf("\n");
    return (x == "n") ? NO_THANKS : YES_PLEASE;
}

bool always_takes_card(gamestate_t gs, int player)
{
    return YES_PLEASE;
}
bool always_no_thanks(gamestate_t gs, int player) { return NO_THANKS; }

bool high_cards_are_scary(gamestate_t gs, int player) { return gs.offer > 31 ? NO_THANKS : YES_PLEASE; }

bool ratio_2_strat(gamestate_t gs, int player)
{
    return gs.offer > 2 * gs.pennies[2] ? NO_THANKS : YES_PLEASE;
}

bool ratio_3_strat(gamestate_t gs, int player)
{
    return gs.offer > 3 * gs.pennies[2] ? NO_THANKS : YES_PLEASE;
}

bool ratio_11_to_1_strat(gamestate_t gs, int player)
{
    int cardsleft = std::popcount(gs.cards[2]);
    float frac = (float)(cardsleft - 9.0) / (33.0 - 9.0);
    float pennyvalue = 1 + 10 * frac;
    return gs.offer > pennyvalue * gs.pennies[2] ? NO_THANKS : YES_PLEASE;
}

bool ratio_21_to_1_strat(gamestate_t gs, int player)
{
    int cardsleft = std::popcount(gs.cards[2]);
    float frac = (float)(cardsleft - 9.0) / (33.0 - 9.0);
    float pennyvalue = 1 + 20 * frac;
    return gs.offer > pennyvalue * gs.pennies[2] ? NO_THANKS : YES_PLEASE;
}

bool take_runs_and_small_cards(gamestate_t gs, int player)
{
    int card = gs.offer;
    if (card < 17)
        return YES_PLEASE;
    if ((gs.cards[player] >> (card + 1)) & 1 ||
        (gs.cards[player] >> (card - 1)) & 1)
    {
        return YES_PLEASE;
    }
    if (card < 30 && gs.pennies[2] > 2)
    {
        return YES_PLEASE;
    }
    return card > 3 * gs.pennies[2] ? NO_THANKS : YES_PLEASE;
}

#define named_strat(strat) std::make_tuple(#strat, strat)
auto strats = {
    // named_strat(always_no_thanks),
    // named_strat(always_takes_card),
    // named_strat(ratio_2_strat),
    named_strat(ratio_3_strat),
    named_strat(ratio_21_to_1_strat),
    named_strat(ratio_11_to_1_strat),
    // named_strat(high_cards_are_scary)
    named_strat(take_runs_and_small_cards),
};