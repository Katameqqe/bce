#pragma once
#include <cstdint>

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

struct CastleFlags {
    bool WhiteCastleQ = true;
    bool WhiteCastleK = true;
    bool BlackCastleQ = true;
    bool BlackCastleK = true;
};

struct Move {
    uint8_t from;
    uint8_t to;
    Promotion promotion;
    Flag flag;
};

struct MoveEntry {
    Move move;
    char piece;
    uint8_t enPassant;
    CastleFlags flags;
};