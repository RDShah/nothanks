import functools
import numpy as np
import random
import time
from tqdm import tqdm

def score_of_hand(hand):
    hand = hand & ~(hand << 1)
    total = 0
    for i in range(3, 36):
        if ((hand >> i) & 1):
            total += i

    return total

def generate_hand(num_cards, streakiness=.5):
    hand = [0, 0]
    *deck, = range(3, 36)
    random.shuffle(deck)
    deck = deck[:num_cards]
    deck.sort()
    p = random.random() < .5
    for i in deck:
        hand[p] |= (1 << i)
        p = p ^ (random.random() > streakiness)

    return hand

def _p1_win_row(hand1, hand2):
    out = np.zeros(24, dtype=np.float32)
    s1 = score_of_hand(hand1)
    s2 = score_of_hand(hand2)
    p = np.floor((s1 - s2) / 2) + 12
    out[max(0, int(p)):] = 1
    out[23] = 1
    return out


@functools.cache
def prob_p1_win_row(hand1, hand2):
    union = hand1 | hand2
    popcount = union.bit_count()
    if popcount == 24:
        return _p1_win_row(hand1, hand2)

    sum_prob = np.zeros(24, dtype=np.float32)
    for offer in range(3, 36):
        if ((union >> offer) & 1) == 0:
            sum_prob += prob_p1_win_row_offer(hand1, hand2, offer)

    return sum_prob / (33 - popcount)

def prob_p1_win_row_offer(hand1, hand2, offer):
    p2_refuses = prob_p1_win_row(hand1 | (1 << offer), hand2)
    p2_takes = 1 - prob_p1_win_row(hand2 | (1 << offer), hand1)

    out = np.empty(24, dtype=np.float32)
    out[23] = 1
    for p in range(23):
        off1 = p2_refuses[p + 1 : 2 * (min(11, p) + 1)]
        off2 = p2_takes[22 - p + 1 : 2 * (min(11, 22 - p) + 1)]
        out[p] = max( np.minimum(off1, off2) )

    return np.maximum(out, p2_refuses)


if __name__ == "__main__":
    N = 1000
    num_cards = 21
    dataset_number = 0
    while 1:
        dataset_number += 1
        print(f"Dataset #{dataset_number}")
        x = np.empty((N, 2), dtype=np.uint64)
        y = np.empty((N, 23), dtype=np.float32)
        random.seed(dataset_number)
        for i in tqdm(range(N)):
            hand = generate_hand(num_cards)
            x[i] = hand
            y[i] = prob_p1_win_row(*hand)[:23]
            prob_p1_win_row.cache_clear()

        np.save(f'data/x_data_d{num_cards}_{dataset_number}.npy', x)
        np.save(f'data/y_data_d{num_cards}_{dataset_number}.npy', y)

    # N = 10
    # hands = [generate_hand(21) for _ in range(N)]
    # a = time.time()
    # for hand in hands:
    #     prob_p1_win_row(*hand)
    # b = time.time()
    # print( (b-a) / N )

    # 22 cards : 0.01s per
    # 21 cards : 0.16s per
    # 20 cards : 1.6 s per
