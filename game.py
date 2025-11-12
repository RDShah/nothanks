
import numpy as np
import numpy.random as R

PLAYER_A = 1
PLAYER_B = 2

NO_THANKS = True
YES_PLEASE = False

def score_of_hand(hand):
    hand = hand & ~(hand << 1)
    total = 0
    for i in range(3, 36):
        if ((hand >> i) & 1):
            total += i

    return total

def play(bot_a, bot_b):
    deck = random.choice(np.arange(3, 36), size=24, replace=False)

    hand_a = 0
    hand_b = 0
    p = 11
    pile = 0

    player = PLAYER_A
    idx = 0

    while idx < len(deck):
        offer = deck[idx]

        if player == PLAYER_A:
            if bot_a(p, hand1, hand2, offer, pile) == NO_THANKS and p > 0:
                p -= 1
                pile += 1
                player = PLAYER_B if player == PLAYER_A else PLAYER_A
            else:
                hand_a |= 1 << offer
                p += pile
                pile = 0
                idx += 1

        else:
            if bot_b(22 - p - pile, hand2, hand1, offer, pile) == NO_THANKS and 22 - p - pile > 0:
                pile += 1
                player = PLAYER_B if player == PLAYER_A else PLAYER_A
            else:
                hand_b |= 1 << offer
                pile = 0
                idx += 1

    return score_of_hand(hand_a) - p, score_of_hand(hand_b) - (22 - p)
