#ifndef __COMPONENT_POSITION_H_
#define __COMPONENT_POSITION_H_
#include <SDL2/SDL.h>

//forward declaration of sceneT, allows us to use the sceneT without causing a cross-referencing header error
typedef struct sceneT sceneT;

typedef struct componentPositionT
{
    float x;        //x position
    float y;        //y position
    float oldx[3];  //old x position history 3 frames
    float oldy[3];  //old y position history 3 frames
    float xOffset;  //x base, used to define where on a object its base is
    float yOffset;  //y base, used to define where on a object its base is
}componentPositionT;

componentPositionT *componentNewPosition();
void componentFreePosition(componentPositionT *positionComponent);
void componentPositionAllocateMoreMemory(sceneT *scene,int componentIndex);
void componentPositionSetOffset(componentPositionT *positionComponent,Uint32 entity,float x,float y);
void componentPositionSetPosition(componentPositionT *positionComponent,Uint32 entity,float x,float y);
void componentPositionAddOldPositionToStack(componentPositionT *positionComponent, Uint32 entity);
#endif // __COMPONENT_POSITION_H_
