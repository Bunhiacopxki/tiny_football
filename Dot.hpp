#ifndef DOT_HPP
#define DOT_HPP

#include <SDL.h>
#include <vector>
#include <cmath>
#include <cstdlib> 
#include "LTexture.hpp"

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern int direction;
extern double getDistance(int x1, int y1, int x2, int y2);



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
		void render(LTexture& gDotTexture);

        void rotate(LTexture& gDotTexture);

        //Dot's collision box
		SDL_Rect mCollider;

        bool isStop();
        LTexture* mainCircle;
    private:
		//The X and Y offsets of the dot
		int mPosX, mPosY;

        double mAngle;

		//The velocity of the dot
		int mVelX, mVelY;
		bool mIsMainDot;  // Xác định dot chính
};


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

    //Set collision box dimension
	mCollider.w = DOT_WIDTH;
	mCollider.h = DOT_HEIGHT;
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
        mCollider.x = mPosX;
        mCollider.y = mPosY;

        // 🔹 Tính toán góc xoay dựa trên hướng di chuyển
        if (mVelX != 0 || mVelY != 0)
        {
            mAngle = atan2(mVelY, mVelX) * (180.0 / M_PI); // Chuyển radian sang độ
        }
        
        // Kiểm tra va chạm biên
        if ((mPosX < 0) || (mPosX + DOT_WIDTH > SCREEN_WIDTH)){
            mPosX -= mVelX;
            mCollider.x = mPosX;
        }
        if ((mPosY < 0) || (mPosY + DOT_HEIGHT > SCREEN_HEIGHT)){
            mPosY -= mVelY;
            mCollider.y = mPosY;
        }
    }
    else
    {
        // Nếu main dot đứng yên, dot phụ cũng đứng yên
        if (mainDot.mVelX == 0 && mainDot.mVelY == 0) {
            return;
        }

        double dx = mainDot.mPosX - mPosX;
        double dy = mainDot.mPosY - mPosY;
        mAngle = atan2(dy, dx) * (180.0 / M_PI); // 🔹 Xoay Dot phụ về hướng mainDot

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
            mCollider.x = mPosX;
            mCollider.y = mPosY;
            
            // Kiểm tra va chạm biên
            if ((mPosX < 0) || (mPosX + DOT_WIDTH > SCREEN_WIDTH)){
                mPosX -= mVelX;
                mCollider.x = mPosX;
            }
            if ((mPosY < 0) || (mPosY + DOT_HEIGHT > SCREEN_HEIGHT)){
                mPosY -= mVelY;
                mCollider.y = mPosY;
            }

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

// void Dot::render(LTexture& gDotTexture)
// {
//     //Show the dot
// 	// gDotTexture.render( mPosX, mPosY );
//     SDL_Point center = { 40 / 2, 70 / 2 };
//     // double angle = 45;
//     gDotTexture.renderScale(mPosX, mPosY,40,70, NULL, mAngle, &center, SDL_FLIP_NONE);
//     if(mIsMainDot) mainCircle->render(mPosX, mPosY);
// }

void Dot::render(LTexture& gDotTexture)
{
    // 🔹 Lấy kích thước thực tế từ texture
    int dotWidth = gDotTexture.getWidth();
    int dotHeight = gDotTexture.getHeight();

    // 🔹 Xác định tâm ảnh
    SDL_Point center = { dotWidth / 2, dotHeight / 2 };

    // 🔹 Render Dot tại vị trí (mPosX, mPosY) nhưng lấy tâm làm điểm gốc
    gDotTexture.renderScale(mPosX, mPosY, dotWidth, dotHeight, NULL, mAngle, &center, SDL_FLIP_NONE);

    // 🔹 Đảm bảo `mainCircle` luôn trùng tâm với Dot
    if (mIsMainDot)
    {
        mainCircle->render(mPosX - (center.x / 2)-5, mPosY);//,70, 70, NULL, mAngle, &center, SDL_FLIP_NONE); // Tâm của `mainCircle` chính là `mPosX, mPosY`
    }
}


bool Dot::isStop(){
    return mVelX == 0 && mVelY == 0;
}

#endif