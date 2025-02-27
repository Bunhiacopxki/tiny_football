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

enum RETURNSYMBOL {
	QUIT,
	SUCCESS,
	GAME,
	INSTRUCTION
};

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

int Game::menu(){
    
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
			if (e.type == SDL_QUIT){
                SDL_DestroyTexture(bgTexture);
				return QUIT;
            }

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
					return GAME;
				else if (instructionButton.isClicked(mouseX, mouseY))
					return INSTRUCTION;
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
    SDL_DestroyTexture(bgTexture);
	return SUCCESS;
}

int Game::mainGame(){
    //Main loop flag
	bool quit = false;

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
	int frame = 0;
	int frameCount = 0;
	int frame_char = 0;
	mainDot.mainCircle = &circle;
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
				return QUIT;
			}

			//Handle input for the dot
			mainDot.handleEvent( e );
		}

		//Move the dot
		if (checkCollision(mainDot.mCollider, ball.mCollider)){
			ball.isFollowing = true;
		}
		mainDot.move(mainDot, dots, deltaTime);
		for (auto &dot : dots)
		{
			dot.move(mainDot, dots, deltaTime);
		}
		// Move the ball
		if (ball.isFollowing) ball.follow(mainDot);
		else ball.move();

		//Clear screen
		SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
		SDL_RenderClear( gRenderer );

		background.renderScale(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
		renderScoreboard(0, 10);

		//Render objects
		if (frameCount % 6 == 0){
			if(!mainDot.isStop()) frame_char = (frame_char + 1) % 21; // Cập nhật frame
			else{
				frame_char = 0;
			}
		}
		mainDot.render(gDotTexture[frame_char]);
		for (auto &dot : dots)
		{
			dot.render(gDotTexture[frame_char]);
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
		ball.render(gBallTexture[frame]);
		//Update screen
		SDL_RenderPresent( gRenderer );
	}
	return SUCCESS;
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
			int menuRet = game.menu();
			switch (menuRet)
			{
			case QUIT:
				return 0;
			
			case INSTRUCTION:
				if (game.showInstructions() == QUIT) return 0;
				break;
			
			case GAME:
				if (game.mainGame() == QUIT) return 0;
			default:
				break;
			}
		}
	}

	//Free resources and close SDL
	game.close();

	return 0;
}
