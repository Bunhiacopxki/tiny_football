#ifndef KICKMETER_H
#define KICKMETER_H

#include <SDL.h>
#include <SDL_ttf.h>
#include "LTexture.hpp"
#include "Ball.hpp"
class KickMeter
{
public:
    KickMeter();
    void start();                                   // Bắt đầu thanh chạy
    void update(Ball ball);                         // Cập nhật vị trí mũi tên
    void render(SDL_Renderer *renderer, Ball ball); // Vẽ lên màn hình
    int stop();                                     // Dừng thanh và trả về vị trí (0-4)

private:
    int arrowX;    // Vị trí mũi tên trên thanh
    int direction; // Hướng di chuyển của mũi tên
};

const int METER_X = 500;     // Vị trí bắt đầu thanh
const int METER_Y = 600;     // Vị trí Y
const int METER_WIDTH = 300; // Độ dài thanh
const int METER_HEIGHT = 20;
const int ARROW_WIDTH = 10; // Kích thước mũi tên

KickMeter::KickMeter()
{
    arrowX = METER_X;
    direction = 1; // 1: đi sang phải, -1: đi sang trái
}

void KickMeter::start()
{
    arrowX = METER_X;
    direction = 1;
}

void KickMeter::update(Ball ball)
{
    if (!ball.isWaitingForKick)
        return;

    arrowX += direction * 5; // Di chuyển mũi tên
    // Đảo hướng nếu chạm biên
    if (arrowX <= METER_X || arrowX >= METER_X + METER_WIDTH - ARROW_WIDTH)
    {
        direction *= -1;
    }
}

void KickMeter::render(SDL_Renderer *renderer, Ball ball)
{
    if (!ball.isWaitingForKick)
        return;
    // Màu sắc cho 5 vùng
    SDL_Color colors[5] = {
        {0, 250, 0, 255}, // Xanh lá rất nhạt
        {0, 200, 0, 255}, // Xanh lá nhạt
        {0, 150, 0, 255}, // Xanh lá trung bình
        {0, 100, 0, 255}, // Xanh lá đậm
        {0, 50, 0, 255}   // Xanh lá rất đậm
    };

    int sectionWidth = METER_WIDTH / 5;

    // Vẽ 5 vùng với màu sắc khác nhau
    for (int i = 0; i < 5; i++)
    {
        SDL_SetRenderDrawColor(renderer, colors[i].r, colors[i].g, colors[i].b, colors[i].a);
        SDL_Rect section = {METER_X + i * sectionWidth, METER_Y, sectionWidth, METER_HEIGHT};
        SDL_RenderFillRect(renderer, &section);
    }

    // Vẽ mũi tên
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect arrow = {arrowX, METER_Y - 10, ARROW_WIDTH, METER_HEIGHT + 20};
    SDL_RenderFillRect(renderer, &arrow);

    // Vẽ số 0 - 20 - 40 - 60 - 80 - 100
    TTF_Font *font = TTF_OpenFont("arial.ttf", 16);
    if (font)
    {
        SDL_Color textColor = {255, 255, 255, 255};
        char textBuffer[4];

        for (int i = 0; i <= 5; i++)
        {
            sprintf(textBuffer, "%d", i * 20); // 0 - 20 - 40 - 60 - 80 - 100
            SDL_Surface *textSurface = TTF_RenderText_Solid(font, textBuffer, textColor);
            SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_Rect textRect = {METER_X + i * sectionWidth - 10, METER_Y + METER_HEIGHT + 5, 20, 20};

            SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture);
        }

        TTF_CloseFont(font);
    }
}

int KickMeter::stop()
{
    int range = (arrowX - METER_X) / (METER_WIDTH / 5); // Chia thanh thành 5 vùng
    return range;                                       // Trả về vùng 0-4
}

#endif
