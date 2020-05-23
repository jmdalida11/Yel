#include "piece.hpp"

GuiPiece::GuiPiece(int x, int y, int size, int s, Sqr t, SDL_Renderer *ren)
{
    position.x = x;
    position.y = y;
    position.w = size;
    position.h = size;
    renderer = ren;
    sqr = s;
    type = t;
}

void GuiPiece::initTexture(SDL_Surface* tileSurface)
{
    texture = SDL_CreateTextureFromSurface(renderer, tileSurface);
}

void GuiPiece::render()
{
    SDL_RenderCopy(renderer, texture, NULL, &position);
}

GuiPiece::~GuiPiece()
{
    SDL_DestroyTexture(texture);
}