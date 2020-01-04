#include "board.hpp"

Move parseMove(std::string moveString, board::Game game)
{
	int fromFile = moveString[0] - 97;
	int fromRank = moveString[1] - 49;
	int toFile = moveString[2] - 97;
	int toRank = moveString[3] - 49;

	Sqr from = defs::fRSqr(fromFile, fromRank);
	Sqr to = defs::fRSqr(toFile, toRank);

	for (const auto& move : game.getBoard().moves)
	{
		if (moveString.length() == 5)
		{
			if (from == FROM(move) && to == TO(move) && PIECE(move) == game.getBoard().position[from])
			{
				std::string piecesCharPromotion = "nbrq";
				for (int i=0; i<piecesCharPromotion.length(); i++)
				{
					if (moveString[4] == piecesCharPromotion[i])
					{
						Piece promotedPiece = defs::promoteChar(moveString[4], game.getBoard().side);

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

int main()
{
	print("Starting Yel 1.0 Chess Engine!");
	print("");

	board::Game game;
	game.init();
	game.initPieces();

	while (true)
	{
		print("");
		game.printBoard();
		game.generateMove();

		std::string turn = game.getBoard().side == 0 ? "White" : "Black";
		std::cout << turn << " Enter Move (Ex. e2e4): " << std::endl;

		std::string m;
		std::cin >> m;

		if (m == "quit") break;
		
		const auto move = parseMove(m, game);
		if (move)
		{
			game.makeMove(move);
		}
	}
}