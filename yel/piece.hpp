#pragma once
#include <SDL2/SDL.h>
#include "defs.hpp"

class GuiPiece
{
public:
    GuiPiece(int x, int y, int size, Sqr s, int t, SDL_Renderer *ren);
    ~GuiPiece();
    void render();
    void initTexture(SDL_Surface* tileSurface);
    SDL_Rect position {};
    SDL_Texture* texture = NULL;
    SDL_Renderer *renderer = NULL;
    Sqr type;
    int sqr;
};