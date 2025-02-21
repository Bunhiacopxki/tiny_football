/*This source code copyrighted by Lazy Foo' Productions 2004-2024
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <cmath>
#include <cstdlib> 

//Screen dimension constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 780;
int direction = -1;

//Texture wrapper class
class LTexture
{
	public:
		//Initializes variables
		LTexture();

		//Deallocates memory
		~LTexture();

		//Loads image at specified path
		bool loadFromFile( std::string path );
		
		#if defined(SDL_TTF_MAJOR_VERSION)
		//Creates image from font string
		bool loadFromRenderedText( std::string textureText, SDL_Color textColor );
		#endif

		//Deallocates texture
		void free();

		//Set color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );

		//Set blending
		void setBlendMode( SDL_BlendMode blending );

		//Set alpha modulation
		void setAlpha( Uint8 alpha );
		
		//Renders texture at given point
		void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

		//Gets image dimensions
		int getWidth();
		int getHeight();

	private:
		//The actual hardware texture
		SDL_Texture* mTexture;

		//Image dimensions
		int mWidth;
		int mHeight;
};

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

//The dot that will move around on the screen
class Dot
{
    public:
		//The dimensions of the dot
		static const int DOT_WIDTH = 20;
		static const int DOT_HEIGHT = 20;
		static const int RANGE = 200;

		//Maximum axis velocity of the dot
		static const int DOT_VEL = 5;
		static const int DOT_VEL_SLOW = 5;
		

		//Initializes the variables
		Dot(bool isMainDot = false);

		//Takes key presses and adjusts the dot's velocity
		void handleEvent( SDL_Event& e );

		//Moves the dot
		void move(Dot &mainDot, std::vector<Dot> &otherDots);

		//Shows the dot on the screen
		void render();

    private:
		//The X and Y offsets of the dot
		int mPosX, mPosY;

		//The velocity of the dot
		int mVelX, mVelY;
		bool mIsMainDot;  // Xác định dot chính
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Scene textures
LTexture gDotTexture;

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile( std::string path )
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

#if defined(SDL_TTF_MAJOR_VERSION)
bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
	if( textSurface != NULL )
	{
		//Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		if( mTexture == NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}
	else
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}

	
	//Return success
	return mTexture != NULL;
}
#endif

void LTexture::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}
		
void LTexture::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

double getDistance(int x1, int y1, int x2, int y2)
{
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

Dot::Dot(bool isMainDot)
{
    // Đặt vị trí ngẫu nhiên trên màn hình cho dot phụ
    if (!isMainDot)
    {
        mPosX = rand() % (SCREEN_WIDTH - DOT_WIDTH);
        mPosY = rand() % (SCREEN_HEIGHT - DOT_HEIGHT);
    }
    else
    {
        mPosX = SCREEN_WIDTH / 2;  // Dot chính ở giữa màn hình
        mPosY = SCREEN_HEIGHT / 2;
    }

    //Initialize the velocity
    mVelX = 0;
    mVelY = 0;
	mIsMainDot = isMainDot;
}

void Dot::handleEvent( SDL_Event& e )
{
	if (!mIsMainDot) return; // Chỉ dot chính nhận input

    //If a key was pressed
	if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_UP: 
				mVelY -= DOT_VEL;
				direction = 0;
				break;
            case SDLK_DOWN: 
				mVelY += DOT_VEL;
				direction = 0;
				break;
            case SDLK_LEFT: 
				mVelX -= DOT_VEL; 
				direction = 1;
				break;
            case SDLK_RIGHT: 
				mVelX += DOT_VEL;
				direction = 1;
				break;
        }
    }
    //If a key was released
    else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_UP: mVelY += DOT_VEL; break;
            case SDLK_DOWN: mVelY -= DOT_VEL; break;
            case SDLK_LEFT: mVelX += DOT_VEL; break;
            case SDLK_RIGHT: mVelX -= DOT_VEL; break;
        }
		direction = -1;
    }
}

void Dot::move(Dot &mainDot, std::vector<Dot> &dots)
{
    if (mIsMainDot) {
        // Cập nhật vị trí của main dot theo điều khiển
        mPosX += mVelX;
        mPosY += mVelY;

        // Kiểm tra va chạm biên
        if ((mPosX < 0) || (mPosX + DOT_WIDTH > SCREEN_WIDTH)) mPosX -= mVelX;
        if ((mPosY < 0) || (mPosY + DOT_HEIGHT > SCREEN_HEIGHT)) mPosY -= mVelY;
    }
    else
    {
        // Nếu main dot đứng yên, dot phụ cũng đứng yên
        if (mainDot.mVelX == 0 && mainDot.mVelY == 0) {
            return;
        }

        double distance = getDistance(mPosX, mPosY, mainDot.mPosX, mainDot.mPosY);
		double speed = sqrt(mainDot.mVelX * mainDot.mVelX + mainDot.mVelY * mainDot.mVelY);

        if (distance < RANGE) {  
            // Nếu dot phụ quá gần main dot, né ra
            double dx = (mPosX - mainDot.mPosX) / distance;
            double dy = (mPosY - mainDot.mPosY) / distance;

			double escapeSpeed = speed * 1.5;

            mVelX += (dx * escapeSpeed - mVelX) * 0.3;	
			mVelY += (dy * escapeSpeed - mVelY) * 0.3;

			mPosX += mVelX * 0.9;
			mPosY += mVelY * 0.9;

        }
        else if (distance > RANGE) {
            // Nếu nằm ngoài RANGE, di chuyển lại gần main dot
            if (mPosX < mainDot.mPosX) mPosX += DOT_VEL_SLOW;
            else if (mPosX > mainDot.mPosX) mPosX -= DOT_VEL_SLOW;
    
            if (mPosY < mainDot.mPosY) mPosY += DOT_VEL_SLOW;
            else if (mPosY > mainDot.mPosY) mPosY -= DOT_VEL_SLOW;
        }
        else {
            // Nếu nằm trong RANGE, di chuyển theo góc phần tư tương ứng
            double angle = 0;

            if (mainDot.mVelY < 0) { // Main dot đi lên
                if (mPosX < mainDot.mPosX) angle = 90 + (rand() % 90); // 90 đến 180
                else angle = rand() % 90; // 0 đến 90
            }
            else if (mainDot.mVelY > 0) { // Main dot đi xuống
                if (mPosX < mainDot.mPosX) angle = 180 + (rand() % 90); // 180 đến 270
                else angle = 270 + (rand() % 90); // 270 đến 360
            }
            else if (mainDot.mVelX > 0) { // Main dot đi sang phải
                if (mPosY > mainDot.mPosY) angle = 270 + (rand() % 90); // 270 đến 360
                else angle = rand() % 90; // 0 đến 90
            }
            else if (mainDot.mVelX < 0) { // Main dot đi sang trái
                if (mPosY > mainDot.mPosY) angle = 180 + (rand() % 90); // 180 đến 270
                else angle = 90 + (rand() % 90); // 90 đến 180
            }

            // Chuyển đổi góc từ độ sang radian
            double radian = angle * (M_PI / 180.0);

            // Cập nhật vị trí của dot phụ
            mPosX += speed * cos(radian);
            mPosY += speed * sin(radian);
        }

        // *** Xử lý va chạm giữa các dot phụ ***
        for (Dot &other : dots) {
            if (&other == this || other.mIsMainDot) continue; // Không xét chính nó hoặc main dot

            double d = getDistance(mPosX, mPosY, other.mPosX, other.mPosY);

            if (d < RANGE) { // Nếu hai dot phụ chạm nhau
                double overlap = RANGE - d;
				if (d > 1e-5) {
					double dx = (mPosX - other.mPosX) / d;
					double dy = (mPosY - other.mPosY) / d;

					// Đẩy hai dot phụ ra xa để tránh chồng lên nhau
					mPosX += dx * overlap * 0.02;
					mPosY += dy * overlap * 0.02;
					other.mPosX -= dx * overlap * 0.02;
					other.mPosY -= dy * overlap * 0.02;
				}
            }
        }

		if (mPosX < 0) mPosX = 0;
		if (mPosX + DOT_WIDTH > SCREEN_WIDTH) mPosX = SCREEN_WIDTH - DOT_WIDTH;
		if (mPosY < 0) mPosY = 0;
		if (mPosY + DOT_HEIGHT > SCREEN_HEIGHT) mPosY = SCREEN_HEIGHT - DOT_HEIGHT;

    }
}

void Dot::render()
{
    //Show the dot
	gDotTexture.render( mPosX, mPosY );
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
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

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load dot texture
	if( !gDotTexture.loadFromFile( "dot.bmp" ) )
	{
		printf( "Failed to load dot texture!\n" );
		success = false;
	}

	return success;
}

void close()
{
	//Free loaded images
	gDotTexture.free();

	//Destroy window	
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia() )
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

				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );

				//Render objects
				mainDot.render();
				for (auto &dot : dots)
				{
					dot.render();
				}

				//Update screen
				SDL_RenderPresent( gRenderer );
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}
