#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <streambuf>

#include <chrono>
#include <thread>

#include "game.h"

int main(int argc, char *argv[])
{
    std::ifstream t("input_2.txt");
    std::string str((std::istreambuf_iterator<char>(t)),
                     std::istreambuf_iterator<char>());

    Board board = generate_from_text(str);

    std::vector<Move> possible_wall_moves = find_open_wall_positions(board, true);
    int dist = goal_bfs_distance(board, true);
    std::cout << std::to_string(dist) << std::endl;

    std::cout << std::to_string(calculate_heurisitic(board, false)) << std::endl;

    // std::vector<Move> moves = generate_valid_moves(board, true);
    // std::cout << std::to_string(moves.size()) << std::endl;
    // for (Move move : moves)
    // {
    //     Board temp_board = board;
    //     temp_board.apply_move(move);
    //     std::cout << output_to_ascii(temp_board);
    //     std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // }

    // board.apply_move(moves[0]);
    // std::cout << std::to_string(moves.size()) << std::endl;
    // std::cout << output_to_ascii(board);
    return 0;
}
