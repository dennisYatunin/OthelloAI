Documentation

First, I created a custom board that relies entirely on bitwise operations.
Since an Othello board has 64 squares, each of which can contain a white or a
black stone, it can be uniquely represented by two 64-bit integers, one of which
denotes the positions of the white stones and the other of which denotes the
positions of the black stones. These can be manipulated using bitshifts,
bitwise-and, and bitwise-or to generate moves, flip stones, count pieces, etc.
Using only bitwise operations allows for greatly reduced overhead, and, since
many board functions are called millions of times over, this can lead to a
noticeable increase in speed.

Then, I implemented a negascout seach algorithm, as described on Wikipedia.
Negascout uses alpha-beta pruning, but it also searches using empty intervals
(where beta = alpha + 1) before searching with full intervals. If the decision
of which moves to search first is made correctly, this can dramatically increase
the speed of the search.

To determine which moves to search first, I implemented a transposition table.
The entries of this table were hashed using Zobrist hashing, as described on
Wikipedia. Each entry recorded the three best moves for a given board, as well
as the depth to which those moves were searched in the decision tree. For a
minor improvement in performance, I also used this table to detect
transpositions (identical boards reached by different paths through the decision
tree) and to avoid re-computing them. In order to minimize hash collisions, the
size of the table is set to be as large as allowed by the operating system (the
program attempts to make it around 537 MB in size, then halves the size until it
successfully produces an array without recieving a bad_alloc error).

The heuristic calculation took the most time to perfect. I got a general idea of
what characteristics of the board to examine from the blog post at
https://kartikkukreja.wordpress.com/2013/03/30/heuristic-function-for-reversiothello/,
but actually calculating the various aspects of the heuristic required a lot of
trial and error. In the end, I choose 6 parameters: stone imbalance (how many
stones each player has), mobility (how many moves each player has), potential
mobility (how many spaces there are next to each player's pieces), corners (how
many corners each player has captured), potential corners (how many corners a
player can capture in the next move), and stability (how many of a player's
pieces cannot be flipped in the next move). The weights on these parameters
change as the game progresses. Stone imbalance, for example, starts off with the
lowest weight, but, by the last turn, it is the only parameter that is taken
into account. This results in a dynamic heuristic that proves to be effective.
