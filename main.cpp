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

using namespace std;

//Screen dimension constants
int SCREEN_WIDTH = 1280;
int SCREEN_HEIGHT = 780;
int direction = -1;

//The window we'll be rendering to
SDL_Window* gWindow = NULL;
        
//The window renderer
SDL_Renderer* gRenderer = NULL;

TTF_Font* gFont = NULL; // Khai báo biến toàn cục
LTexture gTextTexture; // Texture cho văn bản

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

void renderText(const std::string& text, int x, int y)
{
    SDL_Color textColor = { 0, 0, 0 }; // Màu chữ đen
    if (!gTextTexture.loadFromRenderedText(text, textColor))
    {
        printf("Failed to render text: %s\n", text.c_str());
    }
    gTextTexture.render(x, y);
}

void renderScoreboard(int red, int blue)
{
    const int cred = (red  < 10) ? 574 + 8 : 574; 
    const int cblu = (blue < 10) ? 675 + 8 : 675;

    renderText(to_string(red) , cred, 10);
    renderText(to_string(blue), cblu, 10);
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
		if (TTF_Init() == -1)
		{
			printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
			return false;
		}

		gFont = TTF_OpenFont("lazy.ttf", 28); // Thay đổi "arial.ttf" thành font có sẵn trong thư mục
		if (gFont == NULL)
		{
			printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
		}

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
			int frame = 0;
			int frameCount = 0;
			int frame_char = 0;
			mainDot.mainCircle = &game.circle;
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

				game.background.renderScale(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
				renderScoreboard(0, 10);

				//Render objects
				if (frameCount % 6 == 0){
					if(!mainDot.isStop()) frame_char = (frame_char + 1) % 21; // Cập nhật frame
					else{
						frame_char = 0;
					}
				}
				mainDot.render(game.gDotTexture[frame_char]);
				for (auto &dot : dots)
				{
					dot.render(game.gDotTexture[frame_char]);
				}

				// Chỉ đổi frame sau mỗi 5 vòng lặp
				if (frameCount % 5 == 0)
				{
					if(!ball.isStop()) frame = (frame + 1) % 10; // Cập nhật frame
					// frameCount = 0; // Reset biến đếm
				}
				if (frameCount % 30 == 0){
					frameCount = 0;
				}
				frameCount++; // Tăng biến đếm
				
				// Ball render
				ball.render(game.gBallTexture[frame]);
				//Update screen
				SDL_RenderPresent( gRenderer );
			}
		}
	}

	//Free resources and close SDL
	game.close();

	return 0;
}
