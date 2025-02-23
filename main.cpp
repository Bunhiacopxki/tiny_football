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
