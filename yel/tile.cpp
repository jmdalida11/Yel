#include "tile.hpp"

Tile::Tile(int x, int y, int size, int f, int r, int s, bool isW, SDL_Renderer *ren)
{
    position.x = x;
    position.y = y;
    position.w = size;
    position.h = size;
    renderer = ren;
    isWhite = isW;
    file = f;
    rank = r;
    sqr = s;
}

void Tile::initTexture(SDL_Surface* tileSurface[])
{
    if (isWhite)
        texture = SDL_CreateTextureFromSurface(renderer, tileSurface[0]);
    else
        texture = SDL_CreateTextureFromSurface(renderer, tileSurface[1]);
}

void Tile::update()
{

}

SDL_Rect Tile::getPosition()
{
    return position;
}

Piece* Tile::getPiece()
{
    return piece;
}

void Tile::setPiece(Piece* p)
{
    piece = p;
}

int Tile::getSqr()
{
    return sqr;
}

void Tile::render()
{
    SDL_RenderCopy(renderer, texture, NULL, &position);
}

Tile::~Tile()
{
    SDL_DestroyTexture(texture);
}