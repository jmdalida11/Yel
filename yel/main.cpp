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
		if (m == "fen")
		{
			print("Please Enter valid fen");
			std::string fen;
			std::string pos = "";
			std::string side = "";
			std::string castPerm = "";
			std::string enPass = "";
			std::string fifty = "";
			std::string fullmove = "";

			std::cin >> pos >> side >> castPerm >> enPass >> fifty >> fullmove;

			fen = pos + " " + side + " " + castPerm + " " + enPass + " " + fifty + " " + fullmove;

			game.getBoard().moves.clear();
			game.init();
			for(int i=wP; i<=bK; i++)
			{
				game.getBoard().pieces[i].clear();
			}
			utils::loadFen(fen, game);
			game.generateMove();

			continue;
		}
		if (m == "moves")
		{
			printMoves(game.getBoard().moves);
			continue;
		}
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