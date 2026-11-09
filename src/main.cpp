#include <iostream>
#include "board.hpp"
#include "movegen.hpp"

int main() {
    Board board;
    std::cout << board.toString() << std::endl;
    
    Board b;
    b.whiteToMove = true;
    b.squares.fill('.');
    const int pos = 1 * 8 + 4;
    b.squares[pos] = 'P';
    b.squares[2 * 8 + 3] = 'b';
    b.squares[3 * 8 + 5] = 'P';
    b.squares[7 * 8 + 1] = 'P';
    b.squares[2 * 8 + 6] = 'p';
    std::cout << b.toString() << std::endl;
    std::vector<Move> moves = generateMoves(b);
    std::cout << MovesToString(moves, pos) << std::endl;
}