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

int pawnPlace[64] = {
    0,   0,   0,   0,   0,   0,   0,   0,
    5,  10,  15,  20,  20,  15,  10,   5,
    4,   8,  12,  16,  16,  12,   8,   4,
    3,   6,   9,  12,  12,   9,   6,   3,
    2,   4,   6,   8,   8,   6,   4,   2,
    1,   2,   3, -10, -10,   3,   2,   1,
    0,   0,   0, -40, -40,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0
};

int evaluation(board::Game& game)
{
    int whiteScore = 0;
    int blackScore = 0;

    for (int pieceIndex=wP; pieceIndex<=wK; pieceIndex++)
    {
        if (pieceIndex == wP)
        {
            for (const auto& pawnSqr : game.getBoard().pieces[pieceIndex])
            {
                int piecePos = mailbox[pawnSqr];
                whiteScore += pawnPlace[piecePos];
            }
        }

        whiteScore += pieceValue[pieceIndex] * game.getBoard().pieces[pieceIndex].size();
    }

    for (int pieceIndex=bP; pieceIndex<=bK; pieceIndex++)
    {
        if (pieceIndex == bP)
        {
            for (const auto& pawnSqr : game.getBoard().pieces[pieceIndex])
            {
                int piecePos = mailbox[pawnSqr];
                blackScore += pawnPlace[flip[piecePos]];
            }
        }

        blackScore += pieceValue[pieceIndex] * game.getBoard().pieces[pieceIndex].size();
    }

    if (game.getBoard().side == BLACK)
        return blackScore - whiteScore;

    return whiteScore - blackScore;
}