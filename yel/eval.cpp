#include "board.hpp"

using namespace defs;

static const int pieceValue[13]
{
    0, 100, 300, 300, 500, 900, 0, 100, 300, 300, 500, 900, 0
};

static const int flip[64]
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

static const int pawnPlace[64]
{
    0,  0,  0,  0,  0,  0,  0,  0,
    10, 10, -5, -30,-30, 0,  10, 10,
    5,  0,  0,  5,  5,  0,  0,  5,
    0,  0,  10, 20, 20, 10, 0,  0,
    5,  5,  5,  25, 25, 5,  5,  5,
    10, 10, 10, 26, 26, 10, 10, 10,
    20, 20, 20, 30, 30, 20, 20, 20,
    0,  0,  0,  0,  0,  0,  0,  0
};

static const int pawnPlaceEndGame[64]
{
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    5,  5,  5,  5,  5,  5,  5,  5,
    7,  7,  10, 20, 20, 10, 7,  7,
    8,  8,  8,  25, 25, 8,  8,  8,
    10, 10, 10, 26, 26, 10, 10, 10,
    20, 20, 20, 30, 30, 20, 20, 20,
    0,  0,  0,  0,  0,  0,  0,  0
};

static const int knightPlace[64]
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

static const int bishopPlace[64]
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

static const int rookPlace[64]
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

static const int kingPlaceOpening[64]
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

static const int KingPlaceEndgame[64]
{
    -50, -10, 0, 0, 0, 0, -10, -50,
    -10, 0, 10, 10, 10, 10, 0, -10,
    0, 10, 15, 15, 15, 15, 10, 0,
    0, 10, 15, 20, 20, 15, 10, 0,
    0, 10, 15, 20, 20, 15, 10, 0,
    0, 10, 15, 15, 15, 15, 10, 0,
    -10, 0, 10, 10, 10, 10, 0, -10,
    -50, -10, 0, 0, 0, 0, -10, -50
};

enum Stage
{
    OPENING,
    MIDDLEGAME,
    ENDGAME
};

static int stageScore = 0;

static inline Stage stage(board::Game& game)
{
    int wq = game.getBoard().pieces[wQ].size();
    int bq = game.getBoard().pieces[wQ].size();

    if (stageScore <= 1300 || (wq == 0 && bq == 0)) return ENDGAME;
    else if (stageScore <= 2000) return MIDDLEGAME;
    else return OPENING;
}

static void kingPawnCover(int& whiteScore, int& blackScore, board::Game& game)
{
    if (stage(game) == OPENING || stage(game) == MIDDLEGAME)
    {
        for (const auto& pieceSqr : game.getBoard().pieces[wP])
        {
            if (pieceSqr == game.getBoard().pieces[wK][0] + 10) whiteScore += 15;
            if (pieceSqr == game.getBoard().pieces[wK][0] + 9) whiteScore += 10;
            if (pieceSqr == game.getBoard().pieces[wK][0] + 11) whiteScore += 10;
        }

        for (const auto& pieceSqr : game.getBoard().pieces[bP])
        {
            if (pieceSqr == game.getBoard().pieces[bK][0] - 10) blackScore += 15;
            if (pieceSqr == game.getBoard().pieces[bK][0] - 9) blackScore += 10;
            if (pieceSqr == game.getBoard().pieces[bK][0] - 11) blackScore += 10;
        }
}
}

static void countWhiteScore(int& score, board::Game& game)
{
    for (int pieceIndex=wP; pieceIndex<=wK; pieceIndex++)
    {
        score += pieceValue[pieceIndex] * game.getBoard().pieces[pieceIndex].size();

        for (const auto& pieceSqr : game.getBoard().pieces[pieceIndex])
        {
            int piecePos = mailbox[pieceSqr];

            if (pieceIndex == wP)
            {
                if (stage(game) == ENDGAME)
                    score += pawnPlaceEndGame[piecePos];
                else
                    score += pawnPlace[piecePos];
            }
            else if (pieceIndex == wN)
            {
                score += knightPlace[piecePos];
            }
            else if (pieceIndex == wB)
            {
                score += bishopPlace[piecePos];
            }
            else if (pieceIndex == wR)
            {
                score += rookPlace[piecePos];
            }
            else if (pieceIndex == wK)
            {
                if (stage(game) == ENDGAME)
                    score += KingPlaceEndgame[piecePos];
                else if (stage(game) == OPENING)
                    score += kingPlaceOpening[piecePos];
            }
        }
    }
}

static void countBlackScore(int& score, board::Game& game)
{
    for (int pieceIndex=bP; pieceIndex<=bK; pieceIndex++)
    {
        score += pieceValue[pieceIndex] * game.getBoard().pieces[pieceIndex].size();

        for (const auto& pieceSqr : game.getBoard().pieces[pieceIndex])
        {
            int piecePos = mailbox[pieceSqr];

            if (pieceIndex == bP)
            {
                if (stage(game) == ENDGAME)
                    score += pawnPlaceEndGame[flip[piecePos]];
                else
                    score += pawnPlace[flip[piecePos]];
            }
            else if (pieceIndex == bN)
            {
                score += knightPlace[flip[piecePos]];
            }
            else if (pieceIndex == bB)
            {
                score += bishopPlace[flip[piecePos]];
            }
            else if (pieceIndex == bR)
            {
                score += rookPlace[flip[piecePos]];
            }
            else if (pieceIndex == bK)
            {
                if (stage(game) == ENDGAME)
                    score += KingPlaceEndgame[flip[piecePos]];
                else if (stage(game) == OPENING)
                    score += kingPlaceOpening[flip[piecePos]];
            }
        }
    }
}

void countAllPiecesScore(int& score, board::Game& game)
{
    for (int pieceIndex=wP; pieceIndex<=bK; pieceIndex++)
    {
        for (const auto& pieceSqr : game.getBoard().pieces[pieceIndex])
        {
            score += pieceValue[pieceIndex] * game.getBoard().pieces[pieceIndex].size();
        }
    }
}

int evaluation(board::Game& game)
{
    int whiteScore = 0;
    int blackScore = 0;
    stageScore = 0;

    countAllPiecesScore(stageScore, game);
    countWhiteScore(whiteScore, game);
    countBlackScore(blackScore, game);

    kingPawnCover(whiteScore, blackScore, game);

    if (game.getBoard().side == BLACK)
        return blackScore - whiteScore;

    return whiteScore - blackScore;
}