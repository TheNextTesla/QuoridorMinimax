#ifndef game_h
#define game_h

#include <vector>

#include "board.h"
#include "path.h"

struct Transposition
{
    unsigned char depth;
    char flag;
    float value;
};

inline std::vector<Move> generate_valid_moves(Board board, bool player1)
{
    // Player Movement Valid Moves
    std::vector<Move> valid_moves = valid_player_moves(board, player1);


    // Player Valid Wall Placements
    if ((player1 && board.p1_walls_left > 0) || (!player1 && board.p2_walls_left > 0))
    {
        std::vector<Move> possible_wall_moves = find_open_wall_positions(board, player1);
        for (Move move : possible_wall_moves)
        {
            Board test_board = board;
            test_board.apply_move(move);
            if (goal_bfs_distance(test_board, !player1) >= 0 &&
                goal_bfs_distance(test_board, player1) >= 0)
            {
                valid_moves.push_back(move);
            }
        }
    }

    return valid_moves;
}

inline float calculate_heurisitic(Board board, bool player1)
{
    float hueristic;

    unsigned char p1_x, p1_y, p2_x, p2_y;
    board.get_player_positions(p1_x, p1_y, true);
    board.get_player_positions(p2_x, p2_y, false);


    if (p1_y == 0)
        hueristic = 100;
    else if (p2_y == 8)
        hueristic = -100;
    else
    {
        float p1_goal_dist = goal_bfs_distance(board, player1);
        float p2_goal_dist = goal_bfs_distance(board, !player1);
        float delta = p2_goal_dist - p1_goal_dist;

        float delta_y = (8-((float) p2_y)) - ((float) p1_y);

        float delta_w = ((float) board.p2_walls_left) - ((float) board.p1_walls_left);

        hueristic = 10*delta + 5*delta_y + delta_w;
    }
    return hueristic;
}

#endif
