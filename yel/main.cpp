#include "utils.hpp"

int main()
{
	print("Starting Yel 1.0 Chess Engine!");
	print("");

	const std::string Startfen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	//const std::string Startfen = "rn1qkbnr/pbpp3p/1p2p3/1P3ppP/3P4/2P3P1/P3PP2/RNBQKBNR b KQkq - 0 1";

	board::Game game;
	game.init();
	utils::loadFen(Startfen, game);
	bool noTakeback = true;

	while (true)
	{
		print("");
		game.printBoard();

		if (noTakeback)
			game.generateMove();

		std::string turn = game.getBoard().side == defs::WHITE ? "White" : "Black";
		std::cout << turn << " Enter Move (Ex. e2e4): " << std::endl;

		std::string m;
		std::cin >> m;

		if (m == "quit") break;
		if (m == "takeback")
		{
			game.takeback();
			continue;
		}

		const auto move = utils::parseMove(m, game);
		if (move)
		{
			noTakeback = game.makeMove(move);
		}
	}
}