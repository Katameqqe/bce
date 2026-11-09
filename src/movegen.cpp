#include "movegen.hpp"
#include <ranges>


std::string MovesToString(std::vector<Move>& moves, int base) {
    std::array<char, 64> squares{};
    squares.fill('.');
    auto view = moves | std::views::filter([base](const Move& m) {return m.from == base;});
    for (const auto& m: view) {
        squares[m.to] = 'x';
        if (m.promotion != Promotion::None) squares[m.to] = '^';
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

void makeMove(Board& board, const Move& m) {
    char RemovedPiece = board.squares[m.to];
    char movedPiece = board.squares[m.from];
    board.squares[m.to] = movedPiece;
    board.squares[m.from] = '.';
    uint8_t res;
    switch (m.flag)
    {
    case Flag::DoublePush:
        res = (board.whiteToMove) ? m.to - 8 + m.to % 8 : m.to + 8 + m.to % 8;
        board.enPassant = res;
        break;
    case Flag::EnPassant:
        res = (board.whiteToMove) ? m.to - 8 + m.to % 8 : m.to + 8 + m.to % 8;
        RemovedPiece = board.squares[res];
        board.squares[res] = '.';
    case Flag::Castle:
        break;
    }
    if (m.flag != Flag::DoublePush) {
        board.enPassant = 0;
    }
    if ((movedPiece == 'R' && (board.WhiteCastleK || board.WhiteCastleQ )) ||
        (movedPiece == 'r' && (board.BlackCastleK || board.BlackCastleQ ))) {
        uint8_t file = m.from % 8;
        if (board.whiteToMove && (file == 0)) {
            board.WhiteCastleQ = false;
        } else if (board.whiteToMove && (file == 7)) {
            board.WhiteCastleK = false;
        }
        if (!board.whiteToMove && (file == 0)) {
            board.BlackCastleQ = false;
        } else if (!board.whiteToMove && (file == 7)) {
            board.BlackCastleK = false;
        }
    }
    if ((movedPiece == 'K' && (board.WhiteCastleK || board.WhiteCastleQ )) ||
        (movedPiece == 'k' && (board.BlackCastleK || board.BlackCastleQ ))) {
        if (board.whiteToMove && m.from == (0*8+4)) {
            board.WhiteCastleK = false;
            board.WhiteCastleQ = false;
        }
        else if (!board.whiteToMove && m.from == (7*7+4)) {
            board.BlackCastleK = false;
            board.BlackCastleQ = false;
        }
    }
    if (m.promotion != Promotion::None) {
        switch (m.promotion)
        {
        case Promotion::Knight:
            board.squares[m.to] = (board.whiteToMove) ? 'K' : 'k';
            break;
        case Promotion::Bishop:
            board.squares[m.to] = (board.whiteToMove) ? 'B' : 'b';
            break;
        case Promotion::Rook:
            board.squares[m.to] = (board.whiteToMove) ? 'R' : 'r';
            break;
        case Promotion::Queen:
            board.squares[m.to] = (board.whiteToMove) ? 'Q' : 'q';
            break;
        }
    }
    board.whiteToMove = !board.whiteToMove;
}

std::vector<Move> generateMoves(const Board& board) {
    std::vector<Move> moves;
    const int knightOffsets[8][2] = {{1,2},{2,1},{2,-1},{1,-2},{-1,-2},{-2,-1},{-2,1},{-1,2}};
    const int bishopOffsets[4][2] = {{1,1},{1,-1},{-1,-1},{-1,1}};
    const int rookOffsets[4][2] = {{1,0},{0,1},{0,-1},{-1,0}};
    const int queenOffsets[8][2] = {{1,1},{1,-1},{-1,-1},{-1,1},{1,0},{0,1},{0,-1},{-1,0}};
    const int pawnAttackOffsets[2][2] = {{1,1}, {-1,1}};
    const int pawnMoveOffsets[2][2] = {{0,1},{0,2}};
    const int kingOffsets[8][2] = {{0,1},{1,1},{1,0},{1,-1},{-1,0},{-1,-1},{0,-1},{-1, 1}};

    for (uint8_t sq = 0; sq < 64; ++sq) {
        char piece = board.squares[sq];
        if (piece == '.') continue;

        bool isWhite = (piece >= 'A' && piece <= 'Z');
        if (isWhite != board.whiteToMove) continue;

        int file = sq % 8, rank = sq / 8;

        if (piece == 'N' || piece == 'n') {
            
            for (auto& o : knightOffsets) {
                int f = file + o[0], r = rank + o[1];
                if (f < 0 || f > 7 || r < 0 || r > 7) continue;
                uint8_t to = r * 8 + f;
                char target = board.squares[to];
                bool targetIsOwn = (target != '.') && ((target >= 'A' && target <= 'Z') == isWhite);
                if (targetIsOwn) continue;
                moves.push_back(Move{
                    .from = sq,
                    .to = to,
                    .promotion = Promotion::None,
                    .flag = Flag::Normal
                });
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
                    uint8_t to = r * 8 + f;
                    char target = board.squares[to];
                    if (target == '.') {
                        moves.push_back(Move{
                            .from = sq,
                            .to = to,
                            .promotion = Promotion::None,
                            .flag = Flag::Normal
                        });;
                        continue;
                    }
                    bool targetIsOwn = (target >= 'A' && target <= 'Z') == isWhite;
                    if (!targetIsOwn) 
                        moves.push_back(Move{
                            .from = sq,
                            .to = to,
                            .promotion = Promotion::None,
                            .flag = Flag::Normal
                        });
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
                    uint8_t to = r * 8 + f;
                    char target = board.squares[to];
                    if (target == '.') {
                        moves.push_back(Move{
                            .from = sq,
                            .to = to,
                            .promotion = Promotion::None,
                            .flag = Flag::Normal
                        });
                        continue;
                    }
                    bool targetIsOwn = (target >= 'A' && target <= 'Z') == isWhite;
                    if (!targetIsOwn)
                        moves.push_back(Move{
                            .from = sq,
                            .to = to,
                            .promotion = Promotion::None,
                            .flag = Flag::Normal
                        });
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
                    uint8_t to = r * 8 + f;
                    char target = board.squares[to];
                    if (target == '.') {
                        moves.push_back(Move{
                            .from = sq,
                            .to = to,
                            .promotion = Promotion::None,
                            .flag = Flag::Normal
                        });
                        continue;
                    }
                    bool targetIsOwn = (target >= 'A' && target <= 'Z') == isWhite;
                    if (!targetIsOwn)
                        moves.push_back(Move{
                            .from = sq,
                            .to = to,
                            .promotion = Promotion::None,
                            .flag = Flag::Normal
                        });
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
                uint8_t to = r * 8 + f;
                char target = board.squares[to];
                bool targetIsOwn = ((target >= 'A' && target <= 'Z') == isWhite);
                if (targetIsOwn || target == '.') continue;
                moves.push_back(Move{
                    .from = sq,
                    .to = to,
                    .promotion = Promotion::None,
                    .flag = Flag::Normal
                });
            }

            for (auto &o : pawnMoveOffsets)
            {
                int f = file + o[0], r = rank + o[1]*act;
                if (f < 0 || f > 7 || r < 0 || r > 7) continue;
                uint8_t to = r * 8 + f;
                char target = board.squares[to];
                if (target == '.')
                    moves.push_back(Move{
                        .from = sq,
                        .to = to,
                        .promotion = Promotion::None,
                        .flag = (o[1] == 2) ? Flag::DoublePush : Flag::Normal 
                    });
                else break;
                if ((isWhite && rank != 1) || (!isWhite && rank != 6)) break;
            }
        }

        if (piece == 'K' || piece == 'k') {
            for (auto& o: kingOffsets) {
                int f = file+o[0], r = rank+o[1];
                if (f < 0 || f > 7 || r < 0 || r > 7) continue;
                uint8_t to = r * 8 + f;
                char target = board.squares[to];
                bool targetIsOwn = (target != '.') && ((target >= 'A' && target <= 'Z') == isWhite);
                if (targetIsOwn) continue;
                moves.push_back(Move{
                    .from = sq,
                    .to = to,
                    .promotion = Promotion::None,
                    .flag = Flag::Normal
                });
            }
        }
        // TODO: advanced moves, pins and checks
    }
    return moves;
}