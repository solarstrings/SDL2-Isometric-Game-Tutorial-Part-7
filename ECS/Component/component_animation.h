#ifndef __COMPONENT_ANIMATION_H_
#define __COMPONENT_ANIMATION_H_
#include "../../texture.h"
#include "../../timer.h"
#define COMPONENT_ANIMATION_NUM_INITIAL_ANIMATIONS  2

#define ANIMATION_STATE_NONE    -1

typedef enum EntityDirectionInWorldT
{
    ENTITY_WORLD_DIRECTION_UP           = 0,
    ENTITY_WORLD_DIRECTION_DOWN         = 1,
    ENTITY_WORLD_DIRECTION_LEFT         = 2,
    ENTITY_WORLD_DIRECTION_RIGHT        = 3,
    ENTITY_WORLD_DIRECTION_UPLEFT       = 4,
    ENTITY_WORLD_DIRECTION_UPRIGHT      = 5,
    ENTITY_WORLD_DIRECTION_DOWNLEFT     = 6,
    ENTITY_WORLD_DIRECTION_DOWNRIGHT    = 7,
}EntityDirectionInWorldT;

//forward declaration of sceneT, allows us to use the sceneT struct without causing a cross-referencing header error
typedef struct sceneT sceneT;

typedef struct animationFrameT
{
    SDL_Rect clipRect;          //clip rectangle for the animation frame
    int frameTimeMilliSeconds;  //how long this frame should be displayed
}animationFrameT;

typedef struct animationT
{
    textureT *texture;          //pointer to the texture
    char *name;                 //name of the animation
    animationFrameT *frames;    //animation frames
    int numFrames;              //number of frames in the animation
    int currentFrame;           //current frame to draw
    timerT frameTime;           //frame timer
}animationT;

typedef struct componentAnimationT
{
    animationT *animations;             //pointer to the animations
    int animationState;                 //which animation to show
    EntityDirectionInWorldT direction;  //the direction the entity is facing
    int numAnimations;                  //number of animations
    int maxAnimations;                  //current max number of animations
}componentAnimationT;

componentAnimationT *componentNewAnimation();
void componentFreeAnimation(componentAnimationT *componentAnimation,int maxEntities);
void componentAnimationAllocateMoreMemory(sceneT *scene,int componentIndex);
int componentAnimationCreateNewAnimation(componentAnimationT *componentAnimation,int entity,textureT *texture,char *animationName);
int componentAnimationAddAnimationFrames(componentAnimationT *componentAnimation,int entity,int animationIndex,
                                         int width,int height,int numFrames,int startFrameIndex, int frameTimeMilliSeconds);
void componentAnimationSetAnimationFrameTime(componentAnimationT *componentAnimation,int entity,int animationIndex,int frame,int frameTimeMilliseconds);
int componentAnimationGetAnimationIndexByName(componentAnimationT *componentAnimation,int entity,char *animationName);
void componentAnimationSetAnimationState(componentAnimationT *componentAnimation,int entity,char *animationName);

#endif // __COMPONENT_ANIMATION_H_

