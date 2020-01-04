#pragma once

#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>

#define DEBUG
#ifdef DEBUG
#define print(x) std::cout << x << std::endl;
#else
#define print(x)
#endif
#define OFF_BOARD -1

#define FROM(x) (x & 0x7F)
#define TO(x) ((x >> 7) & 0x7F)
#define ISCAP(x) ((x >> 14) & 1)
#define PIECE(x) ((x >> 25) & 0xF)
#define ISPROMOTION(x) ((x >> 20) & 1)
#define ENPASS(x) ((x >> 29) & 0x7F)
#define ENPASSCAP(x) ((x >> 36) & 0x7F)

using Sqr = int32_t;
using Piece = int8_t;
using Move = uint64_t;

namespace defs {

struct MoveVal
{
	Move m;
	int s;
};

enum : Sqr
{
	WHITE, BLACK, BOTH
};

enum : Sqr
{
	a1 = 21, b1, c1, d1, e1, f1, g1, h1,
	a2 = 31, b2, c2, d2, e2, f2, g2, h2,
	a3 = 41, b3, c3, d3, e3, f3, g3, h3,
	a4 = 51, b4, c4, d4, e4, f4, g4, h4,
	a5 = 61, b5, c5, d5, e5, f5, g5, h5,
	a6 = 71, b6, c6, d6, e6, f6, g6, h6,
	a7 = 81, b7, c7, d7, e7, f7, g7, h7,
	a8 = 91, b8, c8, d8, e8, f8, g8, h8
};

inline std::string fileRankChar()
{
	return "";
}

enum : Sqr
{
	RANK_1 = 0, RANK_2, RANK_3, RANK_4,
	RANK_5, RANK_6, RANK_7, RANK_8, NO_RANK = OFF_BOARD
};

enum : Sqr
{
	FILE_A = 0, FILE_B, FILE_C, FILE_D,
	FILE_E, FILE_F, FILE_G, FILE_H, NO_FILE = OFF_BOARD
};

enum : Sqr 
{
	EMPTY = 0,
	wP, wN, wB, wR, wQ, wK,
	bP, bN, bB, bR, bQ, bK,
	oB = OFF_BOARD
};

const static Sqr pieceColor[]
{
	BOTH, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE,
	BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BOTH
};

const static std::vector<Sqr> pieceMovesArray[]
{
	{19, 21, -19, -21, 8, -8, 12, -12},
	{11, 9, -11, -9},
	{10, -10, 1, -1},
	{11, 9, -11, -9, 10, -10, 1, -1},
	{}
};

inline const std::vector<Sqr>& pieceMoves(const Piece& piece)
{
	switch(piece)
	{
		case wN: return pieceMovesArray[0];
		case wB: return pieceMovesArray[1];
		case wR: return pieceMovesArray[2];
		case wQ: return pieceMovesArray[3];
		case wK: return pieceMovesArray[3];
		case bN: return pieceMovesArray[0];
		case bB: return pieceMovesArray[1];
		case bR: return pieceMovesArray[2];
		case bQ: return pieceMovesArray[3];
		case bK: return pieceMovesArray[3];
	}

	return pieceMovesArray[4];
}

const static bool isPawn[]
{
	false, true, false, false, false, false, false,
	true, false, false, false, false, false, false
};

const static bool sliding[]
{
	false, false, false, true, true, true, false,
	false, false, true, true, true, false, false
};

inline Move moveFromTo(Sqr from, Sqr to)
{
	Move move = 0;
	move |= from;
	move |= (to << 7);
	return move;
}

inline void moveFromTo(Move& move, Sqr from, Sqr to)
{
	move |= from;
	move |= (to << 7);
}

inline void promoteBits(Move& move, Piece p)
{
	move |= 1 << 20;
	move |= (p & 0xF) << 21;
}

inline void addPieceBits(Move& move, Piece p)
{
	move |= (p & 0xF) << 25;
}

inline void addCaptureBit(Move& move)
{
	move |= 1 << 14;
}

inline void addPromotionBit(Move& move)
{
	move |= 1 << 20;
}

inline void setEnpassBits(Move& move, Sqr s)
{
	uint64_t sqr = (s & 0x7F);
	move |= (sqr << 29);
}

inline void setEnPassCapBits(Move& move, Sqr s)
{
	uint64_t sqr = (s & 0x7F);
	move |= sqr << 36;
}

inline std::string getPieceChar(const Piece& piece)
{
	switch(piece)
	{
		case wP: return "wP";
		case wN: return "wN";
		case wB: return "wB";
		case wR: return "wR";
		case wQ: return "wQ";
		case wK: return "wK";
		case bP: return "bP";
		case bN: return "bN";
		case bB: return "bB";
		case bR: return "bR";
		case bQ: return "bQ";
		case bK: return "bK";
		case EMPTY: return "--";
	}
	return "x";
}

inline Sqr fRSqr(Sqr file, Sqr rank)
{
	return (21 + file) + (10 * rank);
}

inline void printMoves(const std::vector<Move>& moves)
{
	print("");
	print("Generated Moves:");

	for(const auto& move : moves)
	{
		Move from = move & 0x7F;
		Move to = (move >> 7) & 0x7F;

		std::cout << from << "-" << to << std::endl;
	}
}

const static Sqr mailbox[120] 
{
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
    -1,  8,  9, 10, 11, 12, 13, 14, 15, -1,
    -1, 16, 17, 18, 19, 20, 21, 22, 23, -1,
    -1, 24, 25, 26, 27, 28, 29, 30, 31, -1,
    -1, 32, 33, 34, 35, 36, 37, 38, 39, -1,
    -1, 40, 41, 42, 43, 44, 45, 46, 47, -1,
    -1, 48, 49, 50, 51, 52, 53, 54, 55, -1,
    -1, 56, 57, 58, 59, 60, 61, 62, 63, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

const static Sqr mailbox64[64] 
{
	21, 22, 23, 24, 25, 26, 27, 28,
	31, 32, 33, 34, 35, 36, 37, 38,
	41, 42, 43, 44, 45, 46, 47, 48,
	51, 52, 53, 54, 55, 56, 57, 58,
	61, 62, 63, 64, 65, 66, 67, 68,
	71, 72, 73, 74, 75, 76, 77, 78,
	81, 82, 83, 84, 85, 86, 87, 88,
	91, 92, 93, 94, 95, 96, 97, 98
};

} // namespace definitions