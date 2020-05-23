#include "board.hpp"
#include <climits>

using namespace defs;

extern int evaluation(board::Game& game);

void pickMove(int moveIndex, std::vector<MoveVal>& moves)
{
    int bestMoveIndex = moveIndex;

    for(int i = moveIndex; i < moves.size(); ++i) {
        if(moves[i].s > moves[bestMoveIndex].s) {
            bestMoveIndex = i;
        }
    }

    MoveVal temp = moves[moveIndex];
    moves[moveIndex] = moves[bestMoveIndex];
    moves[bestMoveIndex] = temp;
}

int quiesce(int alpha, int beta, board::Game& game)
{
    if (game.getBoard().ply >= 12)
        return evaluation(game);

    int score = evaluation(game);

    if (score >= beta)
        return beta;
    if (score > alpha)
        alpha = score;

    game.generateMove(true);

    std::vector<MoveVal> moves = std::move(game.getBoard().moves);

    for (int i = 0; i < moves.size(); ++i)
    {
        pickMove(i, moves);

        if (!game.makeMove(moves[i].m)){
            continue;
        }

        score = -quiesce(-beta, -alpha, game);
        game.takeback();

        if (score > alpha){
            alpha = score;

            game.getBoard().pv[game.getBoard().ply].m = moves[i].m;
            game.getBoard().pv[game.getBoard().ply].s = score;

            if (score >= beta){
                return beta;
            }
        }
    }

    return alpha;
}

int alphaBeta(int alpha, int beta, int depth, board::Game& game)
{
    int score;
    bool noLegalMove = true;

    if (depth == 0)
        return quiesce(alpha, beta, game);

    if (game.getBoard().ply && game.repeat())
        return 0;

    game.generateMove(false);

    std::vector<MoveVal> moves = std::move(game.getBoard().moves);

    for (int i=0; i<moves.size(); i++)
    {
        pickMove(i, moves);

        if (!game.makeMove(moves[i].m)){
            continue;
        }

        noLegalMove = false;
        score = -alphaBeta(-beta, -alpha, depth-1, game);
        game.takeback();

        if (score > alpha){
            alpha = score;

            game.getBoard().pv[game.getBoard().ply].m = moves[i].m;
            game.getBoard().pv[game.getBoard().ply].s = score;

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
    int maxDepth = 4;
    game.getBoard().ply = 0;
    game.clearPv(game);
    alphaBeta(-INT_MAX, INT_MAX, maxDepth, game);
}
