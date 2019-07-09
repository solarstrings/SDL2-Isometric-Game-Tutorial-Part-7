#ifndef __COMPONENT_VELOCITY_H_
#define __COMPONENT_VELOCITY_H_
#include <SDL2/SDL.h>

//forward declaration of sceneT, allows us to use the sceneT without causing a cross-referencing header error
typedef struct sceneT sceneT;

typedef struct componentVelocityT
{
    float x;            //x velocity
    float y;            //y velocity
    int maxVelocity;    //max velocity
    float friction;     //friction for the velocity
}componentVelocityT;

componentVelocityT *componentNewVelocity();
void componentVelocityAllocateMoreMemory(sceneT *scene,int componentIndex);
void componentFreeVelocity(componentVelocityT *positionVelocity);
void componentVelocitySetMaxVelocity(componentVelocityT *velocityComponent,Uint32 entity,int maxVelocity);
void componentVelocitySetFriction(componentVelocityT *velocityComponent,Uint32 entity,float friction);
void componentVelocitySetVelocity(componentVelocityT *velocityComponents,Uint32 entity,float x,float y);
#endif // __COMPONENT_VELOCITY_H_

