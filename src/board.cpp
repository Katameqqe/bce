#include "board.hpp"
#include "movegen.hpp"

Board::Board() { setStartPosition(); }

void Board::setStartPosition() {
    squares.fill('.');
    const char back[8] = {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'};
    for (int f = 0; f < 8; ++f) {
        squares[0 * 8 + f] = back[f];
        squares[1 * 8 + f] = 'P';
        squares[6 * 8 + f] = 'p';
        squares[7 * 8 + f] = static_cast<char>(back[f] + 32);
    }
    whiteToMove = true;
}

void Board::makeMove(const Move& m) {
    ::makeMove(*this, std::move(m));
}

void Board::undoMove() {
    ::undoLastMove(*this);
}

void Board::addPlayedMove(const MoveEntry& m) {
    playedMoves.push_back(std::move(m));
}

std::vector<MoveEntry>& Board::getMoveHistory() {
    return playedMoves;
}

std::string Board::toString() const {
    std::string out;
    for (int r = 7; r >= 0; --r) {
        out += static_cast<char>('1' + r);
        for (int f = 0; f < 8; ++f) {
            out += ' ';
            out += squares[r * 8 + f];
        }
        out += '\n';
    }
    out += "  a b c d e f g h\n";
    return out;
}