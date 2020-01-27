#pragma once

#include <stdio.h>
#include <vector>
#include "tile.hpp"
#include "piece.hpp"
#include "board.hpp"

namespace gui {

class Gui
{
public:
    Gui(board::Game* g);
    ~Gui();
    void init();
    void run();
    void update();
    void render();
    void initBoard();
    void initSurface();
    void initPieces();
private:
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    std::vector<Tile> tiles {};
    SDL_Surface* tileSurface[2];
    SDL_Surface* pieceSurface[12];
    board::Game* game = NULL;
    GuiPiece* movingPiece = NULL;
};

}