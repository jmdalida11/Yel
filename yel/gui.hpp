#pragma once

#include <stdio.h>
#include <vector>
#include "tile.hpp"
#include "piece.hpp"
#include "board.hpp"
#include "defs.hpp"

namespace gui {

struct PieceMoving
{
    GuiPiece* pieceMoving = NULL;
    Tile* tile = NULL;
    int from = -1;
    int to = -1;
};

class Gui
{
public:
    Gui();
    ~Gui();
    void init();
    void run();
    void update();
    void render();
    void initBoard();
    void initSurface();
    void initPieces();
    bool castleMove(Move move);
private:
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    std::vector<Tile> tiles {};
    SDL_Surface* tileSurface[2];
    SDL_Surface* pieceSurface[12];
    PieceMoving pieceMovingInfo {};
    Sqr AI = defs::BLACK;
    board::Game game {};
};

}