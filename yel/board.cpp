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
	initHash();
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
					if (sqr + 9 == sqrAttacked || sqr + 11 == sqrAttacked)
					{
						return true;
					}
				}
				else if (side == BLACK)
				{
					if (sqr + -9 == sqrAttacked || sqr + -11 == sqrAttacked)
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
			if (board.position[f1] == EMPTY && board.position[g1] == EMPTY
				&& board.position[e1] == wK && board.position[h1] == wR)
			{
				Move move = 0;
				moveFromTo(move, e1, g1);
				addPieceBits(move, wK);
				addCastlingBits(move, 8);

				uint64_t perms = 15;

				if (board.castle & 8)
					perms &= 7;
				if (board.castle & 4)
					perms &= 11;

				setCastlePerm(move, perms);
				board.moves.push_back(move);
			}
		}
		if (WQSC(board.castle) && board.side == WHITE)
		{
			if (board.position[d1] == EMPTY && board.position[c1] == EMPTY
				&& board.position[e1] == wK && board.position[a1] == wR)
			{
				Move move = 0;
				moveFromTo(move, e1, c1);
				addPieceBits(move, wK);
				addCastlingBits(move, 4);

				uint64_t perms = 15;

				if (board.castle & 8)
					perms &= 7;
				if (board.castle & 4)
					perms &= 11;

				setCastlePerm(move, perms);
				board.moves.push_back(move);
			}
		}
		if (BKSC(board.castle) && board.side == BLACK)
		{
			if (board.position[f8] == EMPTY && board.position[g8] == EMPTY
				&& board.position[e8] == bK && board.position[h8] == bR)
			{
				Move move = 0;
				moveFromTo(move, e8, g8);
				addPieceBits(move, bK);
				addCastlingBits(move, 2);

				uint64_t perms = 15;

				if (board.castle & 2)
					perms &= 13;
				if (board.castle & 1)
					perms &= 14;

				setCastlePerm(move, perms);
				board.moves.push_back(move);
			}
		}
		if (BQSC(board.castle) && board.side == BLACK)
		{
			if (board.position[d8] == EMPTY && board.position[c8] == EMPTY
				&& board.position[e8] == bK && board.position[a8] == bR)
			{
				Move move = 0;
				moveFromTo(move, e8, c8);
				addPieceBits(move, bK);
				addCastlingBits(move, 1);

				uint64_t perms = 15;

				if (board.castle & 2)
					perms &= 13;
				if (board.castle & 1)
					perms &= 14;

				setCastlePerm(move, perms);
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
				setCapturePieceBits(move, board.position[board.enPassant - moves[0]]);
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

	if (board.castle != 0 && isKing[PIECE(move)])
	{
		if (board.side == WHITE)
		{
			uint64_t perms = 15;

			if (board.castle & 8)
				perms &= 7;
			if (board.castle & 4)
				perms &= 11;

			setCastlePerm(move, perms);
		}
		else if (board.side == BLACK)
		{
			uint64_t perms = 15;

			if (board.castle & 2)
					perms &= 13;
			if (board.castle & 1)
				perms &= 14;

			setCastlePerm(move, perms);
		}
	}

	if (board.castle != 0 && isRook[PIECE(move)])
	{
		if (board.side == WHITE)
		{
			if (from == h1)
			{
				uint64_t perms = 15;

				if (board.castle & 8)
					perms &= 7;

				setCastlePerm(move, perms);
			}
			else if (from == a1)
			{
				uint64_t perms = 15;

				if (board.castle & 4)
					perms &= 11;

				setCastlePerm(move, perms);
			}

		}
		else if (board.side == BLACK)
		{
			if (from == h8)
			{
				uint64_t perms = 15;

				if (board.castle & 2)
					perms &= 13;

				setCastlePerm(move, perms);
			}
			else if (from == a8)
			{
				uint64_t perms = 15;

				if (board.castle & 1)
					perms &= 14;

				setCastlePerm(move, perms);
			}
		}
	}

	board.moves.push_back(move);
}

bool Game::makeMove(Move move)
{
	Sqr from = FROM(move);
	Sqr to = TO(move);
	bool illegalCastling = false;

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

	if (CASTBITS(move))
	{
		if (board.side == WHITE)
		{
			uint8_t dir = CASTBITS(move);

			if (WKSC(dir))
			{
				if (attacked(e1, BLACK) || attacked(f1, BLACK))
				{
					illegalCastling = true;
				}

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
				if (attacked(e1, BLACK) || attacked(d1, BLACK))
				{
					illegalCastling = true;
				}

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
		}
		else if (board.side == BLACK)
		{
			uint8_t dir = CASTBITS(move);

			if (BKSC(dir))
			{
				if (attacked(e8, WHITE) || attacked(f8, WHITE))
				{
					illegalCastling = true;
				}

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
				if (attacked(e8, WHITE) || attacked(d8, WHITE))
				{
					illegalCastling = true;
				}

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
		}
	}
	else
	{
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
					break;
				}
			}
		}
		else
		{
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
	}

	if (CASTLEPERM(move))
		board.castle &= CASTLEPERM(move);

	board.fiftyMove++;

	if (isPawn[PIECE(move)] || ISCAP(move))
	{
		board.fiftyMove = 0;
	}

	Sqr kingPos = board.side == WHITE ? board.pieces[wK][0] : board.pieces[bK][0];

	board.enPassant = ENPASS(move);
	board.side ^= 1;

	if (board.side == WHITE)
		board.hply++;

	board.ply++;
	board.moveHistory.push_back(move);

	setPositionKey();

	if (attacked(kingPos, board.side) || illegalCastling)
	{
		takeback();
		return false;
	}

	return true;
}

void Game::takeback()
{
	if (board.moveHistory.size() == 0)
	{
		return;
	}

	Move move = board.moveHistory[board.moveHistory.size() - 1];
	board.moveHistory.pop_back();

	Sqr from = FROM(move);
	Sqr to = TO(move);

	if (CASTBITS(move))
	{
		uint8_t dir = CASTBITS(move);

		if (WKSC(dir))
		{
			board.position[g1] = EMPTY;
			board.position[e1] = wK;
			board.position[f1] = EMPTY;
			board.position[h1] = wR;

			board.pieces[wK][0] = e1;
			for (auto& pieceMovingSqr : board.pieces[wR])
			{
				if (pieceMovingSqr == f1)
				{
					pieceMovingSqr = h1;
					break;
				}
			}
		}
		else if(WQSC(dir))
		{
			board.position[c1] = EMPTY;
			board.position[e1] = wK;
			board.position[d1] = EMPTY;
			board.position[a1] = wR;

			board.pieces[wK][0] = e1;

			for (auto& pieceMovingSqr : board.pieces[wR])
			{
				if (pieceMovingSqr == d1)
				{
					pieceMovingSqr = a1;
					break;
				}
			}
		}
		else if(BKSC(dir))
		{
			board.position[g8] = EMPTY;
			board.position[e8] = bK;
			board.position[f8] = EMPTY;
			board.position[h8] = bR;

			board.pieces[bK][0] = e8;

			for (auto& pieceMovingSqr : board.pieces[bR])
			{
				if (pieceMovingSqr == f8)
				{
					pieceMovingSqr = h8;
					break;
				}
			}
		}
		else if(BQSC(dir))
		{
			board.position[c8] = EMPTY;
			board.position[e8] = bK;
			board.position[d8] = EMPTY;
			board.position[a8] = bR;

			board.pieces[bK][0] = e8;

			for (auto& pieceMovingSqr : board.pieces[bR])
			{
				if (pieceMovingSqr == d8)
				{
					pieceMovingSqr = a8;
					break;
				}
			}
		}
	}
	else
	{
		board.position[from] = PIECE(move);
		board.pieces[PIECE(move)].push_back(from);

		auto& pieceSqr = board.pieces[board.position[to]];
		for (auto itr=pieceSqr.begin(); itr!=pieceSqr.end(); itr++)
		{
			if (*itr == to)
			{
				pieceSqr.erase(itr);
				board.position[to] = EMPTY;
				break;
			}
		}
	}

	if (ISCAP(move))
	{
		if (ENPASSCAP(move))
		{
			board.position[ENPASSCAP(move)] = PIECECAP(move);
			board.pieces[PIECECAP(move)].push_back(ENPASSCAP(move));
		}
		else
		{
			board.position[to] = PIECECAP(move);
			board.pieces[PIECECAP(move)].push_back(to);
		}
	}

	if (ISCAP(move) || isPawn[PIECE(move)])
	{
		board.fiftyMove = 0;
	}
	else if (board.fiftyMove > 0)
	{
		board.fiftyMove--;
	}

	if (CASTLEPERM(move))
	{
		int perms = CASTLEPERM(move);
		for (int i = 0; i < 4; i++)
		{
			perms ^= 1 << i;
		}
		board.castle |= perms;
	}

	if (board.moveHistory.size() == 0)
		board.enPassant = startEnpassant;
	else
		board.enPassant = ENPASS(board.moveHistory[board.moveHistory.size() - 1]);

	board.side ^= 1;
	board.ply--;

	board.histHash.pop_back();
	if (board.histHash.size() > 0)
	{
		board.hash = board.histHash[board.histHash.size() - 1];
	}

	if (board.side == BLACK)
		board.hply--;
}

void Game::setPositionKey()
{
	setHash(board.hash, board.position, board.side, board.enPassant);
	board.histHash.push_back(board.hash);
}

} // namespace board