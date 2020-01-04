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

	board.castle = 15;
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

	for (Piece pieceIndex=start; pieceIndex<=end; ++pieceIndex)
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
								genPieceMove(sqr, s, pieceIndex, true);
							}
							break;
						}

						genPieceMove(sqr, s, pieceIndex, false);

						if (!sliding[pieceIndex])
							break;
					}	
				}
			}
		}
	}
}

void Game::genPawnMove(Sqr sqr)
{
	std::vector<Sqr> secondRank, seventhRank;
	std::vector<int> moves;
	std::vector<int> pieces;

	if (board.side == WHITE)
	{
		secondRank = {a2, h2};
		seventhRank = {a7, h7};
		moves = {10, 20, 9, 11};
		pieces = {wP, wN, wQ};
	}
	else if (board.side == BLACK)
	{
		secondRank = {a7, h7};
		seventhRank = {a2, h2};
		moves = {-10, -20, -9, -11};
		pieces = {bP, bN, bQ};
	}


	if (sqr >= seventhRank[0] && sqr <= seventhRank[1])
	{
		if (board.position[sqr + moves[0]] == EMPTY)
		{
			for (Piece piece=pieces[1]; piece<=pieces[2]; ++piece)
			{
				Move move = 0;
				promoteBits(move, piece);
				moveFromTo(move, sqr, (sqr + moves[0]));
				addPieceBits(move, pieces[0]);
				board.moves.push_back(move);
			}
		}

		if (board.position[sqr + moves[2]] != EMPTY && pieceColor[board.position[sqr + moves[2]]] == (board.side ^ 1))
		{
			for (Piece piece=pieces[1]; piece<=pieces[2]; ++piece)
			{
				Move move = 0;
				promoteBits(move, piece);
				moveFromTo(move, sqr, (sqr + moves[2]));
				addPieceBits(move, pieces[0]);
				addCaptureBit(move);
				board.moves.push_back(move);
			}
		}

		if (board.position[sqr + moves[3]] != EMPTY && pieceColor[board.position[sqr + moves[3]]] == (board.side ^ 1))
		{
			for (Piece piece=pieces[1]; piece<=pieces[2]; ++piece)
			{
				Move move = 0;
				promoteBits(move, piece);
				moveFromTo(move, sqr, (sqr + moves[1]));
				addPieceBits(move, pieces[0]);
				addCaptureBit(move);
				board.moves.push_back(move);
			}
		}
	}
	else 
	{
		if (sqr >= secondRank[0] && sqr <= secondRank[1])
		{
			if (board.position[sqr + moves[0]] == EMPTY && board.position[sqr + moves[1]] == EMPTY)
			{
				Move move = moveFromTo(sqr, (sqr + moves[1]));
				addPieceBits(move, pieces[0]);
				setEnpassBits(move, (sqr + moves[0]));
				board.moves.push_back(move);
			}
		}

		if (board.position[sqr + moves[0]] == EMPTY)
		{
			Move move = moveFromTo(sqr, (sqr + moves[0]));
			addPieceBits(move, pieces[0]);
			board.moves.push_back(move);
		}

		if (board.position[sqr + moves[2]] != EMPTY && pieceColor[board.position[sqr + moves[2]]] == (board.side ^ 1))
		{
			Move move = 0;
			moveFromTo(move, sqr, (sqr + moves[2]));
			addPieceBits(move, pieces[0]);
			addCaptureBit(move);
			board.moves.push_back(move);
		}

		if (board.position[sqr + moves[3]] != EMPTY && pieceColor[board.position[sqr + moves[3]]] == (board.side ^ 1))
		{
			Move move = 0;
			moveFromTo(move, sqr, (sqr + moves[3]));
			addPieceBits(move, pieces[0]);
			addCaptureBit(move);
			board.moves.push_back(move);
		}

		if (board.enPassant)
		{
			if (sqr + moves[3] == board.enPassant || sqr + moves[2] == board.enPassant)
			{
				Move move = 0;
				moveFromTo(move, sqr, board.enPassant);
				addPieceBits(move, pieces[0]);
				addCaptureBit(move);
				setEnPassCapBits(move, board.enPassant - moves[0]);
				board.moves.push_back(move);
			}
		}
	}
}

void Game::genPieceMove(Sqr from, Sqr to, Piece pieceIndex, bool isCapture)
{
	Move move = moveFromTo(from, to);
	addPieceBits(move, pieceIndex);

	if (isCapture)
		addCaptureBit(move);

	board.moves.push_back(move);
}

bool Game::makeMove(Move move)
{
	Sqr from = FROM(move);
	Sqr to = TO(move);

	if (ISCAP(move))
	{
		Sqr toRemoveSqrIndex = to;

		if (ENPASSCAP(move))
		{
			int x = ENPASSCAP(move);
			//std::cout << x << std::endl;
			toRemoveSqrIndex = ENPASSCAP(move);
		}

		auto& pieceToRemove = board.pieces[board.position[toRemoveSqrIndex]];
		for (auto itr=pieceToRemove.begin(); itr!=pieceToRemove.end(); ++itr)
		{
			if (*itr == toRemoveSqrIndex)
			{
				pieceToRemove.erase(pieceToRemove.begin(), itr);
				break;
			}
		}
	}

	board.position[to] = board.position[from];
	board.position[from] = EMPTY;

	for (auto& p : board.pieces[PIECE(move)])
	{
		if (p == from)
		{
			p = to;
			break;
		}
	}

	board.enPassant = ENPASS(move);
	std::cout << board.enPassant;
	board.side ^= 1;
	board.moves.clear();

	return true;
}

} // namespace board