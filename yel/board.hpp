#pragma once

#include "defs.hpp"
#include <vector>

namespace board {

constexpr Sqr NUM_OF_SQR = 120;
constexpr Sqr NUM_OF_PIECE = 13;

struct Board
{
	Sqr position[NUM_OF_SQR];
	std::vector<Sqr> pieces[NUM_OF_PIECE];
	uint8_t side {};
	uint8_t fiftyMove {0};
	uint8_t enPassant {};
	uint8_t castle {15};
	int ply {0};
	std::vector<Move> moves {};
};

class Game
{
public:
	Game() = default;
	void init();
	void initPieces();
	void printBoard();
	void generateMove();
	void genPawnMove(Sqr s);
	bool attack(Sqr s, int side);
	void genPieceMove(Sqr from, Sqr to, bool isCapture);
	bool makeMove(Move move);
	Board& getBoard() { return board; }
private:
	Board board;
};

}