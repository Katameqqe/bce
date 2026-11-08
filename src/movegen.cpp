#include "movegen.hpp"

std::string Moves::toString(int base) const {
    std::array<char, 64> squares{};
    squares.fill('.');
    for (auto& m: to) {
        squares[m] = 'x';
    }
    squares[base] = 'o';
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

std::unordered_map<int, Moves> generateMoves(const Board& board) {
    std::unordered_map<int, Moves> moves;
    const int knightOffsets[8][2] = {{1,2},{2,1},{2,-1},{1,-2},{-1,-2},{-2,-1},{-2,1},{-1,2}};
    const int bishopOffsets[4][2] = {{1,1},{1,-1},{-1,-1},{-1,1}};
    const int rookOffsets[4][2] = {{1,0},{0,1},{0,-1},{-1,0}};
    const int queenOffsets[8][2] = {{1,1},{1,-1},{-1,-1},{-1,1},{1,0},{0,1},{0,-1},{-1,0}};
    const int pawnAttackOffsets[2][2] = {{1,1}, {-1,1}};
    const int pawnMoveOffsets[2][2] = {{0,1},{0,2}};
    const int kingOffsets[8][2] = {{0,1},{1,1},{1,0},{1,-1},{-1,0},{-1,-1},{0,-1},{-1, 1}};

    for (int sq = 0; sq < 64; ++sq) {
        char piece = board.squares[sq];
        if (piece == '.') continue;

        bool isWhite = (piece >= 'A' && piece <= 'Z');
        if (isWhite != board.whiteToMove) continue;

        int file = sq % 8, rank = sq / 8;

        if (piece == 'N' || piece == 'n') {
            
            for (auto& o : knightOffsets) {
                int f = file + o[0], r = rank + o[1];
                if (f < 0 || f > 7 || r < 0 || r > 7) continue;
                int to = r * 8 + f;
                char target = board.squares[to];
                bool targetIsOwn = (target != '.') && ((target >= 'A' && target <= 'Z') == isWhite);
                if (targetIsOwn) continue;
                moves[sq].to.push_back(to);
            }
        }

        if (piece == 'B' || piece == 'b') {
            for (auto& o: bishopOffsets) {
                int f = file, r = rank;
                while (true)
                {
                    f += o[0];
                    r += o[1];
                    if (f < 0 || f > 7 || r < 0 || r > 7) break;
                    int to = r * 8 + f;
                    char target = board.squares[to];
                    if (target == '.') {
                        moves[sq].to.push_back(to);
                        continue;
                    }
                    bool targetIsOwn = (target >= 'A' && target <= 'Z') == isWhite;
                    if (!targetIsOwn) moves[sq].to.push_back(to);
                    break;
                }
            }
        }

        if (piece == 'R' || piece == 'r') {
            for (auto& o: rookOffsets) {
                int f = file, r = rank;
                while (true)
                {
                    f += o[0];
                    r += o[1];
                    if (f < 0 || f > 7 || r < 0 || r > 7) break;
                    int to = r * 8 + f;
                    char target = board.squares[to];
                    if (target == '.') {
                        moves[sq].to.push_back(to);
                        continue;
                    }
                    bool targetIsOwn = (target >= 'A' && target <= 'Z') == isWhite;
                    if (!targetIsOwn) moves[sq].to.push_back(to);
                    break;
                }
            }
        }

        if (piece == 'Q' || piece == 'q') {
            for (auto& o: queenOffsets) {
                int f = file, r = rank;
                while (true)
                {
                    f += o[0];
                    r += o[1];
                    if (f < 0 || f > 7 || r < 0 || r > 7) break;
                    int to = r * 8 + f;
                    char target = board.squares[to];
                    if (target == '.') {
                        moves[sq].to.push_back(to);
                        continue;
                    }
                    bool targetIsOwn = (target >= 'A' && target <= 'Z') == isWhite;
                    if (!targetIsOwn) moves[sq].to.push_back(to);
                    break;
                }
            }
        }

        if (piece == 'P' || piece == 'p') {
            int act = 1;
            if (!isWhite) act = -1;
            for (auto &o : pawnAttackOffsets) {
                int f = file + o[0], r = rank + o[1]*act;
                if (f < 0 || f > 7 || r < 0 || r > 7) continue;
                int to = r * 8 + f;
                char target = board.squares[to];
                bool targetIsOwn = ((target >= 'A' && target <= 'Z') == isWhite);
                if (targetIsOwn || target == '.') continue;
                moves[sq].to.push_back(to);
            }

            for (auto &o : pawnMoveOffsets)
            {
                int f = file + o[0], r = rank + o[1]*act;
                if (f < 0 || f > 7 || r < 0 || r > 7) continue;
                int to = r * 8 + f;
                char target = board.squares[to];
                if (target == '.') moves[sq].to.push_back(to);
                if ((isWhite && rank != 1) || (!isWhite && rank != 6)) break;
            }
        }

        if (piece == 'K' || piece == 'k') {
            for (auto& o: kingOffsets) {
                int f = file+o[0], r = rank+o[1];
                if (f < 0 || f > 7 || r < 0 || r > 7) continue;
                int to = r * 8 + f;
                char target = board.squares[to];
                bool targetIsOwn = (target != '.') && ((target >= 'A' && target <= 'Z') == isWhite);
                if (targetIsOwn) continue;
                moves[sq].to.push_back(to);
            }
        }
        // TODO: advanced moves, pins and checks
    }
    return moves;
}