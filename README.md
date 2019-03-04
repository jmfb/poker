# Poker

This project is dedicated to odds computation, gameplay simulation, and eventually opponent AI
for No-Limit Texas Hold-em Poker.

## Pre-flop Odds

Starting hand selection is an important aspect of Texas Hold-em Poker.
Many starting hand charts are readily available online.
However, to make the best decisions possible I have create a project to compute the exact odds of winning for each starting hand.

### Starting Hand Simplification

There are 1,326 possible starting hands in poker.
```
C(52, 2) = 52 * 51 / 2 = 1325
```

However, this can be simplified down to pairs, suited cards, and unsuited cards since the suits combinations have identical odds.
```
       Count  Suit Combinations  Total
Pairs:    13                  6     78
Suited:   78                  4    312
Offsuit:  78                 12    936
Total:   169                      1326
```

### Community Card Combinations

There are 2,118,760 possible community card combinations for any given starting hand (50 cards remain in the deck, choose 5).
```
C(50, 5) = (50 * 49 * 48 * 47 * 46) / (5 * 4 * 3 * 2) = 2118760
```

The total odds for a given starting hand equals the sum of odds for each community card combination.


*Optimization:* The odds for community card suit combinations independent from the starting hand are equal and allows for between 1 to 5 free community computations.
```
Distinct Suited Starting Hand Community Card Combinations:    388,863 (1,729,897 free suit combinations)
Distinct Offsuit Starting Hand Community Card Combinations: 1,094,724 (1,024,036 free suit combinations)
NOTE: Pairs have the same combinations as offsuit starting hands.
```

### Hand Ranking

There are 2,598,960 possible hands in poker (52 choose 5).
```
C(52, 5) = (52 * 51 * 50 * 49 * 48) / (5 * 4 * 3 * 2) = 2598960
```

Each hand has a specific rank that can be used to compare against other hands, from the lowest high card hand (23457) to a royal flush.
The following shows the distribution of hands for each category of hand ranking:
```
                  Count  Type Faces (0 to 5)
High Card:      1302540     0 5={Highest Card,...,Lowest Card}
One Pair:       1098240     1 4={Pair,Highest Kicker,...,Lowest Kicker}
Two Pair:        123552     2 3={Top Pair,Bottom Pair,Kicker}
Three of a Kind:  54912     3 3={Three of a Kind,Highest Kicker,Lowest Kicker}
Straight:         10200     4 1={Top of the straight}
Flush:             5108     5 5={Highest Card,...,Lowest Card}
Full House:        3744     6 2={Three of a Kind,Pair}
Four of a Kind:     624     7 2={Four of a Kind,Kicker}
Straight Flush:      36     8 1={Top of the straight}
Royal Flush:          4     9 0={}
```

Hand rank can converted to a 32-bit unsigned int using the following formula:
```
Rank(hand) = Type * 13^5 + Faces[0] * 13^4 + ... + Faces[4]
```

5-card hands with cards represented as integers from 0-51 and ordered from lowest to highest can be converted to a 32-bit unsigned int using the following formula:
```
Hash(hand) = Card[0] * 52^4 + ... + Card[4]
Max(Hash(hand)) = 47 * 52^4 + ... + 51 = 350530283
```

We can then compute the integer rank for all 2,598,960 possible hands and store them in a vector of size equal to max hand hash plus one (350,530,284)
to give a constant time comparison for any given hands.  This vector is saved to disk in a 1.3GB file, handRanksByHash.bin, to save time computing and
comparing hands.  See the `AllHands` class for details.

### Best Hand Rank

For any given set of two hole cards and five community cards there exists a best hand rank.
The best hand rank is the max hand rank for all possible five card combinations:
```
C(2, 0) * C(5, 5) = 1 * 1 = 1   // Use no hole cards and the entire community.
C(2, 1) * C(5, 4) = 2 * 5 = 10  // Use either hole card and any four community cards.
C(2, 2) * C(5, 3) = 1 * 10 = 10 // Use both hole cards and any three community cards.
Total: 21 possible 5 card combinations.
```

### Win, Draw, Lose Calculations

The total number of opponent card combinations, for `k`-opponents, for a given starting hand and community is:
```
C(45, 2k) * Product(n=0..k-1, 2n + 1)
```
The first part represents the way to deal `x` cards to the opponents where each opponent gets 2 cards: `x = 2k`.
The second part represents the ways to partition `2k` cards among `k` opponents.

The following table shows the resulting problem space for 1 to 8 opponents:
```
k        C(45, 2k) Partitions                      Total
1              990          1                        990
2          148,995          3                    446,985
3        8,145,060         15                122,175,900 (122 million)
4      215,553,195        105             22,633,085,475 ( 22 billion)
5    3,190,187,286        945          3,014,726,985,270 (  3 trillion)
6   28,760,021,745     10,395        298,960,426,039,275 (298 trillion)
7  166,871,334,960    135,135     22,550,157,849,819,600 ( 22 quadrillion)
8  646,626,422,970  2,027,025  1,310,727,925,020,764,250 (  1 quintillion)
```

Brute forcing the problem space is not feasible beyond a single opponent as even two opponents would take 451 times longer.
The 8 opponent problem space is huge:
```
Brute Force Problem Space = Starting Hands * Distinct Community Combinations * Opponent Combinations
Suited:  78 *   388,863 * 1,310,727,925,020,764,250 =  39,756,100,262,373,256,986,724,500 ( 39 septillion)
Offsuit: 91 * 1,094,724 * 1,310,727,925,020,764,250 = 130,574,563,846,129,232,176,347,000 (130 septillion)
```

However, we can reduce the problem space significantly.

### Removing 1-Card Losses

For a given starting hand and community combination you have a given best hand rank `v`.
You can now test the remaining 45 cards in the deck to see if just adding the one card to the community beats your hand.
You need to test each of the 45 cards against up to `C(5, 4) = 5` combinations of the community cards
since you only need to test if any combination beats you and not what the best hand rank would be.
You can remove the 1-card losses from the remaining deck since dealing that card to any opponent would result in a loss.
We will call the remaining deck size `r` where `r = 45 - count(1-card losses)` and remaining deck `R`
where `R = Full Deck - Starting Hand - Community Cards - 1-Card Losses`.
The maximum 1-card loss count is 44 for the starting hand pair of 2s with the community of four 3s and a 2 since every other card
in the deck other than the remaining 2 is a four of a kind 3s with a better kicker.

### Finding 2-Card Losses

The only way you might still lose would be if an opponent had exactly two cards that, when combined with the community cards, beat you.
You need to test the remaining `C(r, 2)` possible 2-card combinations of `R` against up to the `C(5, 3) = 10` community card combinations.
This will give you the set of 2-card losses, `S = {s_0,...,s_(n-1)}` of size `n`.
Each 2-card loss, `s_i`, can be represented as the set `{c1, c2}` where `c1` and `c2` are the integer representations, in ascending order, of the cards in the deck.
The maximum 2-card loss count is 268 for the starting hand of 4s 8s and a community of 2s 3s 6h 7h 8h.

### Win or Draw Calculation

TODO: left off here

### 2-Card Overlap Time Complexity Problem

https://math.stackexchange.com/questions/3107016/count-of-disjoint-combinations-of-sets-of-sets
