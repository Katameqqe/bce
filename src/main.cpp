#include <iostream>
#include "board.hpp"
#include "movegen.hpp"

int main() {
    Board board;
    std::cout << board.toString() << std::endl;
    
    Board b;
    b.squares.fill('.');
    int pos = 3 * 8 + 5;
    b.squares[pos] = 'Q';
    b.squares[3 * 8 + 2] = 'b';
    b.squares[5 * 8 + 5] = 'P';
    b.squares[7 * 8 + 1] = 'P';
    std::unordered_map<int, Moves> moves = generateMoves(b);
    std::cout << moves[pos].toString(pos) << std::endl;
}