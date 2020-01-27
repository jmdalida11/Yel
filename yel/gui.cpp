#include "gui.hpp"

#include <SDL2/SDL_image.h>
#include "utils.hpp"

extern void search(board::Game& game);

namespace gui {

using namespace defs;

constexpr int SCREEN_SIZE = 600;

Gui::Gui()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }
    else
    {
        window = SDL_CreateWindow("Yel Chess Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_SIZE, SCREEN_SIZE, SDL_WINDOW_SHOWN);

        if(window == NULL)
        {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        }
        else
        {
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        }
    }

    const std::string Startfen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    game.init();
    utils::loadFen(Startfen, game);
    game.setPositionKey();
    game.generateMove(false);

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
        if (game.getBoard().position[mailbox64[i]] != EMPTY)
        {
            tiles[i].setPiece(new GuiPiece(tiles[i].getPosition().x, tiles[i].getPosition().y,
                tiles[i].getPosition().w, tiles[i].getSqr(), game.getBoard().position[mailbox64[i]], renderer));
            tiles[i].getPiece()->initTexture(pieceSurface[game.getBoard().position[mailbox64[i]]-1]);
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
                    case SDLK_s:
                    {
                        break;
                    }
                    case SDLK_b:
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
                        if (game.getBoard().side != AI)
                        {
                            for (int i=0; i<64; i++)
                            {
                                int mouseX = e.button.x;
                                int mouseY = e.button.y;
                                SDL_Rect mousePos {mouseX, mouseY, 1, 1};

                                if (tiles[i].getPiece() == NULL) continue;
                                if (pieceColor[tiles[i].getPiece()->type] != game.getBoard().side) continue;

                                if (tiles[i].isCollide(mousePos))
                                {
                                    pieceMovingInfo.from = mailbox64[i];
                                    pieceMovingInfo.pieceMoving = tiles[i].getPiece();
                                    pieceMovingInfo.tile = &tiles[i];
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            else if (e.type == SDL_MOUSEMOTION)
            {
                if (pieceMovingInfo.pieceMoving != NULL)
                {
                    int tileSize = SCREEN_SIZE / 8;
                    int mouseX = e.button.x;
                    int mouseY = e.button.y;
                    pieceMovingInfo.pieceMoving->position.x = mouseX - tileSize/ 2;
                    pieceMovingInfo.pieceMoving->position.y = mouseY - tileSize / 2;
                }
            }
            else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT)
            {
                if (pieceMovingInfo.pieceMoving != NULL)
                {
                    for (int i=0; i<64; i++)
                    {
                        int mouseX = e.button.x;
                        int mouseY = e.button.y;
                        SDL_Rect mousePos {mouseX, mouseY, 1, 1};

                        if (tiles[i].isCollide(mousePos))
                        {
                            pieceMovingInfo.to = mailbox64[i];

                            Move move = 0;
                            moveFromTo(move, pieceMovingInfo.from, pieceMovingInfo.to);

                            move = utils::parseMove(move, game);

                            if (move != 0 && game.makeMove(move))
                            {
                                if (ISCAP(move))
                                    delete tiles[i].getPiece();

                                if (!castleMove(move))
                                {
                                    tiles[i].setPiece(pieceMovingInfo.pieceMoving);
                                    tiles[i].alignPiece();
                                    pieceMovingInfo.pieceMoving = NULL;
                                    pieceMovingInfo.tile->setPiece(NULL);
                                }

                                render();
                                game.getBoard().moves.clear();
                                game.generateMove(false);
                            }
                            else
                            {
                                pieceMovingInfo.tile->alignPiece();
                                pieceMovingInfo.pieceMoving = NULL;
                                pieceMovingInfo.tile = NULL;
                                pieceMovingInfo.from = -1;
                                pieceMovingInfo.to = -1;
                            }

                            break;
                        }
                    }
                }
            }
        }

        if (game.getBoard().side == AI)
        {
            search(game);
            Move AImove = game.getBoard().pv[0].m;

            game.makeMove(AImove);

            if (ISCAP(AImove))
                delete tiles[mailbox[TO(AImove)]].getPiece();

            if (!castleMove(AImove))
            {
                tiles[mailbox[TO(AImove)]].setPiece(tiles[mailbox[FROM(AImove)]].getPiece());
                tiles[mailbox[FROM(AImove)]].setPiece(NULL);
                tiles[mailbox[TO(AImove)]].alignPiece();
            }

            game.getBoard().moves.clear();
            game.generateMove(false);
        }

        update();
        render();

        SDL_Delay(10);
    }
}

bool Gui::castleMove(Move move)
{
    if (CASTBITS(move))
    {
        if (game.getBoard().side == WHITE)
        {
            uint8_t dir = CASTBITS(move);

            if (WKSC(dir))
            {
                tiles[mailbox[g1]].setPiece(tiles[mailbox[e1]].getPiece());
                tiles[mailbox[g1]].alignPiece();
                tiles[mailbox[e1]].setPiece(NULL);

                tiles[mailbox[f1]].setPiece(tiles[mailbox[h1]].getPiece());
                tiles[mailbox[f1]].alignPiece();
                tiles[mailbox[h1]].setPiece(NULL);

                return true;
            }
            else if (WQSC(dir))
            {
                tiles[mailbox[c1]].setPiece(tiles[mailbox[e1]].getPiece());
                tiles[mailbox[c1]].alignPiece();
                tiles[mailbox[e1]].setPiece(NULL);

                tiles[mailbox[d1]].setPiece(tiles[mailbox[a1]].getPiece());
                tiles[mailbox[d1]].alignPiece();
                tiles[mailbox[a1]].setPiece(NULL);

                return true;
            }
        }
        else if (game.getBoard().side == BLACK)
        {
            uint8_t dir = CASTBITS(move);

            if (BKSC(dir))
            {
                tiles[mailbox[g8]].setPiece(tiles[mailbox[e8]].getPiece());
                tiles[mailbox[g8]].alignPiece();
                tiles[mailbox[e8]].setPiece(NULL);

                tiles[mailbox[f8]].setPiece(tiles[mailbox[h8]].getPiece());
                tiles[mailbox[f8]].alignPiece();
                tiles[mailbox[h8]].setPiece(NULL);

                return true;
            }
            else if (BQSC(dir))
            {
                tiles[mailbox[c8]].setPiece(tiles[mailbox[e8]].getPiece());
                tiles[mailbox[c8]].alignPiece();
                tiles[mailbox[e8]].setPiece(NULL);

                tiles[mailbox[d8]].setPiece(tiles[mailbox[a8]].getPiece());
                tiles[mailbox[d8]].alignPiece();
                tiles[mailbox[a8]].setPiece(NULL);

                return true;
            }
        }
    }

    return false;
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

    if (pieceMovingInfo.pieceMoving != NULL)
    {
        pieceMovingInfo.pieceMoving->render();
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