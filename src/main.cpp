#include <iostream>
#include "board.hpp"
#include "movegen.hpp"

int main() {
    Board board;
    std::cout << board.toString() << std::endl;
    
    Board b;
    b.whiteToMove = false;
    b.squares.fill('.');
    const int pos = 0 * 8 + 4;
    b.squares[pos] = 'k';
    b.squares[3 * 8 + 3] = 'b';
    b.squares[3 * 8 + 5] = 'P';
    b.squares[7 * 8 + 1] = 'P';
    b.squares[2 * 8 + 6] = 'p';
    std::cout << b.toString() << std::endl;
    std::unordered_map<int, Moves> moves = generateMoves(b);
    std::cout << moves[pos].toString(pos) << std::endl;
}