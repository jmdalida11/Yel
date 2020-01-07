#include "utils.hpp"

int main()
{
	print("Starting Yel 1.0 Chess Engine!");
	print("");

	const std::string Startfen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

	board::Game game;
	game.init();
	utils::loadFen(Startfen, game);
	game.generateMove();

	while (true)
	{
		print("");
		game.printBoard();

		std::string turn = game.getBoard().side == defs::WHITE ? "White" : "Black";
		std::cout << turn << " Enter Move (Ex. e2e4): " << std::endl;

		std::string m;
		std::cin >> m;

		if (m == "quit") break;
		if (m == "takeback")
		{
			game.takeback();
			game.getBoard().moves.clear();
			game.generateMove();
			continue;
		}

		const auto move = utils::parseMove(m, game);
		if (move)
		{
			if(game.makeMove(move))
			{
				game.getBoard().moves.clear();
				game.generateMove();
			}
		}
	}
}