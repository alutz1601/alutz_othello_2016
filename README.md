Caltech CS2 Assignment 9: Othello

See [assignment9_part1.html](http://htmlpreview.github.io/?https://github.com/caltechcs2/othello/blob/master/assignment9_part1.html) and [assignment9_part2.html](http://htmlpreview.github.io/?https://github.com/caltechcs2/othello/blob/master/assignment9_part2.html)

ASSIGNENT 9 PART 2
-No partners

For the tournament, I developed an approach which combined a 5-ply minimax with a simple heuristic which gave special weights to the spaces near the edges and corners. Originally, I had simply performed a 2-ply minimax which was able to beat a random player most of the time. However, for the purpose of the tournament, I felt that a greater depth search was necessary to anticipate more complicated enemies. I settled on a 5-ply depth after testing the win rates against the ConstantTimePlayer at different depths; this setting was optimal because it could complete the tree computation within the given time constraint while still "thinking ahead" enough to avoid costly losses. I also weighted certain spaces as follows:

-corner spaces: 3 * base
-edge spaces: 2 * base
-spaces next to corners: -3 * base

The base is simply the value of a given move as determined by the minimax. I used the base multiplication system to "normalize" the bonuses and penalties of playing on or near an edge or corner, since a flat weight would have a different impact on the AI's decision making process depending on how many pieces were in play. Also, I never gave a weight to the spaces next to edges (but not next to corners), simply because I felt that doing so would limit the choices of play for my AI too much, which would have been just as damaging to my side as if I had lost an edge.

In the minimax, the weights are added at the end of the recursion. However, in the main doMove function, I also add half the weights to each hypothetical move. These moves are the ones that my AI will actually take this turn, so I wanted to add a small incentive to gravitate towards corners and edges (even if eventually there is a chance for a great loss later on). I didn't add the full weight because I wanted the minimax function to be the main way that my AI determined the optimal move.