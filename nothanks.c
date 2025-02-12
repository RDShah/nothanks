#include "nothanks.h"

bool gamestate_t::p1_has_card(int j)
{
    return (this->cards[0] >> j) & 1;
}
bool gamestate_t::p2_has_card(int j)
{
    return (this->cards[1] >> j) & 1;
}

bool gamestate_t::no_one_has_card(int j)
{
    return (this->cards[2] >> j) & 1;
}

void gamestate_t::print_state() {
    printf("P1 ");
    printf("(pennies: %d) cards: ", this->pennies[0]);
    for (auto i = 3; i <= 35; i++)
    {
        if (this->p1_has_card(i))
            printf("%d, ", i);
    }
    printf("\n");
    printf("P2 hand: ");
    printf("(pennies: %d) cards: ", this->pennies[1]);
    for (auto i = 3; i <= 35; i++)
    {
        if (this->p2_has_card(i))
            printf("%d, ", i);
    }
    printf("\n");
    printf("Offer (pennies: %d) card: %d\n", this->pennies[2], this->offer);
}