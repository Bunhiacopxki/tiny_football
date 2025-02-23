#ifndef BALL_HPP
#define BALL_HPP

#include <SDL.h>
#include <vector>
#include <cmath>
#include <cstdlib> 
#include "LTexture.hpp"

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern int direction;
extern double getDistance(int x1, int y1, int x2, int y2);


class Ball {
	public:
		static const int BALL_WIDTH = 30;
		static const int BALL_HEIGHT = 30;
		static const int RANGE = 200;

		//Maximum axis velocity of the dot
		static const int BALL_VEL = 5;
		static const int BALL_VEL_SLOW = 5;
		
        Ball();

		//Takes key presses and adjusts the dot's velocity
		// void handleEvent( SDL_Event& e );

		//Moves the dot
		void move();

		//Shows the dot on the screen
		void render(LTexture& gBallTexture);

    private:
		//The X and Y offsets of the dot
		int mPosX, mPosY;

		//The velocity of the dot
		int mVelX, mVelY;
        
};


Ball::Ball()
{
    
    mPosX = SCREEN_WIDTH / 2;  // Bóng ở giữa màn hình
    mPosY = SCREEN_HEIGHT / 2;

    //Initialize the velocity
    mVelX = 0;
    mVelY = 0;
}

// void Ball::handleEvent( SDL_Event& e )
// {
//     //If a key was pressed
// 	if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
//     {
//         //Adjust the velocity
//         switch( e.key.keysym.sym )
//         {
//             case SDLK_UP: 
// 				mVelY -= DOT_VEL;
// 				direction = 0;
// 				break;
//             case SDLK_DOWN: 
// 				mVelY += DOT_VEL;
// 				direction = 0;
// 				break;
//             case SDLK_LEFT: 
// 				mVelX -= DOT_VEL; 
// 				direction = 1;
// 				break;
//             case SDLK_RIGHT: 
// 				mVelX += DOT_VEL;
// 				direction = 1;
// 				break;
//         }
//     }
//     //If a key was released
//     else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
//     {
//         //Adjust the velocity
//         switch( e.key.keysym.sym )
//         {
//             case SDLK_UP: mVelY += DOT_VEL; break;
//             case SDLK_DOWN: mVelY -= DOT_VEL; break;
//             case SDLK_LEFT: mVelX += DOT_VEL; break;
//             case SDLK_RIGHT: mVelX -= DOT_VEL; break;
//         }
// 		direction = -1;
//     }
// }

void Ball::move()
{
    
        // Cập nhật vị trí của main dot theo điều khiển
        mPosX += mVelX;
        mPosY += mVelY;

        // Kiểm tra va chạm biên
        if ((mPosX < 0) || (mPosX + BALL_WIDTH > SCREEN_WIDTH)) mPosX -= mVelX;
        if ((mPosY < 0) || (mPosY + BALL_HEIGHT > SCREEN_HEIGHT)) mPosY -= mVelY;
    

		if (mPosX < 0) mPosX = 0;
		if (mPosX + BALL_WIDTH > SCREEN_WIDTH) mPosX = SCREEN_WIDTH - BALL_WIDTH;
		if (mPosY < 0) mPosY = 0;
		if (mPosY + BALL_HEIGHT > SCREEN_HEIGHT) mPosY = SCREEN_HEIGHT - BALL_HEIGHT;
}

void Ball::render(LTexture& gBallTexture)
{
    //Show the dot
	gBallTexture.render( mPosX, mPosY );
}


#endif