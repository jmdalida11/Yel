#pragma once
#include <SDL2/SDL.h>

class GuiPiece
{
public:
    GuiPiece(int x, int y, int size, int s, int t, SDL_Renderer *ren);
    ~GuiPiece();
    void update();
    void render();
    void initTexture(SDL_Surface* tileSurface);
    SDL_Rect position {};
    SDL_Texture* texture = NULL;
    SDL_Renderer *renderer = NULL;
    int type;
    int sqr;
};