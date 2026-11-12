#pragma once
#include <vector>
#include "board.hpp"
#include "move.hpp"

void makeMove(Board& board, const Move& m);
void undoLastMove(Board& board);
std::vector<Move> generateMoves(const Board& board);
std::string MovesToString(std::vector<Move>& moves, int base);