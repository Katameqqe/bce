#pragma once
#include <array>
#include <string>
#include <cstdint>

struct Board {
    std::array<char, 64> squares{};
    bool whiteToMove = true;

    bool WhiteCastleQ = true;
    bool WhiteCastleK = true;
    bool BlackCastleQ = true;
    bool BlackCastleK = true;

    uint8_t enPassant = 0;

    Board();
    void setStartPosition();
    std::string toString() const;
};