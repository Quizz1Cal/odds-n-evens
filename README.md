# odds-n-evens

**Author**: Callum Holmes

**Creator of game (to my knowledge)**: Thomas in my MAST20009 tutorial

NOTE: Best viewed with tab 4, no wrap, or just spaces.

*odds_evens_one_file.c has the same functionality as the compilation of all other files into main using the makefile.*

This is the first online project that I submitted. As a rough description, this project is an attempted proof of the forceability of "odds & evens" via C.

### Rules of the game
- Given a 3x3 square grid (much like tic-tac-toe), players take turns writing numbers (either odd or even, respectively) into squares,  increasing as they go e.g. player 1 writes 1, player 2 2, then 1 3 etc.
- A number disappears 6 moves later e.g. when player 1 writes a 7, the 1 that they initially wrote vanishes AFTER the 7 is written down.
- Aim of game is for a player to get 3 in a row. No ties allowed. 

### Contents
- *main.c*: Hub for code execution
- *game_struct.c*: Implements node structs, boards, moves, generation and AI algorithm
- *Interface.c*: Allows for command-line friendly interaction.
- *Analytic.c*: Used to debug.

### Coding approach
The game utilises an adaptation of the minimax algorithm to find winning moves, looking at a node depth of about 9 moves at each decision state. At the time I had no knowledge of the minimax algorithm but still somehow discovered and used the approach when implementing this project, which is pretty cool!

Board states are implicitly stored in a tree of turns i.e. moves - to derive a board, the code backtracks 9 steps to obtain the 9 most recently played moves. 

### Implementation History
- Draft 1: Attempt to create nodes, each storing boards, moves, player tags. This was deemed to be highly inefficient with memory and time. 
- Draft 2: Attempt to create nodes with reduced memory demand by only storing moves made; boards are implicitly inferred.
- Draft 3: Attempted to and then forgave deletion of un-necessary pathways. Incorporated a simulator and an "AI" functionality so that upon execution, a user can play against a computer, or against a second player on same computer (or themselves). 
- Draft 4: Original single-file program was split for functionality into multiple files and accompanied with a makefile for ease of future adjustment.
