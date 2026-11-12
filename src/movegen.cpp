#include "movegen.hpp"
#include <ranges>
#include <span>

namespace 
{
bool emptyCastleLine(const Board& b, bool isWhite, bool QueenSide) {
    std::span<const char> line{b.squares};
    if (isWhite) line = line.subspan(0, 8);
    else line = line = line.subspan(7*8, 8);
    
    if (QueenSide)
    {
        if (   line[1] == '.'
            && line[2] == '.'
            && line[3] == '.')
            return true;
        else 
            return false;
    } else {
        if (   line[5] == '.'
            && line[6] == '.')
            return true;
        else 
            return false;
    }
}
} // namespace 


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

void undoLastMove(Board& board) {
    auto& history = board.getMoveHistory();
    MoveEntry m = history.back();
    if(m.move.promotion == Promotion::None){
        board.squares[m.move.from] = board.squares[m.move.to]; 
        board.squares[m.move.to] = m.piece;
    }
    else {
        board.squares[m.move.from] = (!board.whiteToMove) ? 'P' : 'p';
        board.squares[m.move.to] = m.piece;
    }

    board.enPassant = m.enPassant;
    board.flags = m.flags;
    board.whiteToMove = !board.whiteToMove;
    history.pop_back();
}

void makeMove(Board& board, const Move& m) {
    char RemovedPiece = board.squares[m.to];
    board.addPlayedMove(MoveEntry{
        .move = m,
        .piece = RemovedPiece,
        .enPassant = board.enPassant,
        .flags = board.flags
    });
    if ((RemovedPiece == 'R' || RemovedPiece == 'r')) {
        if      (m.to == 0) board.flags.WhiteCastleQ = false;
        else if (m.to == 7) board.flags.WhiteCastleK = false;
        else if (m.to == 7*8) board.flags.BlackCastleQ = false;
        else if (m.to == 7*8+7) board.flags.BlackCastleK = false;
    }
    char movedPiece = board.squares[m.from];
    board.squares[m.to] = movedPiece;
    board.squares[m.from] = '.';
    uint8_t res;
    switch (m.flag)
    {
    case Flag::DoublePush:
        res = (board.whiteToMove) ? m.to - 8 : m.to + 8;
        board.enPassant = res;
        break;
    case Flag::EnPassant:
        res = (board.whiteToMove) ? m.to - 8 : m.to + 8;
        RemovedPiece = board.squares[res];
        board.squares[res] = '.';
        break;
    case Flag::Castle:
        if (m.to % 8 == 1) {
            board.squares[m.to+1] = board.squares[m.to-1];
            board.squares[m.to-1] = '.';
        }
        else {
            board.squares[m.to-1] = board.squares[m.to+1];
            board.squares[m.to+1] = '.';
        }
        break;
    case Flag::Normal:
        break;
    }
    if (m.flag != Flag::DoublePush) {
        board.enPassant = 0;
    }
    if ((movedPiece == 'R' && (board.flags.WhiteCastleK || board.flags.WhiteCastleQ )) ||
        (movedPiece == 'r' && (board.flags.BlackCastleK || board.flags.BlackCastleQ ))) {
        uint8_t file = m.from % 8;
        uint8_t rank = m.from / 8;
        if (board.whiteToMove && (file == 0 && rank == 0)) {
            board.flags.WhiteCastleQ = false;
        } else if (board.whiteToMove && (file == 7)) {
            board.flags.WhiteCastleK = false;
        }
        if (!board.whiteToMove && (file == 0 && rank == 7)) {
            board.flags.BlackCastleQ = false;
        } else if (!board.whiteToMove && (file == 7)) {
            board.flags.BlackCastleK = false;
        }
    }
    if ((movedPiece == 'K' && (board.flags.WhiteCastleK || board.flags.WhiteCastleQ )) ||
        (movedPiece == 'k' && (board.flags.BlackCastleK || board.flags.BlackCastleQ ))) {
        if (board.whiteToMove && m.from == (0*8+4)) {
            board.flags.WhiteCastleK = false;
            board.flags.WhiteCastleQ = false;
        }
        else if (!board.whiteToMove && m.from == (7*8+4)) {
            board.flags.BlackCastleK = false;
            board.flags.BlackCastleQ = false;
        }
    }
    switch (m.promotion)
    {
    case Promotion::Knight:
        board.squares[m.to] = (board.whiteToMove) ? 'N' : 'n';
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
    case Promotion::None:
        break;
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
                if (board.enPassant == to && target == '.') {
                    moves.push_back(Move{
                        .from = sq,
                        .to = to,
                        .promotion = Promotion::None,
                        .flag = Flag::EnPassant
                    });
                }
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
            if (isWhite) {
                if (board.flags.WhiteCastleQ && emptyCastleLine(board, isWhite, true)) {
                    moves.push_back(Move{
                        .from = sq,
                        .to = 1,
                        .promotion = Promotion::None,
                        .flag = Flag::Castle
                    });
                }
                if (board.flags.WhiteCastleK && emptyCastleLine(board, isWhite, false)) {
                    moves.push_back(Move{
                        .from = sq,
                        .to = 6,
                        .promotion = Promotion::None,
                        .flag = Flag::Castle
                    });
                }
            } else {
                if (board.flags.BlackCastleQ && emptyCastleLine(board, isWhite, true)) {
                    moves.push_back(Move{
                        .from = sq,
                        .to = 7*8+1,
                        .promotion = Promotion::None,
                        .flag = Flag::Castle
                    });
                }
                if (board.flags.BlackCastleK && emptyCastleLine(board, isWhite, false)) {
                    moves.push_back(Move{
                        .from = sq,
                        .to = 7*8+6,
                        .promotion = Promotion::None,
                        .flag = Flag::Castle
                    });
                }
            }
        }
        // TODO: Legality checks
    }
    return moves;
}