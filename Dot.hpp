#ifndef DOT_HPP
#define DOT_HPP

#include <SDL.h>
#include <vector>
#include <cmath>
#include <cstdlib> 
#include "LTexture.hpp"

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

extern double getDistance(int x1, int y1, int x2, int y2);

struct Circle
{
	int x, y;
	int r;
};

class Dot
{
    public:
		//The dimensions of the dot
		static const int DOT_WIDTH = 20;
		static const int DOT_HEIGHT = 20;
		static const int RANGE = 200;

		//Maximum axis velocity of the dot
		static const int DOT_VEL = 3;
		static const int DOT_VEL_SLOW = 5;
		
        double mAngle;
		//Initializes the variables
		Dot(bool isMainDot, int x, int y);

		//Takes key presses and adjusts the dot's velocity
		void handleEvent( SDL_Event& e );

		//Moves the dot
		void move(Dot &mainDot, std::vector<Dot> &otherDots, double deltaTime);

		//Shows the dot on the screen
		void render(LTexture& gDotTexture);

        void rotate(LTexture& gDotTexture);

        //Dot's collision box
		Circle mCollider;

        LTexture* mainCircle;

        void rac();

        bool isStop();

        void setGoalKeeper() {
            goalkeeper = true;
            mVelY = DOT_VEL_SLOW;
        }

		//The X and Y offsets of the dot
		int mPosX, mPosY;

        //The velocity of the dot
		int mVelX, mVelY;
		bool mIsMainDot;  // Xác định dot chính
        double mMoveTimer = 0.5;
        bool goalkeeper = false;
        bool keyUP = false, keyDOWN = false, keyLEFT = false, keyRIGHT = false;
};

Dot::Dot(bool isMainDot, int x, int y)
{
    // Đặt vị trí ngẫu nhiên trên màn hình cho dot phụ
    mPosX = x;
    mPosY = y;

    //Initialize the velocity
    mVelX = 0;
    mVelY = 0;
	mIsMainDot = isMainDot;

	mCollider.r = DOT_HEIGHT * 4;
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
                keyUP = true;
				break;
            case SDLK_DOWN: 
                keyDOWN = true;
				break;
            case SDLK_LEFT: 
                keyLEFT = true;
				break;
            case SDLK_RIGHT: 
                keyRIGHT = true;
				break;
        }
    }
    //If a key was released
    else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_UP: keyUP = false; break;
            case SDLK_DOWN: keyDOWN = false; break;
            case SDLK_LEFT: keyLEFT = false; break;
            case SDLK_RIGHT: keyRIGHT = false; break;
        }
    }

    // Reset vận tốc
    mVelX = 0;
    mVelY = 0;

    // Cập nhật vận tốc dựa trên trạng thái phím
    if (keyUP)    mVelY = -DOT_VEL;
    if (keyDOWN)  mVelY = DOT_VEL;
    if (keyLEFT)  mVelX = -DOT_VEL;
    if (keyRIGHT) mVelX = DOT_VEL;

    // Giảm vận tốc khi đi chéo
    if (keyUP || keyDOWN) {
        if (keyLEFT || keyRIGHT) {
            mVelX = static_cast<int>(mVelX / std::sqrt(2));
            mVelY = static_cast<int>(mVelY / std::sqrt(2));
        }
    }
}

void Dot::rac() {
    if (mPosX < 10) {
        mPosX = mCollider.x = 10;
    }
    if (mPosX + DOT_WIDTH > SCREEN_WIDTH - 25) {
        mCollider.x = mPosX = SCREEN_WIDTH - DOT_WIDTH - 25;
    }
    if (mPosY < 15) {
        mCollider.y = mPosY = 15;
    }
    if (mPosY + DOT_HEIGHT > SCREEN_HEIGHT - 35) {
        mCollider.y = mPosY = SCREEN_HEIGHT - DOT_HEIGHT - 35;
    }
}

void Dot::move(Dot &mainDot, std::vector<Dot> &dots, double deltaTime)
{
    if (mIsMainDot) {
        // Cập nhật vị trí của main dot theo điều khiển
        mPosX += mVelX;
        mPosY += mVelY;
        mCollider.x = mPosX;
        mCollider.y = mPosY;

        // Tính toán góc xoay dựa trên hướng di chuyển
        if (mVelX != 0 || mVelY != 0)
        {
            mAngle = atan2(mVelY, mVelX) * (180.0 / M_PI); // Chuyển radian sang độ
        }

        rac();
    }
    else if (goalkeeper) {
        if (mPosY < SCREEN_HEIGHT / 2 - 20 * DOT_VEL_SLOW || mPosY > SCREEN_HEIGHT / 2 + 20 * DOT_VEL_SLOW) {
            mVelY = -mVelY;
        }

        mPosY += mVelY;
        mPosX = 10;
        mCollider.x = mPosX;
        mCollider.y = mPosY;
        return;
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

        mMoveTimer -= deltaTime;

        if (mMoveTimer <= 0) {
            mMoveTimer = 0.5;
            double distance = getDistance(mPosX, mPosY, mainDot.mPosX, mainDot.mPosY);

            if (distance < RANGE) {  
                // Nếu dot phụ quá gần main dot, né ra
                double dx = (mPosX - mainDot.mPosX) / distance;
                double dy = (mPosY - mainDot.mPosY) / distance;

                mVelX = dx * DOT_VEL_SLOW;
                mVelY = dy * DOT_VEL_SLOW;
            }
            else if (distance > RANGE) {
                // Nếu nằm ngoài RANGE, di chuyển lại gần main dot
                double dx = (mainDot.mPosX - mPosX) / distance;
                double dy = (mainDot.mPosY - mPosY) / distance;

                mVelX = dx * DOT_VEL_SLOW;
                mVelY = dy * DOT_VEL_SLOW;
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
                mVelX = DOT_VEL_SLOW * cos(radian);
                mVelY = DOT_VEL_SLOW * sin(radian);
            }
        }

        mPosX += mVelX;
        mPosY += mVelY;
        mCollider.x = mPosX;
        mCollider.y = mPosY;

        rac();

        // *** Xử lý va chạm giữa các dot phụ ***
        for (Dot &other : dots) {
            if (&other == this || other.mIsMainDot || other.goalkeeper) continue; // Không xét chính nó hoặc main dot

            double dx = mCollider.x - other.mCollider.x;
            double dy = mCollider.y - other.mCollider.y;
            double distance = sqrt(dx * dx + dy * dy);
            double minDistance = mCollider.r + other.mCollider.r;

            if (distance < minDistance && distance > 1e-5) {
                // Hai dot đang va chạm, cần đẩy ra xa
                double overlap = minDistance - distance;
                dx /= distance;
                dy /= distance;
    
                // Đẩy dot này ra xa
                mPosX += dx * overlap * 0.5;
                mPosY += dy * overlap * 0.5;
                mCollider.x = mPosX;
                mCollider.y = mPosY;

                rac();
    
                // Đẩy dot kia ra xa theo hướng ngược lại (trừ mainDot)
                if (!other.mIsMainDot) {
                    other.mPosX -= dx * overlap * 0.5;
                    other.mPosY -= dy * overlap * 0.5;
                    other.mCollider.x = other.mPosX;
                    other.mCollider.y = other.mPosY;

                    other.rac();
                }
            }
        }

        if (!mIsMainDot || !goalkeeper) {
            double dx = mCollider.x - mainDot.mCollider.x;
            double dy = mCollider.y - mainDot.mCollider.y;
            double distance = sqrt(dx * dx + dy * dy);
    
            if (distance < 2 * mCollider.r && distance > 1e-5) { // Main dot đến gần
                dx /= distance;
                dy /= distance;
    
                // Quyết định né trái hay phải (random hoặc dựa vào vị trí ban đầu)
                double angleOffset = (rand() % 2 == 0) ? 90 : -90;
                double angle = atan2(dy, dx) + angleOffset * (M_PI / 180.0);
    
                // Đẩy dot phụ sang hai bên
                mPosX += cos(angle) * DOT_VEL_SLOW;
                mPosY += sin(angle) * DOT_VEL_SLOW;
                mCollider.x = mPosX;
                mCollider.y = mPosY;

                rac();
            }
        }
    }
}

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