#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <sys/time.h>

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <streambuf>

#include "game.h"
#include "optimization_omp.hpp"

#define DEPTH 4

int main(int argc, char *argv[])
{
    struct timeval start_time, stop_time, elapsed_time;
    gettimeofday(&start_time, NULL);

    // Board start_board = game_start_board();
    std::ifstream t("input.txt");
    std::string str((std::istreambuf_iterator<char>(t)),
                     std::istreambuf_iterator<char>());
    Board start_board = generate_from_text(str);
    std::cout << output_to_ascii(start_board);
    bool starting_player = (bool) 1;

    Move best_next_move = minimax_parallel_helper(start_board, DEPTH, starting_player);
    start_board.apply_move(best_next_move);
    std::cout << output_to_ascii(start_board);

    gettimeofday(&stop_time,NULL);
    timersub(&stop_time, &start_time, &elapsed_time);
    printf("Total time was %f seconds.\n", elapsed_time.tv_sec+elapsed_time.tv_usec/1000000.0);

}
