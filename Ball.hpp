#ifndef BALL_HPP
#define BALL_HPP

#include <SDL.h>
#include <vector>
#include <cmath>
#include <cstdlib> 
#include "LTexture.hpp"
#include "Dot.hpp"

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


		//Moves the dot and checks collision
		void move();

		//Shows the dot on the screen
		void render(LTexture& gBallTexture);

		//Gets collision circle
		Circle& getCollider();

        void follow(Dot&);

        bool isStop();
    
		//The X and Y offsets of the dot
		int mPosX, mPosY;

		//The velocity of the dot
		int mVelX, mVelY;

        bool isFollowing;

        //Dot's collision circle
		Circle mCollider;

		//Moves the collision circle relative to the dot's offset
		void shiftColliders();

};

Ball::Ball()
{
    mPosX = SCREEN_WIDTH / 2;  // Bóng ở giữa màn hình
    mPosY = SCREEN_HEIGHT / 2;

    isFollowing = false;
	//Set collision circle size
	mCollider.r = BALL_WIDTH / 2;
    //Initialize the velocity
    mVelX = 0;
    mVelY = 0;

	//Move collider relative to the circle
	shiftColliders();
}

void Ball::move()
{
    // Di chuyển bóng theo vận tốc
    
    mPosX += mVelX;
    mPosY += mVelY;

    // Kiểm tra va chạm biên theo trục X
    if ( mPosX < 0 )
    {
        // Chỉnh lại vị trí cho sát biên
        mPosX = 0;
        // Đổi dấu vận tốc theo công thức reflection
        // n = (1, 0) => v' = ( -v_x, v_y )
        mVelX = -mVelX;
    }
    else if ( mPosX + BALL_WIDTH > SCREEN_WIDTH )
    {
        mPosX = SCREEN_WIDTH - BALL_WIDTH;
        mVelX = -mVelX;
    }

    // Kiểm tra va chạm biên theo trục Y
    if ( mPosY < 0 )
    {
        mPosY = 0;
        // n = (0, 1) => v' = ( v_x, -v_y )
        mVelY = -mVelY;
    }
    else if ( mPosY + BALL_HEIGHT > SCREEN_HEIGHT )
    {
        mPosY = SCREEN_HEIGHT - BALL_HEIGHT;
        mVelY = -mVelY;
    }

    // Cập nhật lại collider nếu cần
    shiftColliders();
}

void Ball::render(LTexture& gBallTexture)
{
    //Show the dot
	gBallTexture.render( mPosX, mPosY );
}

Circle& Ball::getCollider()
{
	return mCollider;
}

void Ball::shiftColliders()
{
	//Align collider to center of dot
	mCollider.x = mPosX;
	mCollider.y = mPosY;
}

bool Ball::isStop(){
    return mVelX == 0 && mVelY == 0;
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
    int offset = 40;  // khoảng cách cách mặt cầu thủ (có thể điều chỉnh)
    mPosX = playerX + int(dirX * offset);
    mPosY = playerY + int(dirY * offset) + 20;
    
    // Khi bóng "dính" vào cầu thủ, ngừng di chuyển độc lập
    mVelX = 0;
    mVelY = 0;
    
    // Đánh dấu rằng bóng đang theo dõi cầu thủ
    isFollowing = true;
}



#endif