#ifndef __DELTA_TIMER_H_
#define __DELTA_TIMER_H_
#include <SDL2/SDL.h>

typedef struct deltaTimerT
{
    Uint32 oldTick;
    Uint32 tick;
    double deltaTime;
}deltaTimerT;

void deltaTimerInit();
void deltaTimerUpdate();
double deltaTimerGetDeltatime();

#endif // __DELTA_TIMER_H_

