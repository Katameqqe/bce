#pragma once
#include <vector>
#include "board.hpp"
#include "unordered_map"

struct Moves {
    std::vector<int> to;

    std::string toString(int piece) const;
};

std::unordered_map<int, Moves> generateMoves(const Board& board);