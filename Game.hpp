#ifndef GAME_HPP
#define GAME_HPP

#include <SDL.h>
#include "LTexture.hpp"
#include "math.h"

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
//The window we'll be rendering to
extern SDL_Window* gWindow;

class Game {
public:
    //Starts up SDL and creates window
    bool init();
    
    //Loads media
    bool loadMedia();
    
    //Frees media and shuts down SDL
    void close();

    void showInstructions();

    LTexture gDotTexture;
    LTexture gBallTexture;
    LTexture gTextTexture;
};
    
    
bool Game::init()
{
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = false;
        return -1;
    }
    else
    {
        //Set texture filtering to linear
        if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
        {
            printf( "Warning: Linear texture filtering not enabled!" );
            return -1;

        }

        //Create window
        gWindow = SDL_CreateWindow( "Football", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( gWindow == NULL )
        {
            printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            //Create vsynced renderer for window
            gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
            if( gRenderer == NULL )
            {
                printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
                success = false;
            }
            else
            {
                //Initialize renderer color
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

                //Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if( !( IMG_Init( imgFlags ) & imgFlags ) )
                {
                    printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
                    success = false;
                }

                if (TTF_Init() == -1)
                {
                    printf("SDL_ttf không thể khởi tạo! SDL_ttf Error: %s\n", TTF_GetError());
                    success = false;
                }
                else
                {
                    gFont = TTF_OpenFont("lazy.ttf", 28);
                    if (gFont == NULL)
                    {
                        printf("Không thể tải font! SDL_ttf Error: %s\n", TTF_GetError());
                        success = false;
                    }
                }
            }
        }
    }

    return success;
}

bool Game::loadMedia()
{
    //Loading success flag
    bool success = true;

    //Load dot texture
    if( !gDotTexture.loadFromFile( "./img/dot.bmp") )
    {
        printf( "Failed to load dot texture!\n" );
        success = false;
    }

    // Load ball texture
    if( !gBallTexture.loadFromFile( "./img/ball.png") )
    {
        printf( "Failed to load ball texture!\n" );
        success = false;
    }

    return success;
}

void Game::close()
{
    //Free loaded images
    gDotTexture.free();
    gBallTexture.free();
    gTextTexture.free();

    //Destroy window	
    SDL_DestroyRenderer( gRenderer );
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;
    gRenderer = NULL;

    //Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}

void Game::showInstructions()
{
    SDL_Event e;
    bool back = false;

    SDL_Color textColor = {0, 0, 0, 255};

    if (!gTextTexture.loadFromRenderedText("Press ESC to return", textColor))
    {
        printf("Không thể hiển thị văn bản hướng dẫn!\n");
    }

    while (!back)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                exit(0);
            }
            else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
            {
                back = true;
            }
        }

        SDL_SetRenderDrawColor(gRenderer, 200, 200, 200, 255);
        SDL_RenderClear(gRenderer);

        gTextTexture.render((SCREEN_WIDTH - gTextTexture.getWidth()) / 2, SCREEN_HEIGHT / 2);

        SDL_RenderPresent(gRenderer);
    }
}
    
#endif