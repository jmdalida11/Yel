#include "board.hpp"
#include <climits>

using namespace defs;

extern int evaluation(board::Game& game);

int quiesce(int alpha,int beta)
{
    return 0;
}

int alphaBeta(int alpha, int beta, int depth, board::Game& game)
{
    int score;
    bool noLegalMove = true;

    if (depth == 0)
        return evaluation(game);

    if (game.repeat() && game.getBoard().ply)
        return 0;

    game.generateMove();

    std::vector<Move> moves = game.getBoard().moves;
    game.getBoard().moves.clear();

    for (const auto move : moves)
    {
        if (!game.makeMove(move)){
            continue;
        }

        noLegalMove = false;
        score = -alphaBeta(-beta, -alpha, depth-1, game);
        game.takeback();

        if (score > alpha){
            game.getBoard().pv[game.getBoard().ply] = move;
            alpha = score;
            if (score >= beta){
                return beta;
            }
        }
    }

    if (noLegalMove) {
        Sqr kingPos = game.getBoard().side == WHITE ? game.getBoard().pieces[wK][0] : game.getBoard().pieces[bK][0];

        if (game.attacked(kingPos, game.getBoard().side ^ 1)){
            return -INT_MAX + game.getBoard().ply;
        }
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
