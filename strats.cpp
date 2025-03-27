#include <iostream>
using namespace std;

#include "nothanks.h"

bool human(gamestate_t gs, int player)
{
    std::string x;
    gs.print();
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

bool ratio_x_strat(float x, gamestate_t gs, int player)
{
    return gs.offer > x * gs.pennies[IN_CENTER] ? NO_THANKS : YES_PLEASE;
}

bool ratio_5_strat(gamestate_t gs, int p)
{
    return ratio_x_strat(5, gs, p);
}

bool ratio_lerp_x_strat(float hi, gamestate_t gs, int player)
{
    float frac = (gs.num_cards_left() - 9.0) / (33.0 - 9.0);
    return ratio_x_strat(1 + (hi - 1) * frac, gs, player);
}

bool ratio_lerp_11_strat(gamestate_t gs, int player)
{
    return ratio_lerp_x_strat(11, gs, player);
}

bool hand_crafted(gamestate_t gs, int p)
{
    int card = gs.offer;
    if (!gs.has(IN_CENTER, card + 1) && !gs.has(IN_CENTER, card - 1))
    {
        return YES_PLEASE;
    }

    // if (gs.has(p, card + 1))
    // {
    //     return ratio_x_strat(10, gs, p);
    // }

    // if (gs.has(p, card - 1))
    // {
    //     return ratio_x_strat(8, gs, p);
    // }

    return ratio_lerp_x_strat(11, gs, p);
}

#define named_strat(strat) std::make_tuple(#strat, strat)
auto strats = {
    // named_strat(always_no_thanks),
    // named_strat(always_takes_card),
    // named_strat(ratio_5_strat),
    // named_strat(ratio_21_to_1_strat),
    named_strat(ratio_lerp_11_strat),
    // named_strat(high_cards_are_scary),
    named_strat(hand_crafted),
    // named_strat(hand_crafted2),
};