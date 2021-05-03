#ifndef optimization_omp_h
#define optimization_omp_h

#include <string>
#include <iostream>
#include <unordered_map>

#include "game.h"

#define SAVE_DEPTH 3
#define MAX_BRANCHING 133
#define NEGAMAX //ALPHABETA

typedef std::unordered_map<Board, Transposition, BoardHasher> table_map;

Move minimax_parallel_helper(Board board, unsigned long ideal_depth, bool player1);
float alphabeta(Board board, unsigned int depth, float alpha, float beta, bool player1);
float negamax(Board board, unsigned int depth, float alpha, float beta, bool player1, table_map& table);


#ifdef ALPHABETA
Move minimax_parallel_helper(Board board, unsigned long ideal_depth, bool player1)
{
    // Set Initial Alpha-Beta Parameters and Find All Fist-Level Moves
    float alpha = -INFINITY;
    float beta = INFINITY;
    float best_score = player1 ? alpha : beta;
    std::vector<Move> first_level_moves = generate_valid_moves(board, player1);
    Move best_move = first_level_moves[0];

    // Estimate the Number of Nodes Expended
    Board test_board = board;
    unsigned int depth = ideal_depth;
    test_board.apply_move(first_level_moves[0]);
    float ideal_nodes = pow(MAX_BRANCHING, ideal_depth);
    unsigned int branching_p1 = first_level_moves.size();
    unsigned int branching_p2 = generate_valid_moves(test_board, !player1).size();
    if (branching_p1 > 0 && branching_p2 > 0)
    {
        do
        {
            depth++;
        }
        while(pow(branching_p1, depth/2 + depth%2)*pow(branching_p2, depth/2) < ideal_nodes);
        depth--;
        std::cout << "Depth Searched: " << std::to_string(depth) << std::endl;
    }

    // Create Transposition Table to Cache Results
    table_map transposition_table;
    transposition_table.reserve((int) pow(MAX_BRANCHING, SAVE_DEPTH));

    #pragma omp parallel
    {
        Board thread_board = board;

        #pragma omp for schedule(static)
        for (unsigned int i = 0; i < first_level_moves.size(); i++)
        {
            // TODO: Add Check for Completed Game?
            Board move_board = board;
            Move m = first_level_moves[i];
            move_board.apply_move(m);
            float score = alphabeta(move_board, depth-1, alpha, beta, !player1, transposition_table);

            if (player1)
            {
                #pragma omp critical
                {
                    if (score > best_score)
                    {
                        best_score = score;
                        best_move = m;
                        alpha = best_score;
                    }
                }
            }
            else
            {
                #pragma omp critical
                {
                    if (score < best_score)
                    {
                        best_score = score;
                        best_move = m;
                        beta = best_score;
                    }
                }
            }
        }
    }
    // std::cout << std::to_string(best_score) << std::endl;
    return best_move;
}
#endif

#ifdef NEGAMAX
Move minimax_parallel_helper(Board board, unsigned long ideal_depth, bool player1)
{
    // Set Initial Alpha-Beta Parameters and Find All Fist-Level Moves
    float alpha = -INFINITY;
    float beta = INFINITY;
    float best_score = -INFINITY;
    std::vector<Move> first_level_moves = generate_valid_moves(board, player1);
    Move best_move = first_level_moves[0];

    // Estimate the Number of Nodes Expended
    Board test_board = board;
    unsigned int depth = ideal_depth;
    test_board.apply_move(first_level_moves[0]);
    float ideal_nodes = pow(MAX_BRANCHING, ideal_depth);
    unsigned int branching_p1 = first_level_moves.size();
    unsigned int branching_p2 = generate_valid_moves(test_board, !player1).size();
    if (branching_p1 > 0 && branching_p2 > 0)
    {
        do
        {
            depth++;
        }
        while(pow(branching_p1, depth/2 + depth%2)*pow(branching_p2, depth/2) < ideal_nodes);
        depth--;
        std::cout << "Depth Searched: " << std::to_string(depth) << std::endl;
    }

    // Create Transposition Table to Cache Results
    table_map transposition_table;
    transposition_table.reserve((int) pow(MAX_BRANCHING, SAVE_DEPTH));

    #pragma omp parallel shared(best_move, best_score, alpha, transposition_table)
    {
        Board thread_board = board;

        #pragma omp for schedule(static)
        for (unsigned int i = 0; i < first_level_moves.size(); i++)
        {
            // TODO: Add Check for Completed Game?
            Board move_board = thread_board;
            Move m = first_level_moves[i];
            move_board.apply_move(m);
            float score = -negamax(move_board, depth-1, -beta, -alpha, !player1, transposition_table);

            #pragma omp critical
            {
                if (score > best_score)
                {
                    best_score = score;
                    best_move = m;
                    alpha = best_score;
                }
            }
        }
    }
    // std::cout << std::to_string(best_score) << std::endl;
    return best_move;
}
#endif

float alphabeta(Board board, unsigned int depth, float alpha, float beta, bool player1)
{
    float alpha_orig = alpha;
    float beta_orig = beta;

    // If at a lead node or the depth limit, return a hueristic
    if ((depth == 0 ) || (board.game_over()))
        return calculate_heurisitic(board, true);

    // Investigate Child Nodes
    float value;
    if (player1)
    {
        value = -INFINITY;
        std::vector<Move> valid_moves = generate_valid_moves(board, player1);
        for (Move move : valid_moves)
        {
            Board child = board;
            child.apply_move(move);
            value = std::max(value, alphabeta(child, depth-1, alpha, beta, !player1));
            alpha = std::max(alpha, value);
            if (alpha >= beta)
                break;
        }
    }
    else
    {
        value = INFINITY;
        std::vector<Move> valid_moves = generate_valid_moves(board, player1);
        for (Move move : valid_moves)
        {
            Board child = board;
            child.apply_move(move);
            value = std::min(value, alphabeta(child, depth-1, alpha, beta, !player1));
            beta = std::min(beta, value);
            if (beta <= alpha)
                break;
        }
    }

    // Return Result
    return value;
}

float negamax(Board board, unsigned int depth, float alpha, float beta, bool player1, table_map& table)
{
    float alpha_orig = alpha;

    // Check Transposition Table for a Previous Run
    table_map::const_iterator table_location = table.find(board);
    if (table_location != table.end() && table_location->second.depth >= depth)
    {
        Transposition node = table_location->second;
        unsigned char saved_flag = node.flag;
        float saved_value = node.value;
        if (saved_flag == 0)
            return saved_value;
        else if (saved_flag == -1)
            alpha = std::max(alpha, saved_value);
        else
            beta = std::min(beta, saved_value);

        if (alpha >= beta)
            return saved_value;
    }

    if ((depth == 0 ) || (board.game_over()))
        return calculate_heurisitic(board, player1);

    std::vector<Move> valid_moves = generate_valid_moves(board, player1);
    float value = -INFINITY;
    for (Move move : valid_moves)
    {
        Board child = board;
        child.apply_move(move);
        value = std::max(value, -negamax(child, depth - 1, -beta, -alpha, !player1, table));
        alpha = std::max(alpha, value);
        if (alpha >= beta)
            break;
    }

    // Store Information in Transposition table
    Transposition tt_entry;
    if (value <= alpha_orig)
        tt_entry.flag = 1;
    else if (value >= beta)
        tt_entry.flag = -1;
    else
        tt_entry.flag = 0;
    tt_entry.depth = depth;
    tt_entry.value = value;
    table[board] = tt_entry;

    return value;
}

#endif
