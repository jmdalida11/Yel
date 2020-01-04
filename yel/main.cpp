#include "board.hpp"

Move parseMove(std::string move, board::Game game)
{
	int fromFile = move[0] - 97;
	int fromRank = move[1] - 49;
	int toFile = move[2] - 97;
	int toRank = move[3] - 49;

	Sqr from = defs::fRSqr(fromFile, fromRank);
	Sqr to = defs::fRSqr(toFile, toRank);

	for (const auto& move : game.getBoard().moves)
	{
		if (from == FROM(move) && to == TO(move) && PIECE(move) == game.getBoard().position[from])
		{
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