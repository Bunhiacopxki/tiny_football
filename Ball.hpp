#ifndef BALL_HPP
#define BALL_HPP

#include <SDL.h>
#include <vector>
#include <cmath>
#include <cstdlib>
#include "LTexture.hpp"
// #include "Dot.hpp"

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern int direction;
extern double getDistance(double x1, double y1, double x2, double y2);
class Dot;
struct Circle
{
    int x, y;
    int r;
};
class Ball
{
public:
    static const int BALL_WIDTH = 30;
    static const int BALL_HEIGHT = 30;
    static const int RANGE = 200;

    // Maximum axis velocity of the dot
    static const int BALL_VEL = 5;
    static const int BALL_VEL_SLOW = 5;
    friend class Dot;
    friend class KickMeter;
    Ball();

    // Moves the dot and checks collision
    void move();

    // Shows the dot on the screen
    void render(LTexture &gBallTexture);
    // void followPlayer(Dot &player);
    void passTo(std::vector<Dot> &players);
    void update(Dot &mainDot, std::vector<Dot> &players); // Cập nhật bóng theo thời gian
    void takeBall(Dot &mainDot);
    void shoot(double angle, double power);
    bool getKick() { return isWaitingForKick; };
    // Gets collision circle
    Circle &getCollider();

    void follow(Dot &);

    bool isStop();

    // The X and Y offsets of the dot
    double mPosX, mPosY;

    // The velocity of the dot
    double mVelX, mVelY;

    int isFollowing;
    bool isShooting = false;
    // chuyền bóng
    bool isPassing = false;
    double targetX, targetY;
    double startX, startY;
    double totalDistance, currentDistance;
    int passedSegments = 0; // Đếm số đoạn đã qua

    Dot *owner = nullptr;
    int teamPossessing = -1; // Đội đang giữ bóng
    double Power = 0.0;
    // thanh lực
    bool isWaitingForKick = false;
    // thời gian chờ tránh chuyền mà lấy lại ngay
    Uint32 lastPassTime = 0; // Lưu thời điểm bóng được chuyền
    Uint32 lastShootTime = 0;

    // Dot's collision circle
    Circle mCollider;

    // Moves the collision circle relative to the dot's offset
    void shiftColliders();
};

Ball::Ball()
{
    mPosX = SCREEN_WIDTH / 2; // Bóng ở giữa màn hình
    mPosY = SCREEN_HEIGHT / 2;
    isPassing = true;
    isFollowing = 0;
    // Set collision circle size
    mCollider.r = BALL_WIDTH / 2;
    // Initialize the velocity
    mVelX = 0;
    mVelY = 0;

    // Move collider relative to the circle
    shiftColliders();
}

void Ball::move()
{
    // Di chuyển bóng theo vận tốc

    mPosX += mVelX;
    mPosY += mVelY;

    // Kiểm tra va chạm biên theo trục X
    if (mPosX < 0)
    {
        // Chỉnh lại vị trí cho sát biên
        mPosX = 0;
        // Đổi dấu vận tốc theo công thức reflection
        // n = (1, 0) => v' = ( -v_x, v_y )
        mVelX = -mVelX;
    }
    else if (mPosX + BALL_WIDTH > SCREEN_WIDTH)
    {
        mPosX = SCREEN_WIDTH - BALL_WIDTH;
        mVelX = -mVelX;
    }

    // Kiểm tra va chạm biên theo trục Y
    if (mPosY < 0)
    {
        mPosY = 0;
        // n = (0, 1) => v' = ( v_x, -v_y )
        mVelY = -mVelY;
    }
    else if (mPosY + BALL_HEIGHT > SCREEN_HEIGHT)
    {
        mPosY = SCREEN_HEIGHT - BALL_HEIGHT;
        mVelY = -mVelY;
    }

    // Cập nhật lại collider nếu cần
    shiftColliders();
}

void Ball::render(LTexture &gBallTexture)
{
    // Show the dot
    gBallTexture.render(mPosX, mPosY);
}

void Ball::shiftColliders()
{
    // Align collider to center of dot
    mCollider.x = mPosX;
    mCollider.y = mPosY;
}

bool Ball::isStop()
{
    return !isFollowing;
    return mVelX == 0 && mVelY == 0;
}

Circle &Ball::getCollider()
{
    return mCollider;
}

#endif