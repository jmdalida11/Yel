#include "gui.hpp"
#include "defs.hpp"

#include <SDL2/SDL_image.h>

namespace gui {

using namespace defs;

constexpr int SCREEN_SIZE = 600;

Gui::Gui(board::Game* g)
{
    if( SDL_Init(SDL_INIT_VIDEO) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    }
    else
    {
        window = SDL_CreateWindow( "Yel Chess Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_SIZE, SCREEN_SIZE, SDL_WINDOW_SHOWN);

        if(window == NULL)
        {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        }
        else
        {
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        }
    }

    game = g;

    SDL_Init(SDL_INIT_EVERYTHING);
    initSurface();
    initBoard();
    initPieces();
}

void Gui::initSurface()
{
    tileSurface[0] = IMG_Load("imgs/whiteSqr.png");
    tileSurface[1] = IMG_Load("imgs/blackSqr.png");

    pieceSurface[0] = IMG_Load("imgs/wP.png");
    pieceSurface[1] = IMG_Load("imgs/wN.png");
    pieceSurface[2] = IMG_Load("imgs/wB.png");
    pieceSurface[3] = IMG_Load("imgs/wR.png");
    pieceSurface[4] = IMG_Load("imgs/wQ.png");
    pieceSurface[5] = IMG_Load("imgs/wK.png");
    pieceSurface[6] = IMG_Load("imgs/bP.png");
    pieceSurface[7] = IMG_Load("imgs/bN.png");
    pieceSurface[8] = IMG_Load("imgs/bB.png");
    pieceSurface[9] = IMG_Load("imgs/bR.png");
    pieceSurface[10] = IMG_Load("imgs/bQ.png");
    pieceSurface[11] = IMG_Load("imgs/bK.png");
}

void Gui::initBoard()
{
    bool isWhite = false;
    int tileSize = SCREEN_SIZE / 8;

    for(Sqr rank=RANK_8; rank>=RANK_1; --rank)
    {
        for(Sqr file=FILE_A; file<=FILE_H; ++file)
        {
            auto sqr = fRSqr(file, rank);

            tiles.push_back(Tile(tileSize * file, tileSize * rank, tileSize, file, rank, sqr, isWhite, renderer));
            isWhite = !isWhite;
        }
        isWhite = !isWhite;
    }

    for (int i=0; i<tiles.size(); i++)
    {
        tiles[i].initTexture(tileSurface);
    }
}

void Gui::initPieces()
{
    for (int i=0; i<64; i++)
    {
        if (game->getBoard().position[mailbox64[i]] != EMPTY)
        {
            tiles[i].setPiece(new GuiPiece(tiles[i].getPosition().x, tiles[i].getPosition().y,
                tiles[i].getPosition().w, tiles[i].getSqr(), game->getBoard().position[mailbox64[i]], renderer));

            tiles[i].getPiece()->initTexture(pieceSurface[game->getBoard().position[mailbox64[i]]-1]);
        }
    }
}

void Gui::run()
{
    bool running = true;
    SDL_Event e;

    while (running)
    {
        if (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                running = false;
            }
            else if (e.type == SDL_KEYDOWN)
            {
                switch(e.key.keysym.sym)
                {
                    case SDLK_UP:
                    {
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                switch (e.button.button)
                {
                    case SDL_BUTTON_LEFT:
                    {
                        for (int i=0; i<64; i++)
                        {
                            int mouseX = e.button.x;
                            int mouseY = e.button.y;
                            SDL_Rect mousePos {mouseX, mouseY, 1, 1};

                            if (tiles[i].getPiece() != NULL && tiles[i].isCollide(mousePos))
                            {
                                movingPiece = tiles[i].getPiece();
                                break;
                            }
                        }
                    }
                }
            }
            else if (e.type == SDL_MOUSEMOTION)
            {
                if (movingPiece != NULL)
                {
                    int tileSize = SCREEN_SIZE / 8;
                    int mouseX = e.button.x;
                    int mouseY = e.button.y;
                    movingPiece->position.x = mouseX - tileSize/ 2;
                    movingPiece->position.y = mouseY - tileSize / 2;
                }
            }
            else if (e.type == SDL_MOUSEBUTTONUP)
            {
                for (int i=0; i<64; i++)
                {
                    int mouseX = e.button.x;
                    int mouseY = e.button.y;
                    SDL_Rect mousePos {mouseX, mouseY, 1, 1};

                    if (tiles[i].isCollide(mousePos))
                    {
                        movingPiece->position.x = tiles[i].getPosition().x;
                        movingPiece->position.y = tiles[i].getPosition().y;
                        tiles[i].setPiece(movingPiece);
                        movingPiece = NULL;

                        break;
                    }
                }
            }
        }

        update();
        render();

        SDL_Delay(1000/30);
    }
}

void Gui::update()
{
    for (int i=0; i<tiles.size(); i++)
    {
        tiles[i].update();
    }
}

void Gui::render()
{
    SDL_RenderClear(renderer);

    for (int i=0; i<tiles.size(); i++)
    {
        tiles[i].render();
    }

    for (int i=0; i<tiles.size(); i++)
    {
        if (tiles[i].getPiece() != NULL)
            tiles[i].getPiece()->render();
    }

    if (movingPiece != NULL)
    {
        movingPiece->render();
    }

    SDL_RenderPresent(renderer);
}

Gui::~Gui()
{
    for (int i=0; i<64; i++)
    {
        tiles[i].destroyPiece();
    }

    for (int i=0; i<2; i++)
    {
        SDL_FreeSurface(tileSurface[i]);
    }

    for (int i=0; i<12; i++)
    {
        SDL_FreeSurface(pieceSurface[i]);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

}