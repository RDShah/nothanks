#include "nothanks.h"

bool always_takes_card(gamestate_t gs, int player) { return false; }
bool always_no_thanks(gamestate_t gs, int player) { return true; }

bool high_cards_are_scary(gamestate_t gs, int player) { return gs.offer > 31; }

bool ratio_2_strat(gamestate_t gs, int player)
{
    return gs.offer > 2 * gs.pennies[2];
}

bool ratio_3_strat(gamestate_t gs, int player)
{
    return gs.offer > 3 * gs.pennies[2];
}

bool ratio_21_to_1_strat(gamestate_t gs, int player)
{
    int cardsleft = std::popcount(gs.cards[2]);
    float frac = (float)(cardsleft - 9.0) / (33.0 - 9.0);
    float pennyvalue = 1 + 20 * frac;
    return gs.offer > pennyvalue * gs.pennies[2];
}

#define named_strat(strat) std::make_tuple(#strat, strat)
auto strats = {
    // named_strat(always_no_thanks),
    named_strat(always_takes_card),
    // named_strat(ratio_2_strat),
    // named_strat(ratio_3_strat),
    // named_strat(ratio_21_to_1_strat)
    named_strat(high_cards_are_scary)};