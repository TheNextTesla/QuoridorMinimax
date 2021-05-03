
#include <string>
#include <fstream>
#include <iostream>
#include <streambuf>

#include "board.h"

int main(int argc, char *argv[])
{
    std::ifstream t("input.txt");
    std::string str((std::istreambuf_iterator<char>(t)),
                     std::istreambuf_iterator<char>());

    Board board = generate_from_text(str);
    Board new_board = board;
    // std::cout << std::to_string(board.p1_walls_left) << std::endl;
    std::cout << output_to_ascii(new_board);
    return 0;
}
