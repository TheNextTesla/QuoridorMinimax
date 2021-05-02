#ifndef path_h
#define path_h

#include <queue>
#include <map>
#include <utility>

#include <iostream>
#include <string>

#include "board.h"
#include "game.h"

typedef std::pair<int, int> Point;

inline std::vector<Move> find_open_wall_positions(Board board, bool player1)
{
    std::vector<Move> possible_moves;

    for (unsigned int i=0; i < 8; i++)
    {
        for (unsigned int j=0; j < 8; j++)
        {
            if (board.walls[i][j])
                continue;

            // Up-Down Check
            bool up_check = (j > 0) && board.walls[i][j-1] && board.wall_directions[i][j-1];
            bool down_check = (j < 7) && board.walls[i][j+1] && board.wall_directions[i][j+1];

            if (!(up_check || down_check))
            {
                possible_moves.push_back(Move(i, j, true, true, player1));
            }

            // Left-Right Check
            bool left_check = (i > 0) && board.walls[i-1][j] && !board.wall_directions[i-1][j];
            bool right_check = (i < 7) && board.walls[i+1][j] && !board.wall_directions[i+1][j];

            if (!(left_check || right_check))
            {
                possible_moves.push_back(Move(i, j, true, false, player1));
            }
        }
    }
    return possible_moves;
}

inline void valid_adjacent_tiles(Board board, bool player1,
                                 bool& up, bool& down, bool& left, bool& right)
{
    unsigned char x, y;
    board.get_player_positions(x, y, player1);

    // Edge Checks
    up |= (y == 0);
    down |= (y == 8);
    left |= (x == 0);
    right |= (x == 8);

    // Upper-Left Wall Check
    if (x > 0 && y > 0)
    {
        up |= board.walls[x-1][y-1] && !board.wall_directions[x-1][y-1];
        left |= board.walls[x-1][y-1] && board.wall_directions[x-1][y-1];
    }

    // Lower-Left Wall Check
    if (x > 0 && y < 8) // x < 8 && y > 0
    {
        down |= board.walls[x-1][y] && !board.wall_directions[x-1][y];
        left |= board.walls[x-1][y] && board.wall_directions[x-1][y];
    }

    // Lower-Right Wall Check
    if (x < 8 && y < 8)
    {
        down |= board.walls[x][y] && !board.wall_directions[x][y];
        right |= board.walls[x][y] && board.wall_directions[x][y];
    }

    // Upper-Right Wall Check
    if (x < 8 && y > 0)
    {
        up |= board.walls[x][y-1] && !board.wall_directions[x][y-1];
        right |= board.walls[x][y-1] && board.wall_directions[x][y-1];
    }
}

inline std::vector<Move> valid_player_moves(Board board, bool player1)
{
    // Primary Player Position
    unsigned char x, y;
    board.get_player_positions(x, y, player1);

    // Movement Check Booleans
    bool up = false, down = false, left = false, right = false;
    valid_adjacent_tiles(board, player1, up, down, left, right);

    // Secondary Player Position
    unsigned char x_other, y_other;
    board.get_player_positions(x_other, y_other, !player1);

    // Secondary Movement Check Booleans
    bool up_other = false, down_other = false, left_other = false, right_other = false;
    valid_adjacent_tiles(board, !player1, up_other, down_other, left_other, right_other);

    // Possible Moves Vector
    std::vector<Move> moves;

    if(!up)
    {
        // Check if Opponent is In Space
        if ((x == x_other) && (y-1 == y_other))
        {
            if (!up_other)
                moves.push_back(Move(x, y-2, false, false, player1));
            else
            {
                if (!left_other)
                    moves.push_back(Move(x-1, y-1, false, false, player1));
                if(!right_other)
                    moves.push_back(Move(x+1, y-1, false, false, player1));
            }
        }
        else
        {
            moves.push_back(Move(x, y-1, false, false, player1));
        }
    }

    if (!down)
    {
        // Check if Opponent is In Space
        if ((x == x_other) && (y+1 == y_other))
        {
            if (!down_other)
                moves.push_back(Move(x, y+2, false, false, player1));
            else
            {
                if (!left_other)
                    moves.push_back(Move(x-1, y+1, false, false, player1));
                if(!right_other)
                    moves.push_back(Move(x+1, y+1, false, false, player1));
            }
        }
        else
        {
            moves.push_back(Move(x, y+1, false, false, player1));
        }
    }

    if(!left)
    {
        // Check if Opponent is In Space
        if ((x-1 == x_other) && (y == y_other))
        {
            if (!left_other)
                moves.push_back(Move(x-2, y, false, false, player1));
            else
            {
                if (!up_other)
                    moves.push_back(Move(x-1, y-1, false, false, player1));
                if(!down_other)
                    moves.push_back(Move(x-1, y+1, false, false, player1));
            }
        }
        else
        {
            moves.push_back(Move(x-1, y, false, false, player1));
        }
    }

    if(!right)
    {
        // Check if Opponent is In Space
        if ((x+1 == x_other) && (y == y_other))
        {
            if (!right_other)
                moves.push_back(Move(x+2, y, false, false, player1));
            else
            {
                if (!up_other)
                    moves.push_back(Move(x+1, y-1, false, false, player1));
                if(!down_other)
                    moves.push_back(Move(x+1, y+1, false, false, player1));
            }
        }
        else
        {
            moves.push_back(Move(x+1, y, false, false, player1));
        }
    }

    return moves;
}

inline int goal_bfs_distance(Board board, bool player1)
{
    std::queue<Point> bfs_queue;
    std::map<Point, Point> tree;

    unsigned char px, py;
    board.get_player_positions(px, py, player1);
    Point start = std::make_pair((int)px, (int)py);
    bfs_queue.push(start);
    tree[start] = std::make_pair(-1, -1);

    bool output_found = false;
    Point output;

    while (!bfs_queue.empty())
    {
        Point v = bfs_queue.front();
        bfs_queue.pop();
        // std::cout << std::to_string(((player1 && v.second == 0) || (!player1 && v.second == 8))) << std::endl;
        // std::cout << std::to_string(v.second) << std::endl;
        if ((player1 && v.second == 0) || (!player1 && v.second == 8))
        {
            output = v;
            output_found = true;
            break;
        }

        Board temp_board = board;
        temp_board.apply_move(Move(v.first, v.second, false, false, player1));
        std::vector<Move> moves = valid_player_moves(temp_board, player1);
        for (Move move : moves)
        {
            unsigned char x, y;
            move.get_position(x, y);
            Point w = std::make_pair((int)x, (int)y);
            if (tree.find(w) == tree.end())
            {
                tree[w] = v;
                bfs_queue.push(w);
            }
        }
    }

    if (!output_found)
        return -1;
    else
    {
        int levels = 0;
        bool backtrack_complete = false;
        while (!backtrack_complete)
        {
            output = tree[output];
            if (output.first == -1 && output.second == -1)
            {
                backtrack_complete = true;
                break;
            }
            levels++;
        }
        return levels;
    }
}

#endif
