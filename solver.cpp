#include <cstdint>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <random>
#include <complex>
#include <map>
#include <string>
#include <fstream>
using namespace std;

#define DECK_SIZE (10)
#define BUF_SIZE (1 << (DECK_SIZE + 1))
#define TOTAL_PENNIES (22)
#define T (100000)

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint8_t u8;
typedef int8_t i8;
typedef std::tuple<u8, u8, u8> u8triplet;

/**
 * deck is a array of ints, deck[i] is the value of the card in position i
 *
 * There are two ways to represent a hand
 * pair of 64 bit ints representation:
 * (cards[p] >> i) & 1 [[player p has the card with value i]]
 * note this representation is deck agnostic
 * single 32 bit int representation (wrt to a fixed deck):
 * the position of the most signifigant bit denotes how many cards have been distributed
 * so eg 000...0001xxxx means FOUR cards have been distributed
 * when i < the number of cards that have been distributed,
 * (hand >> i) & 1 == [0 if p1 has that card, 1 if p2 has that card]
 *
 * confusingly my variable names call the players "p1" and "p2"
 * but the code represents p1 as 0 and p2 as 1
 */

int score_of_hand(u64 hand)
{
    hand = hand & ~(hand << 1);
    int total = 0;
    for (int i = 3; i <= 35; i++)
        if ((hand >> i) & 1)
            total += i;

    return total;
}

void expand(u64 (&cards)[2], u32 hand, uint8_t *deck)
{
    // while i is not the highest set bit (since that marks the end)
    // give the card with value deck[i] to player (hand >> i) & 1
    for (int i = 0; hand >> (i + 1); i++)
        cards[(hand >> i) & 1] |= (1ul << deck[i]);
}

void p2search(i8 data[BUF_SIZE], u32 hand, u32 p1gets, u32 p2gets, int p1p)
{
    int p2p = TOTAL_PENNIES - p1p;
    for (int delta = 0; delta <= p2p; delta++)
    {
        bool p1_wins_if_p1_takes = data[p1gets] <= p1p + delta;
        bool p1_wins_if_p1_gives = p1p >= delta ? (data[p2gets] <= p1p - delta) : false;
        bool p1_loses_either_way = !p1_wins_if_p1_gives && !p1_wins_if_p1_takes;
        if (p1_loses_either_way)
        {
            data[hand] = p1p + 1;
            break;
        }
    }
}

void p1search(i8 data[BUF_SIZE], u32 hand, u32 p1gets, u32 p2gets, int p1p)
{
    for (int delta = 0; delta <= p1p; delta++)
    {
        bool p1_wins_if_p2_takes = data[p2gets] <= p1p - delta;
        bool p1_wins_if_p2_gives = p1p + delta <= 22 ? (data[p1gets] <= p1p + delta) : true;

        bool p1_wins_either_way = p1_wins_if_p2_takes && p1_wins_if_p2_gives;
        if (p1_wins_either_way)
        {
            data[hand] = p1p;
            break;
        }
    }
}

void fill_dp(i8 data[BUF_SIZE], uint8_t *deck)
{
    int num_cards = DECK_SIZE;
    u32 mask = 1 << num_cards;
    for (u32 hand = 0; hand < mask; hand++)
    {
        u64 cards[2] = {0ul, 0ul};
        expand(cards, mask | hand, deck);
        auto diff = score_of_hand(cards[0]) - score_of_hand(cards[1]);
        int p = (diff + TOTAL_PENNIES) / 2 + 1;
        data[hand | mask] = p <= 0 ? 0 : p > TOTAL_PENNIES + 2 ? TOTAL_PENNIES + 2
                                                               : p;
    }

    while (mask >= 2)
    {
        mask >>= 1;
        for (u32 hand = 0; hand < mask; hand++)
        {
            auto p2gets = hand | mask | (mask << 1);
            auto p1gets = hand | (mask << 1);
            auto p2move = hand & (mask >> 1);

            if (p2move)
            {
                data[hand | mask] = 0;
                for (int p1p = 0; p1p <= TOTAL_PENNIES; p1p++)
                    p2search(data, hand | mask, p1gets, p2gets, p1p);
            }
            else
            {
                data[hand | mask] = TOTAL_PENNIES + 2;
                for (int p1p = TOTAL_PENNIES; p1p >= 0; p1p--)
                    p1search(data, hand | mask, p1gets, p2gets, p1p);
            }
        }
    }
}

void trial(int idx, u8 dt[2 * T], u8 oi[2 * T], u8 ti[4*T], std::mt19937 g)
{
    // std::random_device rd;
    // std::mt19937 g(rd());
    // std::mt19937 g(0);

    u8 deck[33];
    for (auto i = 0; i < 33; i++)
        deck[i] = i + 3;
    std::ranges::shuffle(deck, g);

    // printf("Deck: ");
    // for (auto i = 0; i < DECK_SIZE; i++)
    //     printf("%d ", deck[i]);
    // printf("\n");

    for (auto i = DECK_SIZE; i < 33; i++)
        deck[i] = 0;

    i8 *data = new i8[BUF_SIZE];

    for (auto i = 0; i < BUF_SIZE; i++)
        data[i] = TOTAL_PENNIES + 2;
    fill_dp(data, deck);
    // for (auto i = 1; i < 8; i++)
    //     printf("%d ", data[i]);
    // printf("\n");
    dt[2 * idx + 0] = deck[0];
    dt[2 * idx + 1] = deck[1];

    oi[2 * idx + 0] = data[2];
    oi[2 * idx + 1] = data[3];
    ti[4 * idx + 0] = data[4];
    ti[4 * idx + 1] = data[5];
    ti[4 * idx + 2] = data[6];
    ti[4 * idx + 3] = data[7];
}

int main()
{
    // std::random_device rd;
    u8 *dt = new u8[2 * T];
    u8 *oi = new u8[2 * T];
    u8 *ti = new u8[4 * T];

    for (auto i = 0; i < T; i++)
    {
        if(i % 1000 == 0) printf("%d\n", i);
        std::mt19937 g(i);
        trial(i, dt, oi, ti, g);
    }

    std::ofstream("data/dt.bin", std::ios::binary).write((char*)dt, 2 * T);
    std::ofstream("data/oi.bin", std::ios::binary).write((char *)oi, 2 * T);
    std::ofstream("data/ti.bin", std::ios::binary).write((char *)ti, 4 * T);
}