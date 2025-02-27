#ifndef LTIMER_H
#define LTIMER_H

#include <SDL.h>

class LTimer
{
    public:
        LTimer();
        void start();
        void stop();
        void pause();
        void unpause();
        Uint32 getTicks();
        bool isStarted();
        bool isPaused();

    private:
        Uint32 mStartTicks;
        Uint32 mPausedTicks;
        bool mPaused;
        bool mStarted;
};

LTimer::LTimer()
{
    mStartTicks = 0;
    mPausedTicks = 0;
    mPaused = false;
    mStarted = false;
}

void LTimer::start()
{
    mStarted = true;
    mPaused = false;
    mStartTicks = SDL_GetTicks();
    mPausedTicks = 0;
}

void LTimer::stop()
{
    mStarted = false;
    mPaused = false;
    mStartTicks = 0;
    mPausedTicks = 0;
}

void LTimer::pause()
{
    if (mStarted && !mPaused)
    {
        mPaused = true;
        mPausedTicks = SDL_GetTicks() - mStartTicks;
        mStartTicks = 0;
    }
}

void LTimer::unpause()
{
    if (mStarted && mPaused)
    {
        mPaused = false;
        mStartTicks = SDL_GetTicks() - mPausedTicks;
        mPausedTicks = 0;
    }
}

Uint32 LTimer::getTicks()
{
    if (mStarted)
    {
        if (mPaused)
            return mPausedTicks;
        else
            return SDL_GetTicks() - mStartTicks;
    }
    return 0;
}

bool LTimer::isStarted() { return mStarted; }
bool LTimer::isPaused() { return mPaused && mStarted; }


#endif
