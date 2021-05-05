# QuoridorMinimax

## Parallellized Move Evaluation for the Board Game _Quoridor_

### ECSE-4740 Parallel Computing for Engineers Term Project Code

A parallel minimax solver for the board game Quoridor using OpenMP

### Quoridor Board Game Rules

[Quoridor Rules](https://www.ultraboardgames.com/quoridor/game-rules.php)

### User Manual

#### Compilation

There are two different programs in the code, depending on how the user interacts with the minimax solver:

##### main.cpp (Single Move Mode)

This program loads a board from an ASCII text file called "input.txt" and plays and single move as the first player using the minimax solver AI, and then prints the output.  This was primarily used for code timing tests.
In order to compile it, one can use the following g++ command:

```
g++ main.cpp -fopenmp -O3 -o main.out
```

##### interactive.cpp (Interactive Play Mode)

This program starts from a Quoridor board in the position, and plays a round as the first player.  A human can then enter their move, and the computer will respond with its best move, looping on until the game is over.  This was primarily used for testing to make sure that the parallel minimax solver was working as expected for a whole game.
In order to compile it, one can use the following g++ command:

```
g++ interactive.cpp -fopenmp -O3 -o interactive.out
```

##### Boost

The code also uses a single boost header (for the hash functions used by the transposition table), so if your machine does not have the boost libraries installed, you can use a local copy of the boost source code instead when compiling.  The boost source is available at [https://www.boost.org/users/download/](https://www.boost.org/users/download/).  For example, below is the command used when compiling "interactive.cpp" when the boost source has been extracted to the same folder:

```
g++ interactive.cpp -fopenmp -O3 -I ./boost_1_76_0 -o interactive.out
```

#### Usage

##### Understanding the ASCII board

In the ASCII representation of the board, 'O's represent open tiles, the 'P' represents the primary player (the AI), and the 'S' represents the secondary player (the human).  Each wall is two tiles tall or wide, is represented by  character in between the rows and columns of open squares, and each blocks the path on both sides of it in whatever direction is points.  A wall placed by the first player is represented by a '!' when vertical and a '=' when horizontal.  Similarly, a wall placed by the second player is represented by a '|' when vertical and a '-' when horizontal.  Additionally, the program does not conserve what player placed which wall, but instead the total number of walls each player has placed since it is the only game-relevant feature, so there are multiple ASCII representations of the same board configuration.

Below, the text inside the example "input.txt" file in the repository is shown.

```text
-+-+-+-+-+-+-+-+-
O O O O O O O O O
           !
O O O O S O O O O

O O O O O O O O O
       !   !
O O O O O O O O O
         =
O O O O P O O O O
       =
O O O O O O O O O
         -
O O O O O O O O O

O O O O O O O O O

O O O O O O O O O
-+-+-+-+-+-+-+-+-
```

##### OpenMP Threads

The number of OpenMP threads used by the program can be changed by setting the environment variable 'OMP_NUM_THREADS' to the preferred number of threads.

##### Using Single Play Mode

Single play mode requires modifying the "input.txt" file to the state of the Quoridor board you would.  The program will print the number of move layers it looked through, and then the board after the AI plays its best move.  To run it, execute:

```
./main.out
```

An example run would give the output:
```
Depth Searched: 4
-+-+-+-+-+-+-+-+-
O O O O O O O O O
           !     
O O O O S O O O O

O O O O O O O O O
       !   !     
O O O O O O O O O
         =       
O O O O P O O O O
   =   =         
O O O O O O O O O
         -       
O O O O O O O O O

O O O O O O O O O

O O O O O O O O O
-+-+-+-+-+-+-+-+-
Total time was 14.847725 seconds.
```

##### Using Interactive Mode

Interactive play mode allows you to play a full game from start to finish against the AI.  To run it, execute:

```
./interactive.out
```

Human moves are given using three character codes.  Each code begins with either 'M' (for move), 'B' (for barrier / horizontal wall), or 'W' (for wall / vertical wall) and then the remaining two characters are the coordinates to either move to or place a wall at.  For such coordinates, the first number is the zero-indexed column number (0-8) and the second is the zero-indexed row number (0-8).  Note that since walls and barriers fall between the rows and columns, the are placed using the coordinate of the row and column to their immediate upper-left and thus each coordinate only ranges 0 to 7.
If the move is not allowed within the game rules, the interactive program will request a move again.

An example run showing the first two turns might look like:

```
Turn 1
-+-+-+-+-+-+-+-+-
O O O O S O O O O

O O O O O O O O O

O O O O O O O O O

O O O O O O O O O

O O O O O O O O O

O O O O O O O O O

O O O O O O O O O

O O O O O O O O O

O O O O P O O O O
-+-+-+-+-+-+-+-+-
Depth Searched: 4
Move AI 10.3s
-+-+-+-+-+-+-+-+-
O O O O S O O O O

O O O O O O O O O

O O O O O O O O O

O O O O O O O O O

O O O O O O O O O

O O O O O O O O O

O O O O O O O O O

O O O O P O O O O

O O O O O O O O O
-+-+-+-+-+-+-+-+-
Human Player Turn
Enter Move: M41
Turn 2
-+-+-+-+-+-+-+-+-
O O O O O O O O O

O O O O S O O O O

O O O O O O O O O

O O O O O O O O O

O O O O O O O O O

O O O O O O O O O

O O O O O O O O O

O O O O P O O O O

O O O O O O O O O
-+-+-+-+-+-+-+-+-
Depth Searched: 4
Move AI 17.4s
-+-+-+-+-+-+-+-+-
O O O O O O O O O

O O O O S O O O O

O O O O O O O O O

O O O O O O O O O

O O O O O O O O O

O O O O O O O O O

O O O O P O O O O

O O O O O O O O O

O O O O O O O O O
-+-+-+-+-+-+-+-+-
Human Player Turn
Enter Move: B35
Turn 3
-+-+-+-+-+-+-+-+-
O O O O O O O O O

O O O O S O O O O

O O O O O O O O O

O O O O O O O O O

O O O O O O O O O

O O O O O O O O O
       -         
O O O O P O O O O

O O O O O O O O O

O O O O O O O O O
-+-+-+-+-+-+-+-+-
```
