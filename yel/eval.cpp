#include "board.hpp"

using namespace defs;

#define DOUBLED_PAWN_PENALTY        10
#define ISOLATED_PAWN_PENALTY       20
#define BACKWARDS_PAWN_PENALTY      8
#define PASSED_PAWN_BONUS           20
#define ROOK_SEMI_OPEN_FILE_BONUS   10
#define ROOK_OPEN_FILE_BONUS        15
#define ROOK_ON_SEVENTH_BONUS       20
#define IS_ENDGAME_VAL              2300

const int pieceValue[13]
{
    0, 100, 300, 300, 500, 900, 1000, 100, 300, 300, 500, 900, 1000
};

const int flip[64]
{
    56,  57,  58,  59,  60,  61,  62,  63,
    48,  49,  50,  51,  52,  53,  54,  55,
    40,  41,  42,  43,  44,  45,  46,  47,
    32,  33,  34,  35,  36,  37,  38,  39,
    24,  25,  26,  27,  28,  29,  30,  31,
    16,  17,  18,  19,  20,  21,  22,  23,
    8,   9,  10,  11,  12,  13,  14,  15,
    0,   1,   2,   3,   4,   5,   6,   7
};

const int pawnPlace[64]
{
    0,  0,  0,  0,  0,  0,  0,  0,
    10, 10, 0, -30,-30,0,  10, 10,
    5,  0,  0,  5,  5,  0,  0,  5,
    0,  0,  10, 20, 20, 10, 0,  0,
    5,  5,  5,  25, 25, 5,  5,  5,
    10, 10, 10, 26, 26, 10, 10, 10,
    20, 20, 20, 30, 30, 20, 20, 20,
    0,  0,  0,  0,  0,  0,  0,  0
};

const int knightPlace[64]
{
    0,  -10,0,  0,  0,  0,  -10,0,
    0,  0,  0,  5,  5,  0,  0,  0,
    0,  0,  10, 10, 10, 10, 0,  0,
    0,  0,  10, 20, 20, 10, 5,  0,
    5,  10, 15, 20, 20, 15, 10, 5,
    5,  10, 10, 20, 20, 10, 10, 5,
    0,  0,  5,  10, 10, 5,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0
};

const int bishopPlace[64]
{
    0,  0,  -10,0,  0, -10, 0,  0,
    0,  0,  0,  10, 10, 0,  0,  0,
    0,  0,  10, 15, 15, 10, 0,  0,
    0,  10, 15, 20, 20, 15, 10, 0,
    0,  10, 15, 20, 20, 15, 10, 0,
    0,  0,  10, 15, 15, 10, 0,  0,
    0,  0,  0,  10, 10, 0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0
};

const int rookPlace[64]
{
    0,  0,  5,  10, 10, 5,  0,  0,
    0,  0,  5,  10, 10, 5,  0,  0,
    0,  0,  5,  10, 10, 5,  0,  0,
    0,  0,  5,  10, 10, 5,  0,  0,
    0,  0,  5,  10, 10, 5,  0,  0,
    0,  0,  5,  10, 10, 5,  0,  0,
    25, 25, 25, 25, 25, 25, 25, 25,
    0,  0,  5,  10, 10, 5,  0,  0
};

const int kingPlaceOpening[64]
{
    0,  45,  40,  -5,   0,   -5,  40,  20,
    -5, -5,  -5,  -5,  -5,  -5,  -5,   -5,
    -5, -5,  -5,  -5,  -5,  -5,  -5,   -5,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
};

const int KingPlaceEndgame[64]
{
    -50,-10,0,  0,  0,  0,  -10,-50,
    -10,0,  10, 10, 10, 10, 0,  -10,
    0,  10, 15, 15, 15, 15, 10, 0,
    0,  10, 15, 20, 20, 15, 10, 0,
    0,  10, 15, 20, 20, 15, 10, 0,
    0,  10, 15, 15, 15, 15, 10, 0,
    -10,0,  10, 10, 10, 10, 0,  -10,
    -50,-10,0,  0,  0,  0,  -10,-50
};

int evaluation(board::Game& game)
{
    int whiteScore = 0;
    int blackScore = 0;

    for (int pieceIndex=wP; pieceIndex<=wK; pieceIndex++)
    {
        whiteScore += pieceValue[pieceIndex] * game.getBoard().pieces[pieceIndex].size();

        for (const auto& pieceSqr : game.getBoard().pieces[pieceIndex])
        {
            int piecePos = mailbox[pieceSqr];

            if (pieceIndex == wP)
            {
                whiteScore += pawnPlace[piecePos];
            }
            else if (pieceIndex == wN)
            {
                whiteScore += knightPlace[piecePos];
            }
            else if (pieceIndex == wB)
            {
                whiteScore += bishopPlace[piecePos];
            }
            else if (pieceIndex == wR)
            {
                whiteScore += rookPlace[piecePos];
            }
            else if (pieceIndex == wK)
            {
                if (whiteScore <= IS_ENDGAME_VAL)
                    whiteScore += KingPlaceEndgame[piecePos];
                else
                    whiteScore += kingPlaceOpening[piecePos];
            }
        }
    }

    for (int pieceIndex=bP; pieceIndex<=bK; pieceIndex++)
    {
        blackScore += pieceValue[pieceIndex] * game.getBoard().pieces[pieceIndex].size();

        for (const auto& pieceSqr : game.getBoard().pieces[pieceIndex])
        {
            int piecePos = mailbox[pieceSqr];

            if (pieceIndex == bP)
            {
                blackScore += pawnPlace[flip[piecePos]];
            }
            else if (pieceIndex == bN)
            {
                blackScore += knightPlace[flip[piecePos]];
            }
            else if (pieceIndex == bB)
            {
                blackScore += bishopPlace[flip[piecePos]];
            }
            else if (pieceIndex == bR)
            {
                blackScore += rookPlace[flip[piecePos]];
            }
            else if (pieceIndex == bK)
            {
                if (blackScore <= IS_ENDGAME_VAL)
                    blackScore += KingPlaceEndgame[flip[piecePos]];
                else
                    blackScore += kingPlaceOpening[flip[piecePos]];
            }
        }
    }

    if (game.getBoard().side == BLACK)
        return blackScore - whiteScore;

    return whiteScore - blackScore;
}