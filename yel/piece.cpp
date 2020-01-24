#include "piece.hpp"

Piece::Piece(int x, int y, int size, int s, int t, SDL_Renderer *ren)
{
    position.x = x;
    position.y = y;
    position.w = size;
    position.h = size;
    renderer = ren;
    sqr = s;
    type = t;
}

void Piece::initTexture(SDL_Surface* tileSurface)
{
    texture = SDL_CreateTextureFromSurface(renderer, tileSurface);
}

void Piece::update()
{
}

void Piece::render()
{
    SDL_RenderCopy(renderer, texture, NULL, &position);
}

Piece::~Piece()
{
    SDL_DestroyTexture(texture);
}