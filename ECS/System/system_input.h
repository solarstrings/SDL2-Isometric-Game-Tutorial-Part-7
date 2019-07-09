#ifndef __INPUT_SYSTEM_H__
#define __INPUT_SYSTEM_H__
#include <SDL2/SDL.h>

int systemInputInit(void *scene);
void systemInputUpdate();
void systemInputUpdateEntity(Uint32 entity);
void systemFreeInputSystem();
#endif // __INPUT_SYSTEM_H__

