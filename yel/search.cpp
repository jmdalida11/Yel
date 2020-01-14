#include "board.hpp"
#include <climits>

using namespace defs;

extern int evaluation(board::Game& game);

void copyMoves(board::Game& game, std::vector<Move>& moves)
{
    for (int i=0; i<game.getBoard().moves.size(); i++)
    {
        moves.push_back(game.getBoard().moves[i]);
    }
}

int quiesce(int alpha,int beta)
{
    return 0;
}

int alphaBeta(int alpha, int beta, int depth, board::Game& game)
{
    int score;
    bool noLegalMove = false;

    if (depth == 0)
        return evaluation(game);


    if (game.repeat() && game.getBoard().ply)
        return 0;

    game.getBoard().moves.clear();
    game.generateMove();

    std::vector<Move> moves {};

    copyMoves(game, moves);

    for (const auto move : moves)
    {
        if (!game.makeMove(move)){
            continue;
        }

        noLegalMove = true;
        score = -alphaBeta(-beta, -alpha, depth-1, game);
        game.takeback();

        if (score > alpha){
            if (score >= beta){
                return beta;
            }
            alpha = score;

            game.getBoard().pv[game.getBoard().ply] = move;
        }
    }

    if (noLegalMove) {
        Sqr kingPos = game.getBoard().side == WHITE ? game.getBoard().pieces[wK][0] : game.getBoard().pieces[bK][0];

        if (game.attacked(kingPos, game.getBoard().side ^ 1))
            return -10000 + game.getBoard().ply;
        else
            return 0;
    }

    if (game.getBoard().fiftyMove >= 100)
        return 0;

    return alpha;
}

void search(board::Game& game)
{
    int maxDepth = 6;
    game.getBoard().ply = 0;
    game.clearPv(game);
    alphaBeta(-INT_MAX, INT_MAX, maxDepth, game);
}
