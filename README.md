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


TODO: Put in statistics for pairs, suited, and offsuit counts.

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

### Win, Draw, Lose Calculations

TODO: left off here

### 2-Card Overlap Time Complexity Problem

https://math.stackexchange.com/questions/3107016/count-of-disjoint-combinations-of-sets-of-sets
