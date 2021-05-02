#ifndef board_h
#define board_h

#include <string>
#include <vector>

#include <boost/functional/hash.hpp>

struct Move
{
    uint8_t new_pos;
    bool wall_place;
    bool wall_up;
    bool player1;

    Move() {}

    Move(unsigned char x, unsigned char y, bool wall, bool up, bool player)
    {
        set_position(x, y);
        wall_place = wall;
        wall_up = up;
        player1 = player;
    }

    inline void set_position(unsigned char x, unsigned char y)
    {
        new_pos = ((x & 0x0F) << 4) | (y & 0x0F);
    }

    inline void get_position(unsigned char& x, unsigned char& y)
    {
        x = (new_pos & 0xF0) >> 4;
        y = (new_pos & 0x0F);
    }

    inline bool operator==(const Move& rhs)
    {
        return new_pos == rhs.new_pos &&
               wall_place == rhs.wall_place &&
               wall_up == rhs.wall_up &&
               player1 == rhs.player1;
    }
};

inline bool generate_from_string(std::string input, bool player1, Move& move)
{
    if (input.length() != 3)
        return false;

    if (input[0] == 'W')
    {
        move.wall_place = true;
        move.wall_up = true;
    }
    else if (input[0] == 'B')
    {
        move.wall_place = true;
        move.wall_up = false;
    }
    else if (input[0] == 'M')
    {
        move.wall_place = false;
        move.wall_up = false;
    }
    else
    {
        return false;
    }

    if (std::isdigit(input[1]) && std::isdigit(input[2]))
    {
        unsigned char x = (unsigned char) std::stoi(std::string(1, input[1]));
        unsigned char y = (unsigned char) std::stoi(std::string(1, input[2]));
        move.set_position(x, y);
        return true;
    }
    return false;
}

struct Board
{
    uint8_t p1_pos;
    uint8_t p2_pos;
    bool walls[8][8];
    bool wall_directions[8][8];
    uint8_t p1_walls_left;
    uint8_t p2_walls_left;

    Board()
    {
        for (unsigned int i = 0; i < 8; i++)
        {
            for (unsigned int j = 0; j < 8; j++)
            {
                walls[i][j] = false;
            }
        }
    }

    inline void apply_move(Move move)
    {
        if (move.wall_place)
        {
            if (move.player1)
                p1_walls_left--;
            else
                p2_walls_left--;

            unsigned char x, y;
            move.get_position(x, y);
            walls[x][y] = true;
            wall_directions[x][y] = move.wall_up;
        }
        else
        {
            unsigned char x, y;
            move.get_position(x, y);
            set_player_position(x, y, move.player1);
        }
    }

    inline void set_player_position(unsigned char x, unsigned char y, bool player1)
    {
        if (player1)
            p1_pos = ((x & 0x0F) << 4) | (y & 0x0F);
        else
            p2_pos = ((x & 0x0F) << 4) | (y & 0x0F);
    }

    inline void get_player_positions(unsigned char& x, unsigned char& y, bool player1)
    {
        if (player1)
        {
            x = (p1_pos & 0xF0) >> 4;
            y = (p1_pos & 0x0F);
        }
        else
        {
            x = (p2_pos & 0xF0) >> 4;
            y = (p2_pos & 0x0F);
        }
    }

    inline void set_wall(unsigned char x, unsigned char y, bool up)
    {
        walls[x][y] = true;
        wall_directions[x][y] = up;
    }

    inline void set_player_walls_left(unsigned char walls_left, bool p1)
    {
        if (p1)
            p1_walls_left = walls_left;
        else
            p2_walls_left = walls_left;
    }

    inline bool game_over()
    {
        unsigned char p1_x, p1_y, p2_x, p2_y;
        get_player_positions(p1_x, p1_y, true);
        get_player_positions(p2_x, p2_y, false);
        return ((p1_y == 0) || (p2_y == 8));
    }

    inline bool operator==(const Board& rhs) const
    {
        bool same_walls = true;
        for (unsigned int i = 0; i < 8; i++)
        {
            for (unsigned int j = 0; j < 8; j++)
            {
                if (walls[i][j] != rhs.walls[i][j] ||
                    wall_directions[i][j] != rhs.wall_directions[i][j])
                {
                    same_walls = false;
                    break;
                }
            }
        }

        return same_walls &&
               p1_pos == rhs.p1_pos &&
               p2_pos == rhs.p2_pos &&
               p1_walls_left == rhs.p1_walls_left &&
               p2_walls_left == rhs.p2_walls_left;
    }
};

struct BoardHasher
{
    std::size_t operator()(const Board& b) const
    {
        size_t h = 0;
        for (size_t i = 0; i < 8; i++)
        {
            for (size_t j = 0; j < 8; j++)
            {
                boost::hash_combine(h, b.walls[i][j]);
                boost::hash_combine(h, b.wall_directions[i][j]);
            }
        }
        boost::hash_combine(h, b.p1_pos);
        boost::hash_combine(h, b.p2_pos);
        boost::hash_combine(h, b.p1_walls_left);
        boost::hash_combine(h, b.p2_walls_left);
        return h;
    }
};


inline Board game_start_board()
{
    Board board;
    board.set_player_position(4, 0, false);
    board.set_player_position(4, 8, true);
    board.set_player_walls_left(10, false);
    board.set_player_walls_left(10, true);
    return board;
}

inline std::vector<std::string> split (std::string s, std::string delimiter)
{
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos)
    {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

inline Board generate_from_text(std::string input)
{
    Board board;
    unsigned char p1_walls_left = 10;
    unsigned char p2_walls_left = 10;

    std::vector<std::string> lines = split(input, "\n");
    for (unsigned int i = 0; i < lines.size(); i++)
    {
        if (i == 0 || i > 18)
            continue;

        std::string line = lines[i];
        if (i % 2 == 0)
        {
            for (unsigned int j = 0; j < line.length(); j++)
            {
                if (j % 2 == 0)
                    continue;

                else if (line[j] == '!')
                {
                    board.set_wall((j-1)/2, (i/2)-1, true);
                    p1_walls_left--;
                }
                else if (line[j] == '=')
                {
                    board.set_wall((j-1)/2, (i/2)-1, false);
                    p1_walls_left--;
                }
                else if (line[j] == '|')
                {
                    board.set_wall((j-1)/2, (i/2)-1, true);
                    p2_walls_left--;
                }
                else if (line[j] == '-')
                {
                    board.set_wall((j-1)/2, (i/2)-1, false);
                    p2_walls_left--;
                }
            }
        }
        else
        {
            for (unsigned int j = 0; j < line.length(); j++)
            {
                if (j % 2 != 0)
                    continue;
                else if (line[j] == 'S')
                    board.set_player_position(j/2, (i-1)/2, false);
                else if (line[j] == 'P')
                    board.set_player_position(j/2, (i-1)/2, true);
            }
        }
    }

    board.set_player_walls_left(p1_walls_left, true);
    board.set_player_walls_left(p2_walls_left, false);
    return board;
}

inline std::string output_to_ascii(Board board)
{
    std::string output = "-+-+-+-+-+-+-+-+-\n";
    std::vector<std::string> pieces;

    unsigned char p1_x, p1_y, p2_x, p2_y;
    board.get_player_positions(p1_x, p1_y, true);
    board.get_player_positions(p2_x, p2_y, false);

    for (unsigned int i = 0; i < 9; i++)
    {
        std::string line = "";
        for (unsigned int j = 0; j < 9; j++)
        {
            if (j == p1_x && i == p1_y)
                line += "P";
            else if ((j == p2_x) && (i == p2_y))
                line += "S";
            else
                line += "O";

            if (j < 8)
                line += " ";
        }
        pieces.push_back(line);
    }

    std::vector<std::string> walls;
    unsigned char remaining_p1_walls = 10-board.p1_walls_left;
    for (unsigned int i = 0; i < 8; i++)
    {
        std::string line = " ";
        for (unsigned int j = 0; j < 8; j++)
        {
            if (board.walls[j][i])
            {
                if (remaining_p1_walls > 0)
                {
                    remaining_p1_walls--;
                    if (board.wall_directions[j][i])
                        line += "!";
                    else
                        line += "=";
                }
                else
                {
                    if (board.wall_directions[j][i])
                        line += "|";
                    else
                        line += "-";
                }
            }
            else
            {
                line += " ";
            }
            line += " ";
        }
        walls.push_back(line);
    }

    for (unsigned int i = 0; i < 17; i++)
    {
        if (i % 2 == 0)
            output += pieces[i/2];
        else
            output += walls[(i-1)/2];
        output += "\n";
    }

    output += "-+-+-+-+-+-+-+-+-\n";
    return output;
}

#endif
