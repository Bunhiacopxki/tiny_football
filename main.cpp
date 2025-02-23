/*This source code copyrighted by Lazy Foo' Productions 2004-2024
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <stdio.h>

#include "LTexture.hpp"
#include "Dot.hpp"
#include "Ball.hpp"
#include "Game.hpp"
//Screen dimension constants
int SCREEN_WIDTH = 1280;
int SCREEN_HEIGHT = 780;
int direction = -1;

//The window we'll be rendering to
SDL_Window* gWindow = NULL;
        
//The window renderer
SDL_Renderer* gRenderer = NULL;

//The application time based timer
class LTimer
{
    public:
		//Initializes variables
		LTimer();

		//The various clock actions
		void start();
		void stop();
		void pause();
		void unpause();

		//Gets the timer's time
		Uint32 getTicks();

		//Checks the status of the timer
		bool isStarted();
		bool isPaused();

    private:
		//The clock time when the timer started
		Uint32 mStartTicks;

		//The ticks stored when the timer was paused
		Uint32 mPausedTicks;

		//The timer status
		bool mPaused;
		bool mStarted;
};

// class Game {
//     public:
//         //Starts up SDL and creates window
//         bool init();
        
//         //Loads media
//         bool loadMedia();
        
//         //Frees media and shuts down SDL
//         void close();

    
//         LTexture gDotTexture;
//         LTexture gBallTexture;
// };
    
    
//     bool Game::init()
//     {
//         //Initialization flag
//         bool success = true;
    
//         //Initialize SDL
//         if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
//         {
//             printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
//             success = false;
            
//         }
//         else
//         {
//             //Set texture filtering to linear
//             if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
//             {
//                 printf( "Warning: Linear texture filtering not enabled!" );
                

//             }
    
//             //Create window
//             gWindow = SDL_CreateWindow( "Football", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
//             if( gWindow == NULL )
//             {
//                 printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
//                 success = false;
//             }
//             else
//             {
//                 //Create vsynced renderer for window
//                 gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
//                 if( gRenderer == NULL )
//                 {
//                     printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
//                     success = false;
//                 }
//                 else
//                 {
//                     //Initialize renderer color
//                     SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
    
//                     //Initialize PNG loading
//                     int imgFlags = IMG_INIT_PNG;
//                     if( !( IMG_Init( imgFlags ) & imgFlags ) )
//                     {
//                         printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
//                         success = false;
//                     }
//                 }
//             }
//         }
    
//         return success;
//     }
    
//     bool Game::loadMedia()
//     {
//         //Loading success flag
//         bool success = true;
    
//         //Load dot texture
//         if( !gDotTexture.loadFromFile( "./img/dot.bmp") )
//         {
//             printf( "Failed to load dot texture!\n" );
//             success = false;
//         }
    
//         // Load ball texture
//         if( !gBallTexture.loadFromFile( "./img/ball.png") )
//         {
//             printf( "Failed to load ball texture!\n" );
//             success = false;
//         }
    
//         return success;
//     }
    
//     void Game::close()
//     {
//         //Free loaded images
//         gDotTexture.free();
//         gBallTexture.free();
    
//         //Destroy window	
//         SDL_DestroyRenderer( gRenderer );
//         SDL_DestroyWindow( gWindow );
//         gWindow = NULL;
//         gRenderer = NULL;
    
//         //Quit SDL subsystems
//         IMG_Quit();
//         SDL_Quit();
//     }

double getDistance(int x1, int y1, int x2, int y2)
{
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

int main( int argc, char* args[] )
{
	Game game;
	//Start up SDL and create window
	if( !game.init() )
	{
		printf( "Failed to initialize!\n" );

	}
	else
	{
		//Load media
		if( !game.loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{	
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//The dot that will be moving around on the screen
			Dot mainDot(true);  // Dot chính do người chơi điều khiển
    		std::vector<Dot> dots; // Danh sách dot phụ
			// Ball
			Ball ball;

			for (int i = 0; i < 5; i++)
			{
				dots.push_back(Dot(false));
			}

			//While application is running
			while( !quit )
			{
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}

					//Handle input for the dot
					mainDot.handleEvent( e );
				}

				//Move the dot
				mainDot.move(mainDot, dots);
				for (auto &dot : dots)
				{
					dot.move(mainDot, dots);
				}
				// Move the ball
				ball.move();

				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );

				//Render objects
				mainDot.render(game.gDotTexture);
				for (auto &dot : dots)
				{
					dot.render(game.gDotTexture);
				}

				// Ball render
				ball.render(game.gBallTexture);

				//Update screen
				SDL_RenderPresent( gRenderer );
			}
		}
	}

	//Free resources and close SDL
	game.close();

	return 0;
}
