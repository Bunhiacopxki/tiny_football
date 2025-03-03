#ifndef GAME_HPP
#define GAME_HPP

#include <SDL.h>
#include <SDL_mixer.h>
#include "LTexture.hpp"
#include "math.h"
#include <vector>
#include "Dot.hpp"
#include "Ball.hpp"
using namespace std;

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
//The window we'll be rendering to
extern SDL_Window* gWindow;
extern double getDistance(double x1, double y1, double x2, double y2);



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

enum GamePhase {
    PHASE_1,
    PHASE_2,
    PHASE_3
};

class Game {
public:
    // Starts up SDL and creates window
    bool init();
    int RedMark;
    int BlueMark;
    // Loads media
    bool loadMedia();

    // Frees media and shuts down SDL
    void close();
    int menu();
    int mainGame();
    int showInstructions();
    bool showEndScreen();
    void renderText(const std::string& text, int x, int y);
    void renderScoreboard(int red, int blue, int timeElapsed);
    bool checkCollision(Dot &dotMain, Ball &ball);
    // Music
    bool initAudio();
    void changePhase(GamePhase newPhase);
    void loadMusic(const std::string& path);
    void stopMusic();
    Mix_Chunk* loadSoundEffect(const std::string& path);
    
    vector<LTexture> gDotTexture;
    vector<LTexture> gDotTexture2;
    // LTexture gDotTexture;
    vector<LTexture> gBallTexture;
    LTexture gTextTexture;
    LTexture background;
    LTexture circle;
    LTexture circle2;
    //Event handler
	SDL_Event e;
    Mix_Music* currentMusic = nullptr;
    GamePhase phase = PHASE_1;
    // Mix_Chunk* gameOverSound;
};


bool Game::init()
{
    // Initialization flag
    bool success = true;

    RedMark = 0;
    BlueMark = 0;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
    {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        success = false;
        return -1;
    }
    else
    {
        if (!initAudio()) return -1;
        //Set texture filtering to linear
        if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
        {
            printf("Warning: Linear texture filtering not enabled!");
            return -1;
        }

        // Create window
        gWindow = SDL_CreateWindow("Football", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (gWindow == NULL)
        {
            printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
            success = false;
        }
        else
        {
            // Create vsynced renderer for window
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if (gRenderer == NULL)
            {
                printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                success = false;
            }
            else
            {
                // Initialize renderer color
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

                // Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlags) & imgFlags))
                {
                    printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
                    success = false;
                }

                if (TTF_Init() == -1)
                {
                    printf("SDL_ttf không thể khởi tạo! SDL_ttf Error: %s\n", TTF_GetError());
                    success = false;
                }
                else
                {
                    gFont = TTF_OpenFont("arial.ttf", 28);
                    if (gFont == NULL)
                    {
                        printf("Không thể tải font! SDL_ttf Error: %s\n", TTF_GetError());
                        success = false;
                    }
                }
            }
        }

        return success;
    }
}

bool Game::loadMedia()
{
    // Loading success flag
    bool success = true;

    // Load dot texture
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

    gDotTexture2.resize(21);
    for (int i = 0; i < 21; i++)
    {
        std::string path = "./img/blue_char/2." + to_string(i) + ".png"; // Tên file: ball0.png, ball1.png, ...
        if (!gDotTexture2[i].loadFromFile(path))
        {
            printf("Failed to load blue_char texture: %s\n", path.c_str());
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

    if (!background.loadFromFile("./img/field2.jpg"))
    {
        printf("Failed to load backgound texture!\n");
        success = false;
    }

    if (!circle.loadFromFile("./img/circle1.png"))
    {
        printf("Failed to load circle texture!\n");
        success = false;
    }
    if (!circle2.loadFromFile("./img/circle2.png"))
    {
        printf("Failed to load circle texture!\n");
        success = false;
    }

    // Tải nhạc nền và hiệu ứng
    changePhase(PHASE_1);
    return success;
}

void Game::close()
{
    // Free loaded images
    circle.free();
    for (int i = 0; i < 21; i++)
        gDotTexture[i].free();
    for (int i = 0; i < 10; i++)
        gBallTexture[i].free();
    background.free();
    gTextTexture.free();

    // Free loads music
    stopMusic();
    // Mix_FreeChunk(gameOverSound);
    Mix_CloseAudio();

    // Destroy window
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;

    // Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}

void Game::renderText(const std::string &text, int x, int y)
{
    SDL_Color textColor = {0, 0, 0}; // Màu chữ đen
    if (!gTextTexture.loadFromRenderedText(text, textColor))
    {
        printf("Failed to render text: %s\n", text.c_str());
    }
    gTextTexture.render(x, y);
}

void Game::renderScoreboard(int red, int blue, int timeElapsed)
{
    const int cred = (red < 10) ? 574 + 8 : 574;
    const int cblu = (blue < 10) ? 675 + 8 : 675;

    renderText(to_string(red), cred, 10);
    renderText(to_string(blue), cblu, 10);

	// Chia thời gian hiển thị thành phút và giây
	int minutes = timeElapsed / 60;
	int seconds = timeElapsed % 60;
	string timeText = (minutes < 10 ? "0" : "") + to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + to_string(seconds);
	renderText(timeText, 865, 10);
}

// Màn hình

int Game::showInstructions()
{
    SDL_Event e;
    bool back = false;

    SDL_Color textColor = {0, 0, 0, 255};

    std::vector<std::string> introText = {
        "Welcome to Tiny Football!",
        "A fast-paced, 2D football game full of action!",
        "Controls:",
        "Move - Arrow Keys",
        "Shoot - ?",
        "Sprint - ?",
        "Pass/Swap - ?",
        "Press ESC to return"};

    std::vector<LTexture> textTextures(introText.size());

    for (size_t i = 0; i < introText.size(); i++)
    {
        if (!textTextures[i].loadFromRenderedText(introText[i], textColor))
        {
            printf("Không thể hiển thị dòng văn bản: %s\n", introText[i].c_str());
        }
    }

    while (!back)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                return 0; // 0 is for QUIT
            }
            else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
            {
                back = true;
            }
        }

        SDL_SetRenderDrawColor(gRenderer, 200, 200, 200, 255);
        SDL_RenderClear(gRenderer);

        int startY = SCREEN_HEIGHT / 4;
        for (size_t i = 0; i < introText.size(); i++)
        {
            textTextures[i].render((SCREEN_WIDTH - textTextures[i].getWidth()) / 2, startY + i * 50);
        }

        SDL_RenderPresent(gRenderer);
    }
    return back;
}

bool Game::showEndScreen() {
    bool running = true;
    SDL_Event e;

    // Tải hình nền
    SDL_Surface* bgSurface = IMG_Load("./img/end_background.png");
	SDL_Texture* bgTexture = SDL_CreateTextureFromSurface(gRenderer, bgSurface);
	SDL_FreeSurface(bgSurface); // Giải phóng surface sau khi chuyển thành texture

    // Tạo nút Play Again
    SDL_Surface* play_surface = IMG_Load("./img/play_again.png");
	SDL_Texture* play_button = SDL_CreateTextureFromSurface(gRenderer, play_surface);
	SDL_FreeSurface(play_surface);
    int buttonWidth = 250;
	int buttonHeight = 75;
	int centerX = (SCREEN_WIDTH - buttonWidth) / 2;
    Button playButton(centerX, SCREEN_HEIGHT * 1 / 2 + buttonHeight * 2, buttonWidth, buttonHeight, play_button);
    changePhase(PHASE_3);

    LTexture redScoreTexture;
    LTexture blueScoreTexture;
    TTF_Font* largeFont = TTF_OpenFont("arial.ttf", 360);
    TTF_Font* resultFont = TTF_OpenFont("arial.ttf", 60);
    TTF_Font* teamFont = TTF_OpenFont("arial.ttf", 40);

    while (running) {
        int mouseX, mouseY;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                SDL_DestroyTexture(bgTexture);
                return true;
            }

            playButton.handleEvent(&e);
            if (e.type == SDL_MOUSEMOTION) {
                playButton.handleEvent(&e);
            }
            if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				SDL_GetMouseState(&mouseX, &mouseY);
				if (playButton.isClicked(mouseX, mouseY))
                    RedMark = BlueMark = 0;
					return false;
			}
        }

        SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
		SDL_RenderClear(gRenderer);

		SDL_Rect bgRect;
		bgRect.w = SCREEN_WIDTH;
		bgRect.h = SCREEN_HEIGHT;
		bgRect.x = 0;
		bgRect.y = 0;

		SDL_RenderCopy(gRenderer, bgTexture, NULL, &bgRect);
        // Định nghĩa màu chữ
        SDL_Color textWhite = {255, 255, 255};
        SDL_Color textGreen = {0, 128, 0};
        SDL_Color textRed = {255, 36, 0};
        SDL_Color textBlue = {0, 0, 255};

        // Tạo texture mới với font lớn hơn
        if (largeFont == NULL || resultFont == NULL) {
            printf("Không thể tải font lớn! SDL_ttf Error: %s\n", TTF_GetError());
        } 
        else {
            if (redScoreTexture.rac1(resultFont, "RESULT", textGreen)) {
                redScoreTexture.render(SCREEN_WIDTH / 2 - 120, 50); // Điều chỉnh vị trí hiển thị
            }

            if (blueScoreTexture.rac1(teamFont, "Team Red", textRed)) {
                blueScoreTexture.render(SCREEN_WIDTH / 4 + 63, 135); // Điều chỉnh vị trí hiển thị
            }

            if (blueScoreTexture.rac1(teamFont, "Team Blue", textBlue)) {
                blueScoreTexture.render(713, 135); // Điều chỉnh vị trí hiển thị
            }

            if (redScoreTexture.rac1(largeFont, to_string(RedMark), textWhite)) {
                redScoreTexture.render(SCREEN_WIDTH / 4 + 52, 150); // Điều chỉnh vị trí hiển thị
            }

            if (redScoreTexture.rac1(largeFont, "-", textWhite)) {
                redScoreTexture.render(SCREEN_WIDTH / 2 - 57, 135); // Điều chỉnh vị trí hiển thị
            }

            if (blueScoreTexture.rac1(largeFont, to_string(BlueMark), textWhite)) {
                blueScoreTexture.render(713, 150); // Điều chỉnh vị trí hiển thị
            }
        }

		playButton.render(gRenderer);
		SDL_RenderPresent(gRenderer);
    }

    // Giải phóng tài nguyên
    SDL_DestroyTexture(bgTexture);
    TTF_CloseFont(largeFont);
    return true;
}

bool Game::initAudio() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) return false;
    return true;
}

void Game::loadMusic(const std::string& path) {
    // Nếu có nhạc đang phát, dừng lại
    if (currentMusic != nullptr) {
        Mix_HaltMusic();
        Mix_FreeMusic(currentMusic);
        currentMusic = nullptr;
    }

    currentMusic = Mix_LoadMUS(path.c_str());
    if (!currentMusic) {
        return;
    } else {
        Mix_PlayMusic(currentMusic, -1); // Phát nhạc lặp vô hạn
    }
}

void Game::stopMusic() {
    if (currentMusic != nullptr) {
        Mix_HaltMusic();
        Mix_FreeMusic(currentMusic);
        currentMusic = nullptr;
    }
}

void Game::changePhase(GamePhase newPhase) {
    phase = newPhase;

    switch (phase) {
        case PHASE_1:
            loadMusic("./sound/intro.mp3");
            break;
        case PHASE_2:
            loadMusic("./sound/background_music.mp3");
            break;
        case PHASE_3:
            loadMusic("./sound/intro.mp3");
            break;
    }
}

Mix_Chunk* loadSoundEffect(const std::string& path) {
    Mix_Chunk* sound = Mix_LoadWAV(path.c_str());
    if (!sound) {
        return NULL;
    }
    return sound;
}

// bool Game::checkCollision(Circle &a, Circle &b, Ball ball)
bool Game::checkCollision(Dot &dotMain, Ball &ball)
{
    // // Calculate total radius squared
    // int totalRadiusSquared = a.r + b.r;

    // // If the distance between the centers of the circles is less than the sum of their radii
    // if (getDistance(a.x, a.y, b.x, b.y) < (totalRadiusSquared))
    // {
    //     // The circles have collided
    //     ball.teamPossessing =
    //     return true;
    // }

    // // If not
    // return false;
    // Calculate total radius squared
    int totalRadiusSquared = dotMain.mCollider.r + ball.mCollider.r;

    // If the distance between the centers of the circles is less than the sum of their radii
    if (getDistance(dotMain.mCollider.x, dotMain.mCollider.y, ball.mCollider.x, ball.mCollider.y) < (totalRadiusSquared))
    {
        // printf("cuong");
        //  The circles have collided
        ball.teamPossessing = dotMain.getTeam();
        ball.isPassing = false;
        ball.owner = &dotMain;
        return true;
    }

    // If not
    return false;
}
#endif