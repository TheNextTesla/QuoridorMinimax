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
#define SAVE_OUTPUT false

int main(int argc, char *argv[])
{
    std::vector<Board> boards;
    Board start_board = game_start_board();
    bool starting_player = (bool) 1;

    unsigned int turn = 1;
    while (!start_board.game_over())
    {
        std::cout << "Turn " << std::to_string(turn) << std::endl;
        std::cout << output_to_ascii(start_board);
        boards.push_back(start_board);

        struct timeval start_time, stop_time, elapsed_time;
        gettimeofday(&start_time, NULL);

        Move best_next_move = minimax_parallel_helper(start_board, DEPTH, starting_player);

        gettimeofday(&stop_time,NULL);
        timersub(&stop_time, &start_time, &elapsed_time);
        printf("Move AI %.1fs\n", elapsed_time.tv_sec+elapsed_time.tv_usec/1000000.0);

        start_board.apply_move(best_next_move);
        std::cout << output_to_ascii(start_board);

        if (start_board.game_over())
        {
            std::cout << "Computer Wins!" << std::endl;
            break;
        }
        else
        {
            boards.push_back(start_board);
            std::cout << "Human Player Turn" << std::endl;
        }

        std::vector<Move> response_moves = generate_valid_moves(start_board, !starting_player);
        std::string input;
        Move user_move;
        bool valid_move;
        do
        {
            std::cout << "Enter Move: ";
            std::cin >> input;
            valid_move = generate_from_string(input, !starting_player, user_move);

            if (!valid_move)
                continue;

            bool possible_move = false;
            for (Move move : response_moves)
            {
                if (move == user_move)
                {
                    possible_move = true;
                    break;
                }
            }
            valid_move &= possible_move;
        }
        while (!valid_move);
        start_board.apply_move(user_move);
        turn++;
    }
    boards.push_back(start_board);

    if (!SAVE_OUTPUT)
        return 0;

    unsigned int count = 0;
    for (Board saved_board : boards)
    {
        std::string file_name = "output/board" + std::to_string(count) + ".txt";
        std::ofstream outfile(file_name);
        outfile << output_to_ascii(saved_board);
        outfile.close();
        count++;
    }
    return 0;
}
