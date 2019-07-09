#ifndef __TIMER_H_
#define __TIMER_H_

typedef struct timerT
{
    float currentTime;     //current time
    float timeLog;         //point in time to count from
    float timeDuration;    //how long shall the timer count
}timerT;

//this function initializes the timer and set how long it shall wait in milliseconds
void timerInit(timerT *timer, int ms);

//this function updates the timer and resets it when it comes to its end.
int timerUpdate(timerT *timer);

#endif // __TIMER_H_
