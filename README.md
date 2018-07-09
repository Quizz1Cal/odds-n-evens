# odds-n-evens
First online project that I submitted. As a rough description, this project is an attempted proof of the forceability of "odds & evens" via C.
Rules of the game:
- Given a 3x3 square grid (much like tic-tac-toe), players take turns writing numbers (either odd or even, respectively) into squares,  increasing as they go e.g. player 1 writes 1, player 2 2, then 1 3 etc.
- A number disappears 6 moves later e.g. when player 1 writes a 7, the 1 that they initially wrote vanishes AFTER the 7 is written down.
- Aim of game is for a player to get 3 in a row. No ties allowed. 

IMPLEMENTATION HISTORY
Draft 1: Attempt to create nodes, each storing boards, moves, player tags.
This was deemed to be highly inefficient with memory and time. 
Draft 2: Attempt to create nodes with reduced memory demand by only storing
moves made; boards are implicitly inferred.

Problems:
- Reducing the amount actually generated (memory efficiency)
- How to implement dynamic array, whilst in a struct
- Determining overly recurrent cases
