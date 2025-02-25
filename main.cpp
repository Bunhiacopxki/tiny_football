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

class Button {
public:
	SDL_Rect mBox; // Vùng button
	SDL_Texture* mTexture;
	SDL_Color defaultColor = {255, 255, 255, 255};  // Màu gốc (trắng)
	SDL_Color hoverColor = {200, 200, 200, 255};    // Màu khi hover (xám)
	bool isHovered = false;

	Button(int x, int y, int w, int h, SDL_Texture* texture) {
		mBox = {x, y, w, h};
		mTexture = texture;
	}

	void handleEvent(SDL_Event* e) {
		int x, y;
		SDL_GetMouseState(&x, &y);
		isHovered = (x >= mBox.x && x <= mBox.x + mBox.w && y >= mBox.y && y <= mBox.y + mBox.h);
	}

	void render(SDL_Renderer* renderer) {
		if (isHovered) {
			SDL_SetTextureColorMod(mTexture, 255, 255, 200); // Làm sáng màu button
		} else {
			SDL_SetTextureColorMod(mTexture, 255, 255, 255); // Màu bình thường
		}
	
		// Vẽ button
		SDL_RenderCopy(renderer, mTexture, NULL, &mBox);
	
		// Vẽ viền xanh khi hover
		if (isHovered) {
			SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
			int borderThickness = 3; // Độ dày của viền
			// Vẽ nhiều hình chữ nhật để tạo viền dày
			for (int i = 0; i < borderThickness; i++) {
				SDL_Rect borderRect = {mBox.x - i, mBox.y - i, mBox.w + 2 * i, mBox.h + 2 * i};
				SDL_RenderDrawRect(renderer, &borderRect);
			}
		}
	}	

	bool isClicked(int mouseX, int mouseY)
	{
		return (mouseX > mBox.x && mouseX < mBox.x + mBox.w &&
				mouseY > mBox.y && mouseY < mBox.y + mBox.h);
	}
};

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

			bool inMenu = true;

			// Play now button
			SDL_Surface* play_surface = IMG_Load("./img/play_button.png");
			SDL_Texture* play_button = SDL_CreateTextureFromSurface(gRenderer, play_surface);
			SDL_FreeSurface(play_surface);
			// Introduction button
			SDL_Surface* intro_surface = IMG_Load("./img/introduction_button.png");
			SDL_Texture* intro_button = SDL_CreateTextureFromSurface(gRenderer, intro_surface);
			SDL_FreeSurface(intro_surface);
			// Background
			SDL_Surface* bgSurface = IMG_Load("./img/background.png");
			SDL_Texture* bgTexture = SDL_CreateTextureFromSurface(gRenderer, bgSurface);
			SDL_FreeSurface(bgSurface); // Giải phóng surface sau khi chuyển thành texture

			int buttonWidth = 250;
			int buttonHeight = 100;
			int centerX = (SCREEN_WIDTH - buttonWidth) / 2;

			Button instructionButton(centerX, SCREEN_HEIGHT * 1 / 2, buttonWidth, buttonHeight, intro_button);
			Button playButton(centerX, SCREEN_HEIGHT * 1 / 2 + buttonHeight * 3 / 2, buttonWidth, buttonHeight, play_button);

			while (inMenu)
			{
				int mouseX, mouseY;
				while (SDL_PollEvent(&e) != 0)
				{
					if (e.type == SDL_QUIT)
						return 0;

					instructionButton.handleEvent(&e);
					playButton.handleEvent(&e);

					if (e.type == SDL_MOUSEMOTION)
					{
						playButton.handleEvent(&e);
    					instructionButton.handleEvent(&e);
					}

					if (e.type == SDL_MOUSEBUTTONDOWN)
					{
						SDL_GetMouseState(&mouseX, &mouseY);
						if (playButton.isClicked(mouseX, mouseY))
							inMenu = false;
						else if (instructionButton.isClicked(mouseX, mouseY))
							game.showInstructions();
					}
				}

				SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
				SDL_RenderClear(gRenderer);


				SDL_Rect bgRect;
				bgRect.w = buttonWidth * 4;
				bgRect.h = buttonHeight * 2;
				bgRect.x = (SCREEN_WIDTH - bgRect.w) / 2;
				bgRect.y = SCREEN_HEIGHT / 2 - buttonHeight * 3;

				SDL_RenderCopy(gRenderer, bgTexture, NULL, &bgRect);

				playButton.render(gRenderer);
				instructionButton.render(gRenderer);

				SDL_RenderPresent(gRenderer);
			}

			//The dot that will be moving around on the screen
			Dot mainDot(true);  // Dot chính do người chơi điều khiển
    		std::vector<Dot> dots; // Danh sách dot phụ
			// Ball
			Ball ball;

			for (int i = 0; i < 4; i++)
			{
				dots.push_back(Dot(false));
			}

			int randomIndex = rand() % dots.size();
			dots[randomIndex].setGoalKeeper();

			Uint32 lastTime = SDL_GetTicks();
			double deltaTime = 0.0;

			//While application is running
			while( !quit )
			{
				Uint32 currentTime = SDL_GetTicks();
				deltaTime = (currentTime - lastTime) / 1000.0;
				lastTime = currentTime;
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
				mainDot.move(mainDot, dots, deltaTime);
				for (auto &dot : dots)
				{
					dot.move(mainDot, dots, deltaTime);
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
			SDL_DestroyTexture(bgTexture);
		}
	}

	//Free resources and close SDL
	game.close();

	return 0;
}
