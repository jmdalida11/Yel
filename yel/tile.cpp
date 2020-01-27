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
    if (piece != NULL)
    {
        piece->update();
    }
}

bool Tile::isCollide(const SDL_Rect& mouse)
{
    return !(
        ( ( mouse.y + mouse.h ) < ( position.y ) ) ||
        ( mouse.y > ( position.y + position.h ) ) ||
        ( ( mouse.x + mouse.w ) < position.x ) ||
        ( mouse.x > ( position.x + position.w ) )
    );
}

void Tile::alignPiece()
{
    piece->position.x = position.x;
    piece->position.y = position.y;
}

SDL_Rect Tile::getPosition()
{
    return position;
}

GuiPiece* Tile::getPiece()
{
    return piece;
}

void Tile::setPiece(GuiPiece* p)
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

void Tile::destroyPiece()
{
    if (piece != NULL)
    {
        delete piece;
        piece = NULL;
    }
}

Tile::~Tile()
{
    SDL_DestroyTexture(texture);
}