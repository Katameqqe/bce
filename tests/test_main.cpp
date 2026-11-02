#include <iostream>
#include "board.hpp"
#include "movegen.hpp"

static int failures = 0;
#define CHECK(cond)                                                        \
    do {                                                                   \
        if (!(cond)) {                                                     \
            std::cerr << "FAILED: " #cond " (line " << __LINE__ << ")\n";  \
            ++failures;                                                    \
        }                                                                  \
    } while (0)

void testStartPosition() {
    Board b;
    int pieces = 0;
    for (char c : b.squares)
        if (c != '.') ++pieces;
    CHECK(pieces == 32);
    CHECK(b.squares[4] == 'K');   // e1
    CHECK(b.squares[60] == 'k');  // e8
}

void testKnightMovesCenter() {
    Board b;
    b.squares.fill('.');
    b.squares[3 * 8 + 3] = 'N';  // knight on d4
    CHECK(generateMoves(b).size() == 8);
}

void testBishopMovesCenter() {
    Board b;
    b.squares.fill('.');
    b.squares[3 * 8 + 3] = 'B';
    CHECK(generateMoves(b).size() == 13);
}

int main() {
    testStartPosition();
    testKnightMovesCenter();
    testBishopMovesCenter();
    if (failures == 0) std::cout << "All tests passed\n";
    return failures == 0 ? 0 : 1;
}