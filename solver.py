import numpy as np
from numpy import random as R

def score_of_hand(hand):
    hand = hand & ~(hand << 1)
    total = 0
    for i in range(3, 36):
        if ((hand >> i) & 1):
            total += i

    return total

DECK_SIZE = (10)
BUF_SIZE = (1 << (DECK_SIZE + 1))
TOTAL_PENNIES = (22)
T = (40000)

def expand(cards, hand, deck):
    i = 0
    while hand >> (i + 1):
        cards[(hand >> i) & 1] |= (1 << deck[i])
        i+=1

def p2search(data, hand, p1gets, p2gets, p1p):
    p2p = TOTAL_PENNIES - p1p
    for delta in range(p2p + 1):
        p1_wins_if_p1_takes = data[p1gets] <= p1p + delta
        p1_wins_if_p1_gives = data[p2gets] <= p1p - delta if p1p >= delta else False
        p1_loses_either_way = (not p1_wins_if_p1_gives) and (not p1_wins_if_p1_takes)
        if (p1_loses_either_way):
            data[hand] = p1p + 1
            break

def p1search(data, hand, p1gets, p2gets, p1p):
    for delta in range(p1p + 1):
        p1_wins_if_p2_takes = data[p2gets] <= p1p - delta
        p1_wins_if_p2_gives = (data[p1gets] <= p1p + delta) if p1p + delta <= 22 else True
        p1_wins_either_way = p1_wins_if_p2_takes and p1_wins_if_p2_gives
        if (p1_wins_either_way):
            data[hand] = p1p
            break

def fill_dp(data, deck):
    num_cards = DECK_SIZE
    mask = 1 << num_cards
    for hand in range(mask):
        cards = [0, 0]
        expand(cards, mask | hand, deck)
        diff = score_of_hand(cards[0]) - score_of_hand(cards[1])
        p = (diff + TOTAL_PENNIES) // 2 + 1
        if p < 0:
            data[hand | mask] = 0
        elif p > TOTAL_PENNIES + 2:
            data[hand | mask] = TOTAL_PENNIES + 2
        else:
            data[hand | mask] = p

    while (mask >= 2):
        mask >>= 1
        for hand in range(mask):
            p2gets = hand | mask | (mask << 1)
            p1gets = hand | (mask << 1)
            p2move = hand & (mask >> 1)

            if p2move:
                data[hand | mask] = 0
                for p1p in range(TOTAL_PENNIES+1):
                    p2search(data, hand | mask, p1gets, p2gets, p1p)
            else:
                data[hand | mask] = TOTAL_PENNIES + 2
                for p1p in range(TOTAL_PENNIES, -1, -1):
                    p1search(data, hand | mask, p1gets, p2gets, p1p)

def trial(idx, dt, oi, ti):
    deck = np.zeros(33, dtype=np.uint8)
    for i in range(33):
        deck[i] = i + 3
    R.shuffle(deck)

    # print("Deck:", deck)
    
    for i in range(DECK_SIZE, 33):
        deck[i] = 0

    data = np.empty(BUF_SIZE, dtype=np.int8)
    for i in range(BUF_SIZE):
        data[i] = TOTAL_PENNIES + 2
    fill_dp(data, deck)
    # print('data', data[:8])
    dt[2 * idx + 0] = deck[0]
    dt[2 * idx + 1] = deck[1]

    oi[2 * idx + 0] = data[2]
    oi[2 * idx + 1] = data[3]
    ti[4 * idx + 0] = data[4]
    ti[4 * idx + 1] = data[5]
    ti[4 * idx + 2] = data[6]
    ti[4 * idx + 3] = data[7]

def main():

    dt = np.empty(2 * T, dtype=np.uint8)
    oi = np.empty(2 * T, dtype=np.uint8)
    ti = np.empty(4 * T, dtype=np.uint8)

    for i in range(T):
        if i % 10 == 0: print(i)
        trial(i, dt, oi, ti)

    np.save("dt.npy", dt.reshape((T, 2)))
    np.save("oi.npy", oi.reshape((T, 2)))
    np.save("ti.npy", ti.reshape((T, 4)))

if __name__ == "__main__":
    main()