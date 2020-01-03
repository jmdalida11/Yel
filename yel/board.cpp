#include "board.hpp"

using namespace defs;

namespace board {

void Game::init()
{
	for(Sqr i=0; i<NUM_OF_SQR; ++i)
	{
		board.position[i] = oB;
	}

	int sqrIndex = 0;
	for(Sqr file=FILE_A; file<=FILE_H; ++file)
	{
		for(Sqr rank=RANK_1; rank<=RANK_8; ++rank)
		{
			auto sqr = fRSqr(file, rank);
			board.position[sqr] = EMPTY;
		}
	}
}

void Game::initPieces()
{
	for(Sqr i=a2; i<=h2; ++i)
	{
		board.pieces[wP].push_back(i);
	}

	for(Sqr i=a7; i<=h7; ++i)
	{
		board.pieces[bP].push_back(i);
	}

	board.pieces[wN].push_back(b1);
	board.pieces[wN].push_back(g1);
	board.pieces[wB].push_back(c1);
	board.pieces[wB].push_back(f1);
	board.pieces[wR].push_back(a1);
	board.pieces[wR].push_back(h1);
	board.pieces[wQ].push_back(d1);
	board.pieces[wK].push_back(e1);

	board.pieces[bN].push_back(b8);
	board.pieces[bN].push_back(g8);
	board.pieces[bB].push_back(c8);
	board.pieces[bB].push_back(f8);
	board.pieces[bR].push_back(a8);
	board.pieces[bR].push_back(h8);
	board.pieces[bQ].push_back(d8);
	board.pieces[bK].push_back(e8);

	for(Sqr i=wP; i<=bK; ++i)
	{
		for(auto itr=board.pieces[i].begin(); itr!=board.pieces[i].end(); ++itr)
		{
			board.position[*itr] = i;
		}
	}

	board.side = WHITE;
}

void Game::printBoard()
{
	for(Sqr rank=RANK_8; rank>=RANK_1; --rank)
	{
		std::cout << rank + 1 << " ";
		for(Sqr file=FILE_A; file<=FILE_H; ++file)
		{
			Piece p = board.position[fRSqr(file, rank)];
			std::cout << " " << getPieceChar(p) << " ";
		}
		std::cout << std::endl;
	}

	std::cout << "  ";
	for(Sqr file=FILE_A; file<=FILE_H; ++file)
	{
		char x = (char) (97 + file);
		std::cout << "  " << x << " ";
	}
	print("");

	//printMoves(board.moves);
}

void Game::generateMove()
{
	Piece start;
	Piece end;

	if (board.side == WHITE)
	{
		start = wP;
		end = wK;
	}
	else if (board.side == BLACK)
	{
		start = bP;
		end = bK;
	}

	for (auto pieceIndex=start; pieceIndex<=end; ++pieceIndex)
	{
		std::vector<Sqr> pieceSqr = board.pieces[pieceIndex];

		for (const auto& sqr : pieceSqr)
		{
			if (isPawn[pieceIndex])
			{
				genPawnMove(sqr);
			}
			else
			{
				const std::vector<Sqr>& pMoves = pieceMoves(pieceIndex);

				for (const auto& pM : pMoves)
				{
					Sqr s = sqr;
					
					while (true)
					{
						s = s + pM;

						if (mailbox[s] == OFF_BOARD)
							break;

						auto bSqr = board.position[s];
						if (bSqr != EMPTY)
						{
							if (pieceColor[bSqr] != board.side)
							{
								genPieceMove(sqr, s, true);
							}
							break;
						}

						genPieceMove(sqr, s, false);

						if (!sliding[pieceIndex])
							break;
					}
					
				}

			}
		}
	}
}

bool Game::attack(Sqr s, int side)
{
	Piece start;
	Piece end;

	if (side == WHITE)
	{
		start = wP;
		end = wK;
	}
	else if (side == BLACK)
	{
		start = bP;
		end = bK;
	}

	for (auto pieceIndex=start; pieceIndex<=end; ++pieceIndex)
	{
		std::vector<Sqr> pieceSqr = board.pieces[pieceIndex];

		for (const auto& sqr : pieceSqr)
		{
			if (sqr)
			{
				return true;
			}
		}
	}

	return false;
}

void Game::genPawnMove(Sqr sqr)
{
	if (board.side == WHITE)
	{
		if (sqr >= a2 && sqr <= h2 && board.position[sqr + 10] == EMPTY && board.position[sqr + 20] == EMPTY)
		{
			board.moves.push_back(moveFromTo(sqr, (sqr + 20)));
		}

		if (board.position[sqr + 10] == EMPTY && sqr >= a7 && sqr <= h7)
		{
			for (Piece piece=wN; piece<=wQ; ++piece)
			{
				Move move = promote(piece);
				moveFromTo(move, sqr, (sqr + 10));
				board.moves.push_back(move);
			}
		}
		else if (board.position[sqr + 10] == EMPTY)
		{
			board.moves.push_back(moveFromTo(sqr, (sqr + 10)));
		}
	}
	else if (board.side == BLACK)
	{
		if (sqr >= a7 && sqr <= h7 && board.position[sqr + -10] == EMPTY && board.position[sqr + -20] == EMPTY)
		{
			board.moves.push_back(moveFromTo(sqr, (sqr + -20)));
		}

		if (board.position[sqr + -10] == EMPTY && sqr >= a2 && sqr <= h2)
		{
			for (Piece piece=bN; piece<=bQ; ++piece)
			{
				Move move = promote(piece);
				moveFromTo(move, sqr, (sqr + -10));
				board.moves.push_back(move);
			}
		}
		else if (board.position[sqr + -10] == EMPTY)
		{
			board.moves.push_back(moveFromTo(sqr, (sqr + -10)));
		}
	}
}

void Game::genPieceMove(Sqr from, Sqr to, bool isCapture)
{
	Move move = moveFromTo(from, to);

	if (isCapture)
		bitCapture(move);

	board.moves.push_back(move);
}

bool Game::makeMove(Move move)
{
	Sqr from = FROM(move);
	Sqr to = TO(move);

	board.position[to] = board.position[from];
	board.position[from] = EMPTY;

	board.side ^= 1;

	return true;
}

} // namespace board