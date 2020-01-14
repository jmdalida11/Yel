#include "board.hpp"

using namespace defs;

int pieceValue[13]
{
    0, 100, 300, 300, 500, 900, 1000, 100, 300, 300, 500, 900, 1000
};

int flip[64] = {
    56,  57,  58,  59,  60,  61,  62,  63,
    48,  49,  50,  51,  52,  53,  54,  55,
    40,  41,  42,  43,  44,  45,  46,  47,
    32,  33,  34,  35,  36,  37,  38,  39,
    24,  25,  26,  27,  28,  29,  30,  31,
    16,  17,  18,  19,  20,  21,  22,  23,
    8,   9,  10,  11,  12,  13,  14,  15,
    0,   1,   2,   3,   4,   5,   6,   7
};

int evaluation(board::Game& game)
{
    int whiteScore = 0;
    int blackScore = 0;

    for (int pieceIndex=wP; pieceIndex<=wK; pieceIndex++)
    {
        whiteScore += pieceValue[pieceIndex] * game.getBoard().pieces[pieceIndex].size();
    }

    for (int pieceIndex=bP; pieceIndex<=bK; pieceIndex++)
    {
        blackScore += pieceValue[pieceIndex] * game.getBoard().pieces[pieceIndex].size();
    }

    if (game.getBoard().side == BLACK)
        return blackScore - whiteScore;

    return whiteScore - blackScore;
}