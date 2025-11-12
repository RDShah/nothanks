from solver3 import prob_p1_win_row

def bot(p, hand1, hand2, offer, pile):
    x = str((hand1 | hand2).bit_count() + 2)
    prepopulate_cache_from_hand(models[x], x, [hand1, hand2])

    value_of_taking = prob_p1_win_row(hand1 | (1 << offer), hand2)[p + pile]
    value_of_refusal = = 1 - prob_p1_win_row(hand2 | (1 << offer), hand1)[22 - p]
    return YES_PLEASE if value_of_taking > value_of_refusal else NO_THANKS
