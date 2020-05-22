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
    void moveAI();
    void clearPieces();
    void handleInput();
    void handleKeyDown(const SDL_Event& e);
    void handleMouseDown(const SDL_Event& e);
    void handleMouseMotion(const SDL_Event& e);
    void movePiece(const SDL_Event& e);
    void handlePromoteMove();
    void newGame();
    void switchSide();

private:
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    std::vector<Tile> tiles {};
    SDL_Surface* tileSurface[3];
    SDL_Surface* pieceSurface[12];
    PieceMoving pieceMovingInfo {};
    Sqr AI = defs::BLACK;
    board::Game game {};
    bool promoting = false;
    SDL_Rect promoteToPieces[4];
    SDL_Surface* promoteSqrSurface = NULL;
    SDL_Texture* promoteTexture = NULL;
    SDL_Texture* lastmoveTexture = NULL;
    int promotePieceIndex = -1;
    SDL_Rect lastmovePosition[2];
    bool lastMoveChecker = false;
    bool aiThinking = false;
    bool running = false;
};

}