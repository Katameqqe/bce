#include <iostream>
#include "board.hpp"
#include "movegen.hpp"

size_t depth_perft(Board& b, int depth) {
    if (depth == 0) return generateMoves(b).size();
    std::vector<Move> moves = generateMoves(b);
    size_t allMoves = 0;
    for (auto &m : moves)
    {
        b.makeMove(m);
        allMoves += depth_perft(b, depth-1);
        b.undoMove();
    }
    return allMoves;
}

int main() {
    Board board;
    std::cout << board.toString() << std::endl;
    
    Board b;
    printf(" Depth %d:\t%zu\n", 1, depth_perft(b, 0));
    printf(" Depth %d:\t%zu\n", 2, depth_perft(b, 1));
    printf(" Depth %d:\t%zu\n", 3, depth_perft(b, 2));
    printf(" Depth %d:\t%zu\n", 4, depth_perft(b, 3));
    printf(" Depth %d:\t%zu\n", 5, depth_perft(b, 4));
}