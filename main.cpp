/*This source code copyrighted by Lazy Foo' Productions 2004-2024
and may not be redistributed without written permission.*/

// Using SDL, SDL_image, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <stdio.h>

#include "LTexture.hpp"
#include "Dot.hpp"
#include "Ball.hpp"
#include "Game.hpp"
#include "KickMeter.hpp"

using namespace std;

enum RETURNSYMBOL
{
	QUIT,
	SUCCESS,
	GAME,
	INSTRUCTION
};

// Screen dimension constants
int SCREEN_WIDTH = 1280;
int SCREEN_HEIGHT = 780;
int direction = -1;

// The window we'll be rendering to
SDL_Window *gWindow = NULL;

// The window renderer
SDL_Renderer *gRenderer = NULL;

TTF_Font *gFont = NULL; // Khai báo biến toàn cục
LTexture gTextTexture;	// Texture cho văn bản

// The application time based timer
class LTimer
{
public:
	// Initializes variables
	LTimer();

	// The various clock actions
	void start();
	void stop();
	void pause();
	void unpause();

	// Gets the timer's time
	Uint32 getTicks();

	// Checks the status of the timer
	bool isStarted();
	bool isPaused();

private:
	// The clock time when the timer started
	Uint32 mStartTicks;

	// The ticks stored when the timer was paused
	Uint32 mPausedTicks;

	// The timer status
	bool mPaused;
	bool mStarted;
};

void Ball::passTo(std::vector<Dot> &players)
{

	if (players.empty())
		return;

	Dot *nearestPlayer = nullptr;
	double minDist = std::numeric_limits<double>::max();

	// Tìm cầu thủ gần nhất (bất kể trên hay dưới)
	for (Dot &player : players)
	{
		if (&player == owner)
			continue;

		double distance = getDistance(owner->getX(), owner->getY(), player.getX(), player.getY());

		if (distance < minDist)
		{
			minDist = distance;
			nearestPlayer = &player;
		}
	}

	// Nếu tìm thấy cầu thủ gần nhất, chuyền bóng
	if (nearestPlayer)
	{
		// Lưu điểm đến
		targetX = nearestPlayer->getX();
		targetY = nearestPlayer->getY();
		startX = owner->getX();
		startY = owner->getY();
		totalDistance = getDistance(startX, startY, targetX, targetY);
		currentDistance = 0;

		// Tính toán hướng chuyền
		double dx = targetX - startX;
		double dy = targetY - startY;
		double distance = sqrt(dx * dx + dy * dy);

		if (distance > 0)
		{
			mVelX = (dx / distance) * 8.0; // Tăng tốc ban đầu
			mVelY = (dy / distance) * 8.0;
		}

		// Cập nhật chủ sở hữu bóng
		isFollowing = 0;
		owner = nullptr;
		isPassing = true;
		teamPossessing = -1;
		lastPassTime = SDL_GetTicks(); // Đánh dấu thời điểm chuyền
	}
}

void Ball::shoot(double angle, double power)
{
	if (owner == nullptr)
		return;
	mVelX = power * cos(angle);
	mVelY = power * sin(angle);
	printf("PassTo called: angle = %f, power = %f,sin = %f, cos = %f, ballVelX= %f, ballVelY = %f\n", angle, power, sin(angle), cos(angle), mVelX, mVelY);
	isShooting = true;
	owner = nullptr;
	lastShootTime = SDL_GetTicks(); // Đánh dấu thời điểm sut
}

void Ball::update(Dot &mainDot, std::vector<Dot> &players)
{
	if (isPassing)
	{
		mPosX += mVelX;
		mPosY += mVelY;
		// Cập nhật quãng đường đã đi
		double currentDistance = getDistance(startX, startY, mPosX, mPosY);
		double segmentDistance = totalDistance / 10.0;
		int currentSegment = currentDistance / segmentDistance;

		if (currentSegment > passedSegments)
		{
			passedSegments = currentSegment;
			double factor = 1.0 - (passedSegments * 0.05);
			factor = std::max(0.05, factor); // Đảm bảo không giảm quá mức
			mVelX = mVelX * factor;
			mVelY = mVelY * factor;
		}

		// Nếu bóng đến nơi hoặc gần đến nơi thì dừng
		if (currentDistance >= totalDistance || (fabs(mVelX) < 0.1 && fabs(mVelY) < 0.1))
		{
			mVelX = 0;
			mVelY = 0;
			isPassing = false;
		}
		// Nếu bóng ra ngoài biên, dừng chuyền
		if (mPosX < 0 || mPosX > SCREEN_WIDTH || mPosY < 0 || mPosY > SCREEN_HEIGHT)
		{
			mVelX = 0;
			mVelY = 0;
			isPassing = false;
		}
	}
	else if (isShooting)
	{
		mPosX += mVelX;
		mPosY += mVelY;
		mVelX *= 0.95;
		mVelY *= 0.95;
		if ((fabs(mVelX) < 0.02 && fabs(mVelY) < 0.02))
		{
			mVelX = 0;
			mVelY = 0;
			isShooting = false;
		}
		if (mPosX < 0 || mPosX > SCREEN_WIDTH || mPosY < 0 || mPosY > SCREEN_HEIGHT)
		{
			isShooting = false;
		}
	}
	else
	{
		passedSegments = 0;
	}
	if (owner == nullptr)
	{
		// printf("check");
		move();
		mainDot.switchMainDot(*this, players);
	}
	if (owner != nullptr && *owner == mainDot)
	{
		follow(mainDot);
	}
	// {
	// 	this->followPlayer(*owner);
	// }
}

void Ball::takeBall(Dot &mainDot)
{
	if (owner)
	{
		return;
	}
	Uint32 currentTime = SDL_GetTicks(); // Lấy thời gian hiện tại
	double distance = getDistance(mPosX, mPosY, mainDot.getX(), mainDot.getY());
	double collisionDistance = mCollider.r + (mainDot.mCollider.r - mainDot.DOT_HEIGHT * 3); // vì r của cầu thủ lúc đầu set bằng 4 lần chiều cao cầu thủ nên t -3 lần đi
	if (distance <= collisionDistance && (currentTime - lastPassTime >= 500) && (currentTime - lastShootTime >= 500))
	{ // Khi bóng chạm Dot
		owner = &mainDot;
		isPassing = false;
		teamPossessing = mainDot.team; // Gán đội giữ bóng
		isFollowing = mainDot.team;
		// Đặt bóng gần chân cầu thủ
		mPosX = mainDot.getX() + Dot::DOT_WIDTH / 4;
		mPosY = mainDot.getY() + Dot::DOT_HEIGHT / 2;
		// printf("checkteam: %d", mainDot.team);
	}
}

void Ball::follow(Dot &player)
{
	// Lấy vị trí cầu thủ
	int playerX = player.mPosX;
	int playerY = player.mPosY;

	// Lấy vận tốc của cầu thủ (để xác định hướng "trước mặt")
	int pVelX = player.mVelX;
	int pVelY = player.mVelY;

	// Tính vector đơn vị cho hướng di chuyển của cầu thủ
	float norm = std::sqrt(float(pVelX * pVelX + pVelY * pVelY));
	float dirX = 0.0f, dirY = 0.0f;
	if (norm > 0.0f)
	{
		dirX = pVelX / norm;
		dirY = pVelY / norm;
	}
	else
	{
		// Nếu cầu thủ không di chuyển, đặt hướng mặc định (ví dụ: về phía phải)
		dirX = 1.0f;
		dirY = 0.0f;
	}

	// Tính vị trí "trước mặt" của cầu thủ với một khoảng cách offset nhất định
	int offset = 40; // khoảng cách cách mặt cầu thủ (có thể điều chỉnh)
	mPosX = playerX + int(dirX * offset);
	mPosY = playerY + int(dirY * offset) + 20;

	// Khi bóng "dính" vào cầu thủ, ngừng di chuyển độc lập
	mVelX = 0;
	mVelY = 0;

	// Đánh dấu rằng bóng đang theo dõi cầu thủ
	// isFollowing = owner->playerID;
	shiftColliders();
}

double getDistance(double x1, double y1, double x2, double y2)
{
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

int Game::menu()
{

	bool inMenu = true;
	// Play now button
	SDL_Surface *play_surface = IMG_Load("./img/play_button.png");
	SDL_Texture *play_button = SDL_CreateTextureFromSurface(gRenderer, play_surface);
	SDL_FreeSurface(play_surface);
	// Introduction button
	SDL_Surface *intro_surface = IMG_Load("./img/introduction_button.png");
	SDL_Texture *intro_button = SDL_CreateTextureFromSurface(gRenderer, intro_surface);
	SDL_FreeSurface(intro_surface);
	// Background
	SDL_Surface *bgSurface = IMG_Load("./img/background.png");
	SDL_Texture *bgTexture = SDL_CreateTextureFromSurface(gRenderer, bgSurface);
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
			{
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

int Game::mainGame()
{
	// Main loop flag
	bool quit = false;

	// Event handler
	Dot *mainDot1 = nullptr;
	Dot *mainDot2 = nullptr;
	std::vector<Dot> dots1; // Danh sách dot 1
	std::vector<Dot> dots2; // Danh sách dot 2
	// Ball
	Ball ball;
	KickMeter kickMeter;
	for (int i = 0; i < 3; i++)
	{
		if (i == 0)
		{
			dots1.push_back(Dot(true, 1, 1));
			mainDot1 = &dots1[i];
			dots2.push_back(Dot(true, 2, 2));
			mainDot2 = &dots2[i];
			continue;
		}
		dots1.push_back(Dot(false, 1, 0));
		dots2.push_back(Dot(false, 2, 0));
	}

	int randomIndex = 1 + (rand() % (dots1.size() - 1));
	// printf("%d,%d", dots1[randomIndex].getTeam(), dots2[randomIndex].getTeam());
	dots1[randomIndex].setGoalKeeper();
	dots2[randomIndex].setGoalKeeper();

	Uint32 lastTime = SDL_GetTicks();
	double deltaTime = 0.0;

	// While application is running
	int frame = 0;
	int frameCount1 = 0;
	int frame_char1 = 0;
	int frameCount2 = 0;
	int frame_char2 = 0;
	(*mainDot1).mainCircle = &circle;
	(*mainDot2).mainCircle = &circle;
	while (!quit)
	{
		Uint32 currentTime = SDL_GetTicks();
		deltaTime = (currentTime - lastTime) / 1000.0;
		lastTime = currentTime;
		// Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			// User requests quit
			if (e.type == SDL_QUIT)
			{
				return QUIT;
			}

			// Handle input for the dot
			(*mainDot1).handleEvent(e, ball, dots1, kickMeter);
			(*mainDot2).handleEvent(e, ball, dots2, kickMeter);
		}
		for (int i = 0; i < 3; i++)
		{
			if (dots1[i].isMain())
			{
				// printf("check5\n");
				mainDot1 = &dots1[i];
			}
			if (dots2[i].isMain())
			{
				mainDot2 = &dots2[i];
			}
		}
		// Move the dot
		// currentTime = SDL_GetTicks();
		// if (checkCollision((*mainDot1), ball) && (currentTime - ball.lastPassTime >= 500) && (currentTime - ball.lastShootTime >= 500))
		// {
		// 	ball.isFollowing = 1;
		// }
		// if (checkCollision((*mainDot2).mCollider, ball.mCollider, ball) && (currentTime - ball.lastPassTime >= 500) && (currentTime - ball.lastShootTime >= 500))
		// {
		// 	ball.isFollowing = 2;
		// }
		if (ball.getKick())
		{
			kickMeter.update(ball);
		}
		else
		{

			// Move the dot
			(*mainDot1).move((*mainDot1), dots1, deltaTime);
			(*mainDot2).move((*mainDot2), dots2, deltaTime);
			for (int i = 0; i < 3; i++)
			{

				if (!dots1[i].isMain())
				{
					dots1[i].move((*mainDot1), dots1, deltaTime);
				}
				if (!dots2[i].isMain())
				{
					dots2[i].move((*mainDot2), dots2, deltaTime);
				}
			}

			for (auto &player : dots1)
			{
				ball.takeBall(player);
			}
			// Cập nhật bóng dựa trên cầu thủ từ cả hai đội
			for (auto &player : dots1)
			{
				ball.takeBall(player);
			}
			for (auto &player : dots2)
			{
				ball.takeBall(player);
			}

			ball.update(*mainDot1, dots1);
			// ball.update(*mainDot2, dots2);
			//     Move the ball
			//   ball.move();
			//   Move the ball
			//  if (ball.isFollowing == 1)
			//  {
			//  	ball.follow(*mainDot1);
			//  }
			//  else if (ball.isFollowing == 2)
			//  {
			//  	ball.follow(*mainDot2);
			//  }
			//  else
			//  	ball.move();
		}
		// printf("check\n");
		//  Clear screen
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(gRenderer);

		background.renderScale(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		renderScoreboard(0, 10);

		// Render objects
		if (frameCount1 % 6 == 0)
		{
			if (!(*mainDot1).isStop())
				frame_char1 = (frame_char1 + 1) % 21; // Cập nhật frame
			else
			{
				frame_char1 = 0;
			}
		}
		// if (frameCount2 % 6 == 0)
		// {
		// 	if (!(*mainDot2).isStop())
		// 		frame_char2 = (frame_char2 + 1) % 21; // Cập nhật frame
		// 	else
		// 	{
		// 		frame_char2 = 0;
		// 	}
		// }
		// printf("check3\n");
		// for (int i = 0; i < 3; i++)
		// {
		// 	if (dots1[i].isMain())
		// 		(*mainDot1).render(gDotTexture[frame_char1]);
		// 	// if (dots2[i].isMain())
		// 	// 	(*mainDot2).render(gDotTexture[frame_char2]);
		// 	dots1[i].render(gDotTexture[frame_char1]);
		// 	// dots2[i].render(gDotTexture[frame_char2]);
		// }
		//(*mainDot1).render(gDotTexture[frame_char1]);
		for (auto &dot : dots1)
		{
			if (dot.isMain())
			{
				(*mainDot1).render(gDotTexture[frame_char1]);
			}
			else
				dot.render(gDotTexture[frame_char1]);
		}

		// (*mainDot2).render(gDotTexture[frame_char2]);
		// for (auto &dot : dots2)
		// 	dot.render(gDotTexture[frame_char2]);
		// printf("check2\n");
		// Chỉ đổi frame sau mỗi 5 vòng lặp
		if (frameCount1 % 5 == 0)
		{
			if (!ball.isStop())
				frame = (frame + 1) % 10; // Cập nhật frame
										  // frameCount = 0; // Reset biến đếm
		}
		if (frameCount1 % 30 == 0)
		{
			frameCount1 = 0;
		}
		frameCount1++; // Tăng biến đếm
		// if (frameCount2 % 30 == 0)
		// {
		// 	frameCount2 = 0;
		// }
		// frameCount2++; // Tăng biến đếm
		// if (frame > 0)
		// 	printf("frame: %d", frame);
		// Ball render
		ball.render(gBallTexture[frame]);
		kickMeter.render(gRenderer, ball); // Vẽ thanh lực sút
		// Update screen
		SDL_RenderPresent(gRenderer);
	}
	return SUCCESS;
}

int main(int argc, char *args[])
{
	Game game;
	// Start up SDL and create window
	if (!game.init())
	{
		printf("Failed to initialize!\n");
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

		// Load media
		if (!game.loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			int menuRet = game.menu();
			switch (menuRet)
			{
			case QUIT:
				return 0;

			case INSTRUCTION:
				if (game.showInstructions() == QUIT)
					return 0;
				break;

			case GAME:
				if (game.mainGame() == QUIT)
					return 0;
			default:
				break;
			}
		}
	}

	// Free resources and close SDL
	game.close();

	return 0;
}
