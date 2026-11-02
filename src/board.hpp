#pragma once
#include <array>
#include <string>

struct Board {
    std::array<char, 64> squares{};
    bool whiteToMove = true;

    Board();
    void setStartPosition();
    std::string toString() const;
};