#pragma once
#include <array>
#include <string>
#include <cstdint>
#include <vector>
#include "move.hpp"

class Board {
public:
    Board();

    std::array<char, 64> squares{};
    bool whiteToMove = true;

    CastleFlags flags;

    uint8_t enPassant = 0;

    void setStartPosition();
    std::string toString() const;
    
    void makeMove(const Move& m);
    void undoMove();

    void addPlayedMove(const MoveEntry& m);
    std::vector<MoveEntry>& getMoveHistory();
private:
    std::vector<MoveEntry> playedMoves;
};