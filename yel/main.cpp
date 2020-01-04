#include "utils.hpp"

int main()
{
	print("Starting Yel 1.0 Chess Engine!");
	print("");

	const std::string Startfen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

	board::Game game;
	game.init();
	utils::loadFen(Startfen, game);

	while (true)
	{
		print("");
		game.printBoard();
		game.generateMove();

		std::string turn = game.getBoard().side == defs::WHITE ? "White" : "Black";
		std::cout << turn << " Enter Move (Ex. e2e4): " << std::endl;

		std::string m;
		std::cin >> m;

		if (m == "quit") break;
		
		const auto move = utils::parseMove(m, game);
		if (move)
		{
			game.makeMove(move);
		}
	}
}