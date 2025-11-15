# Kuhn Poker Notes

## The first step to building a full fledged poker solver. 

Kuhn Poker was initially made as a simplified version of Poker so rudimentary game theory analysis and techniques could be applied. 

There are only two players. Each of the two players is dealt one of three cards: a King, a Queen, or a Jack. 

The rules are as follows
- Both players ante 1 chip
- Player 1 will either check or bet 1 chip
- If player 1 checked, player 2 can also check and it goes to showdown. Otherwise, player 2 may bet 1 chip, and player 1 either folds, or calls and it goes to showdown. 
- If player 2 bet, player 2 can fold, or they may call and there is a showdown. 

So how can we go about solving this?

## Counterfactual Regret Minimization

We can think of the entire universe of all possible games as a tree composed of hands and actions. 

We have 6 possible hand combos across both players. 
First letter: player 1 hand
Second letter: player 2 hand
KQ, KJ, QK, QJ, JK, JQ

Likewise, there are three possible actions, (b)et, (c)heck, and (f)old. 

We can build out our tree in levels, where we have the initial deal node, and from it it branches out to 6 states (representing 6 possible hand combos). 

We'll have the state represent what the current player knows (in Kuhn player this is virtually only the action), and at each level of the tree we will flip states to the other player's POV. The way Kuhn poker is structured, each state only leads to two lower states (either b/p or c/f).

This is helpful because Kuhn Poker is an imperfect information game. If you're player 2 and you were dealt Q, the only info you may know is that player 1 bet, but you have no clue if he has a K or a J. 

Thus, it makes sense to logically group the bets you are facing in both the KQ and JQ part of the tree together.

What we can do is walk down the game tree calculating our beliefs at each stage, about what the other player may have and what we should do at each stage. 

We really only know the results at the very bottom of the tree. So once we know the payoffs, we go up the tree and recalculate what our EV is at every stage. 

The probability of being in each part of the tree is calculated along the edges of the tree (as expected). Once we reach the child node and see the payoff, we can go backwards and recalculate the EV of the parent. 

The forward pass computes the state probabilities, the backward pass computes the state values. 

In the videos and papers I read, I was kind of confused by how beliefs were defined. Formally, the belief at some information set I is a probability distribution over the nodes inside that information set. Because we the player can't distinguish between those nodes (e.g. we as player 2 holding a Q seeing a bet don't know if player 1 is betting for value with a K or bluffing with a J), we assign probabilities to whether we're getting value bet or bluffed. 

