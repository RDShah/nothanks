#include "nothanks.h"

bool always_takes_card(gamestate_t gs, int player, int card) { return false; }
bool always_no_thanks(gamestate_t gs, int player, int card) { return true; }

bool ratio_2_strat(gamestate_t gs, int player, int card)
{
    return card > 2 * gs.pennies[2];
}

bool ratio_3_strat(gamestate_t gs, int player, int card)
{
    return card > 3 * gs.pennies[2];
}
