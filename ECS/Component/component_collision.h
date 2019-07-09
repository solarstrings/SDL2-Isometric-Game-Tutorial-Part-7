#ifndef __COMPONENT_COLLISION_H_
#define __COMPONENT_COLLISION_H_
#include <SDL2/SDL.h>
#include "../../IsoEngine/isoEngine.h"

//forward declaration of sceneT, allows us to use the sceneT struct without causing a cross-referencing header error
typedef struct sceneT sceneT;

typedef enum collisionTypeE
{
    COLLISIONTYPE_DEACTIVATED       = 0,
    COLLISIONTYPE_WORLD             = 1,
    COLLISIONTYPE_ENTITY            = 2,
    COLLISIONTYPE_WORLD_AND_ENTITY  = 3,
}collisionTypeE;

typedef struct componentCollisionT
{
    collisionTypeE collisionType;   //which collisions to apply to the entity
    SDL_Rect rect;                  //collision rectangle
    SDL_Rect worldRect;             //collision rectangle in world coordinates
    short isColliding;              //flag to mark that there was a collision
}componentCollisionT;

componentCollisionT *componentNewCollision();
void componentCollisionAllocateMoreMemory(sceneT *scene,int componentIndex);
void componentFreeCollision(componentCollisionT *collisionComponent);
void componentCollisionSetCollisionType(componentCollisionT *collisionComponent,Uint32 entity,collisionTypeE collisionType);
void componentCollisionSetCollisionRectange(componentCollisionT *collisionComponent,Uint32 entity,SDL_Rect *collisionRect);
#endif // __COMPONENT_COLLISION_H_
