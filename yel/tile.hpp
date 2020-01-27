#pragma once
#include "piece.hpp"

class Tile
{
public:
    Tile(int x, int y, int size, int f, int r, int s, bool isW, SDL_Renderer *ren);
    ~Tile();
    void update();
    void render();
    void initTexture(SDL_Surface* tileSurface[]);
    GuiPiece* getPiece();
    void setPiece(GuiPiece* p);
    SDL_Rect getPosition();
    int getSqr();
    void destroyPiece();
private:
    SDL_Rect position {};
    SDL_Texture* texture = NULL;
    SDL_Renderer *renderer = NULL;
    bool isWhite;
    int file;
    int rank;
    int sqr;
    GuiPiece* piece = NULL;
};