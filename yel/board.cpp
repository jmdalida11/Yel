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

bool Game::attacked(Sqr sqrAttacked, int side)
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
			if (isPawn[pieceIndex])
			{
				if (side == WHITE)
				{
					if (board.position[sqr + 9] == sqrAttacked || board.position[sqr + 11] == sqrAttacked)
					{
						return true;
					}
				}
				else if (side == BLACK)
				{
					if (board.position[sqr + -9] == sqrAttacked || board.position[sqr + -11] == sqrAttacked)
					{
						return true;
					}
				}
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
							if (sqrAttacked == s)
							{
								return true;
							}
							break;
						}

						if (sqrAttacked == s)
						{
							return true;
						}

						if (!sliding[pieceIndex])
							break;
					}
				}
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
		genCastlingMove(pieceIndex);

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

void Game::genCastlingMove(Piece pieceIndex)
{
	if (isKing[pieceIndex])
	{
		if (WKSC(board.castle) && board.side == WHITE)
		{
			if (board.position[f1] == EMPTY && board.position[g1] == EMPTY)
			{
				Move move = 0;
				moveFromTo(move, e1, g1);
				addPieceBits(move, wK);
				addCastlingBits(move, 8);
				board.moves.push_back(move);
			}
		}
		if (WQSC(board.castle) && board.side == WHITE)
		{
			if (board.position[d1] == EMPTY && board.position[c1] == EMPTY)
			{
				Move move = 0;
				moveFromTo(move, e1, c1);
				addPieceBits(move, wK);
				addCastlingBits(move, 4);
				board.moves.push_back(move);
			}
		}
		if (BQSC(board.castle) && board.side == BLACK)
		{
			if (board.position[f8] == EMPTY && board.position[g8] == EMPTY)
			{
				Move move = 0;
				moveFromTo(move, e8, g8);
				addPieceBits(move, bK);
				addCastlingBits(move, 2);
				board.moves.push_back(move);
			}
		}
		if (BQSC(board.castle) && board.side == BLACK)
		{
			if (board.position[d8] == EMPTY && board.position[c8] == EMPTY)
			{
				Move move = 0;
				moveFromTo(move, e8, c8);
				addPieceBits(move, bK);
				addCastlingBits(move, 1);
				board.moves.push_back(move);
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
				addPromoteBits(move, piece);
				moveFromTo(move, sqr, (sqr + moves[0]));
				addPieceBits(move, pieces[0]);
				board.moves.push_back(move);
			}
		}

		if (board.position[sqr + moves[2]] != EMPTY && board.position[sqr + moves[2]] != OFF_BOARD &&
			pieceColor[board.position[sqr + moves[2]]] == (board.side ^ 1))
		{
			for (Piece piece=pieces[1]; piece<=pieces[2]; ++piece)
			{
				Move move = 0;
				addPromoteBits(move, piece);
				moveFromTo(move, sqr, (sqr + moves[2]));
				addPieceBits(move, pieces[0]);
				addCaptureBit(move);
				setCapturePieceBits(move, board.position[(sqr + moves[2])]);
				board.moves.push_back(move);
			}
		}

		if (board.position[sqr + moves[3]] != EMPTY && board.position[sqr + moves[3]] != OFF_BOARD &&
			pieceColor[board.position[sqr + moves[3]]] == (board.side ^ 1))
		{
			for (Piece piece=pieces[1]; piece<=pieces[2]; ++piece)
			{
				Move move = 0;
				addPromoteBits(move, piece);
				moveFromTo(move, sqr, (sqr + moves[3]));
				addPieceBits(move, pieces[0]);
				addCaptureBit(move);
				setCapturePieceBits(move, board.position[(sqr + moves[3])]);
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

		if (board.position[sqr + moves[2]] != EMPTY && board.position[sqr + moves[2]] != OFF_BOARD &&
			pieceColor[board.position[sqr + moves[2]]] == (board.side ^ 1))
		{
			Move move = 0;
			moveFromTo(move, sqr, (sqr + moves[2]));
			addPieceBits(move, pieces[0]);
			addCaptureBit(move);
			setCapturePieceBits(move, board.position[(sqr + moves[2])]);
			board.moves.push_back(move);
		}

		if (board.position[sqr + moves[3]] != EMPTY && board.position[sqr + moves[3]] != OFF_BOARD &&
			pieceColor[board.position[sqr + moves[3]]] == (board.side ^ 1))
		{
			Move move = 0;
			moveFromTo(move, sqr, (sqr + moves[3]));
			addPieceBits(move, pieces[0]);
			addCaptureBit(move);
			setCapturePieceBits(move, board.position[(sqr + moves[3])]);
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
				setCapturePieceBits(move, board.enPassant - moves[0]);
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
	{
		addCaptureBit(move);
		setCapturePieceBits(move, board.position[to]);
	}

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
			toRemoveSqrIndex = ENPASSCAP(move);
		}

		auto& pieceToRemove = board.pieces[board.position[toRemoveSqrIndex]];
		for (auto itr=pieceToRemove.begin(); itr!=pieceToRemove.end(); ++itr)
		{
			if (*itr == toRemoveSqrIndex)
			{
				if (ENPASSCAP(move))
				{
					board.position[ENPASSCAP(move)] = EMPTY;
				}
				pieceToRemove.erase(itr);
				break;
			}
		}
	}

	if (PROMOTE(move))
	{
		board.position[to] = PROMOTE(move);
		board.position[from] = EMPTY;

		board.pieces[PROMOTE(move)].push_back(to);

		for (auto itr=board.pieces[PIECE(move)].begin(); itr!=board.pieces[PIECE(move)].end(); ++itr)
		{
			if (*itr == from)
			{
				board.pieces[PIECE(move)].erase(itr);
			}
		}
	}

	if (CASTBITS(move))
	{
		if (board.side == WHITE)
		{
			uint8_t dir = CASTBITS(move);

			if (WKSC(dir))
			{
				board.position[to] = wK;
				board.position[from] = EMPTY;
				board.position[f1] = wR;
				board.position[h1] = EMPTY;

				board.pieces[wK][0] = to;
				for (auto& pieceMovingSqr : board.pieces[wR])
				{
					if (pieceMovingSqr == h1)
					{
						pieceMovingSqr = f1;
						break;
					}
				}
			}
			else if (WQSC(dir))
			{
				board.position[to] = wK;
				board.position[from] = EMPTY;
				board.position[d1] = wR;
				board.position[a1] = EMPTY;

				board.pieces[wK][0] = to;

				for (auto& pieceMovingSqr : board.pieces[wR])
				{
					if (pieceMovingSqr == a1)
					{
						pieceMovingSqr = d1;
						break;
					}
				}
			}

			board.castle &= 3;
		}
		else if (board.side == BLACK)
		{
			uint8_t dir = CASTBITS(move);

			if (BKSC(dir))
			{
				board.position[to] = bK;
				board.position[from] = EMPTY;
				board.position[f8] = bR;
				board.position[h8] = EMPTY;

				board.pieces[bK][0] = to;

				for (auto& pieceMovingSqr : board.pieces[bR])
				{
					if (pieceMovingSqr == h8)
					{
						pieceMovingSqr = f8;
						break;
					}
				}
			}
			else if (BQSC(dir))
			{
				board.position[to] = bK;
				board.position[from] = EMPTY;
				board.position[d8] = bR;
				board.position[a8] = EMPTY;

				board.pieces[bK][0] = to;

				for (auto& pieceMovingSqr : board.pieces[bR])
				{
					if (pieceMovingSqr == a8)
					{
						pieceMovingSqr = d8;
						break;
					}
				}
			}

			board.castle &= 12;
		}
	}
	else
	{
		if (board.castle != 0 && isKing[PIECE(move)])
		{
			if (board.side == WHITE)
			{
				board.castle &= 3;
			}
			else if (board.side == BLACK)
			{
				board.castle &= 12;
			}
		}
		else if (board.castle != 0 && isRook[PIECE(move)])
		{
			if (board.side == WHITE)
			{
				if (from == h1)
					board.castle &= 7;
				if (from == a1)
					board.castle &= 11;
			}
			else if (board.side == BLACK)
			{
				if (from == h8)
					board.castle &= 13;
				if (from == a8)
					board.castle &= 14;
			}
		}

		board.position[to] = board.position[from];
		board.position[from] = EMPTY;

		for (auto& pieceMovingSqr : board.pieces[PIECE(move)])
		{
			if (pieceMovingSqr == from)
			{
				pieceMovingSqr = to;
				break;
			}
		}
	}

	Sqr kingPos = board.side == WHITE ? board.pieces[wK][0] : board.pieces[bK][0];

	board.enPassant = ENPASS(move);
	board.side ^= 1;

	board.moveHistory.push_back(move);

	if (attacked(kingPos, board.side ^ 1))
	{
		// TODO: Ilegal move. take back move
		return false;
	}

	board.moves.clear();

	return true;
}

void Game::takeback()
{

}

} // namespace board