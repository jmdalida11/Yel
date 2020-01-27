#pragma once

#include "board.hpp"
#include <map>
#include <sstream>

using namespace defs;

namespace utils {

Move parseMove(std::string moveString, board::Game& game)
{
	int fromFile = moveString[0] - 97;
	int fromRank = moveString[1] - 49;
	int toFile = moveString[2] - 97;
	int toRank = moveString[3] - 49;

	Sqr from = fRSqr(fromFile, fromRank);
	Sqr to = fRSqr(toFile, toRank);

	for (const auto& moveV : game.getBoard().moves)
	{
		Move move = moveV.m;
		if (moveString.length() == 5)
		{
			if (from == FROM(move) && to == TO(move) && PIECE(move) == game.getBoard().position[from])
			{
				std::string piecesCharPromotion = "nbrq";
				for (int i=0; i<piecesCharPromotion.length(); i++)
				{
					if (moveString[4] == piecesCharPromotion[i])
					{
						Piece promotedPiece = promoteChar(moveString[4], game.getBoard().side);

						if (promotedPiece == PROMOTE(move))
						{
							return move;
						}
					}
				}
			}
		}
		else if (from == FROM(move) && to == TO(move) && PIECE(move) == game.getBoard().position[from])
		{
			if (PROMOTE(move)) break;
			return move;
		}
	}

	return 0;
}

Move parseMove(Move m, board::Game& game)
{
	for (const auto& moveV : game.getBoard().moves)
	{
		Move move = moveV.m;
		if (PROMOTE(m) == PROMOTE(move) && FROM(m) == FROM(move) && TO(m) == TO(move) && PIECE(move) == game.getBoard().position[FROM(m)])
		{
			return move;
		}
	}

	return 0;
}

void loadFen(std::string fen, board::Game& game)
{
	std::map<char, Piece> pieceMap =
	{
		{'r', bR}, {'n', bN}, {'b', bB}, {'q', bQ}, {'k', bK}, {'p', bP},
		{'R', wR}, {'N', wN}, {'B', wB}, {'Q', wQ}, {'K', wK}, {'P', wP}
	};

	int rank = 7;
	int file = 0;
	int i = 0;

	while (fen[i] != ' ')
	{
		if (fen[i] == '/')
		{
			--rank;
			file = 0;
			i++;
			continue;
		}

		if (std::isdigit(fen[i]))
		{
			file += fen[i] - 48;
		}
		else
		{
			game.getBoard().position[fRSqr(file, rank)] = pieceMap[fen[i]];
			game.getBoard().pieces[pieceMap[fen[i]]].push_back(fRSqr(file, rank));
			file++;
		}

		++i;
	}

	game.getBoard().side = fen[++i] == 'w' ? WHITE : BLACK;

	i++;
	while(fen[++i] != ' ')
	{
		if (fen[i] == '-')
		{
			i++;
			break;
		}

		if (fen[i] == 'K')
			game.getBoard().castle |= 8;
		else if (fen[i] == 'Q')
			game.getBoard().castle |= 4;
		else if (fen[i] == 'k')
			game.getBoard().castle |= 2;
		else if (fen[i] == 'q')
			game.getBoard().castle |= 1;
	}

	if (fen[++i] != '-')
	{
		int file = fen[i] - 97;
		int rank = fen[++i] - 49;

		game.getBoard().enPassant = fRSqr(file, rank);
		game.startEnpassant = fRSqr(file, rank);
	}

	i += 2;

	std::string fiftyMove = "";
	while(fen[i] != ' ')
	{
		fiftyMove += fen[i];
		i++;
	}
	std::stringstream ss(fiftyMove);

	ss >> game.getBoard().fiftyMove;

	i++;

	std::string hply = "";
	while(i<fen.length())
	{
		hply += fen[i];
		i++;
	}

	std::stringstream ss1(hply);
	ss1 >> game.getBoard().hply;
}

} // namespace utils