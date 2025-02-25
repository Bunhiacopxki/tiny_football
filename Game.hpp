#ifndef GAME_HPP
#define GAME_HPP

#include <SDL.h>
#include "LTexture.hpp"
#include "math.h"
#include <vector>
using namespace std;

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

    
        vector<LTexture> gDotTexture;
        // LTexture gDotTexture;
        vector<LTexture> gBallTexture;
        LTexture background;
        LTexture circle;
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
        gDotTexture.resize(21);
        for (int i = 0; i < 21; i++)
        {
            std::string path = "./img/red_char/1." + to_string(i) + ".png"; // Tên file: ball0.png, ball1.png, ...
            if (!gDotTexture[i].loadFromFile(path))
            {
                printf("Failed to load red_char texture: %s\n", path.c_str());
                success = false;
            }
        }
        // if( !gDotTexture.loadFromFile( "./img/test.png") )
        // {
        //     printf( "Failed to load dot texture!\n" );
        //     success = false;
        // }
    
        // Load ball texture
        gBallTexture.resize(10);
        for (int i = 0; i < 10; i++)
        {
            std::string path = "./img/ball" + to_string(i + 1) + ".png"; // Tên file: ball0.png, ball1.png, ...
            if (!gBallTexture[i].loadFromFile(path))
            {
                printf("Failed to load ball texture: %s\n", path.c_str());
                success = false;
            }
        }

        if( !background.loadFromFile( "./img/field2.jpg") )
        {
            printf( "Failed to load backgound texture!\n" );
            success = false;
        }

        // circle.resize(4);
        // for(int i = 0; i < 4; i++){
        //     if( !circle[i].loadFromFile( "./img/circle"+ to_string(i) +".png") )
        //     {
        //         printf( "Failed to load circle texture!\n" );
        //         success = false;
        //     }
        // }
        if( !circle.loadFromFile( "./img/circle1.png") )
        {
            printf( "Failed to load circle texture!\n" );
            success = false;
        }
        return success;
    }
    
    void Game::close()
    {
        //Free loaded images
        circle.free();
        for (int i = 0; i < 21; i++) gDotTexture[i].free();
        for (int i = 0; i < 10; i++) gBallTexture[i].free();
        background.free();
    
        //Destroy window	
        SDL_DestroyRenderer( gRenderer );
        SDL_DestroyWindow( gWindow );
        gWindow = NULL;
        gRenderer = NULL;
    
        //Quit SDL subsystems
        IMG_Quit();
        SDL_Quit();
    }
    
#endif