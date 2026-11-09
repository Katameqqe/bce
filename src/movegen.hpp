#pragma once
#include <vector>
#include "board.hpp"

enum class Promotion: uint8_t {
    None,
    Bishop,
    Rook,
    Knight,
    Queen
};

enum class Flag: uint8_t {
    Normal,
    DoublePush,
    EnPassant,
    Castle
};

struct Move {
    uint8_t from;
    uint8_t to;
    Promotion promotion;
    Flag flag;
};

std::vector<Move> generateMoves(const Board& board);
std::string MovesToString(std::vector<Move>& moves, int base);