#include "SDL2/SDL.h"
#include "timer.h"

//this function initializes the timer and set how long it shall wait in milliseconds
void timerInit(timerT *timer,int ms)
{
    timer->timeLog = SDL_GetTicks();
    timer->timeDuration = ms;
}

//this function updates the timer and resets it when it comes to its end.
int timerUpdate(timerT *timer)
{
    //get current time
    timer->currentTime = SDL_GetTicks();

    //if the timer has passed its duration
    if(timer->currentTime >= timer->timeLog + timer->timeDuration)
    {
        //set the time log position to what the clock is now and count from that.
        timer->timeLog = timer->currentTime;

        //return 1 if the timer has reached a new cycle
        return 1;
    }
    //return false if the duration has not passed
    return 0;
}
