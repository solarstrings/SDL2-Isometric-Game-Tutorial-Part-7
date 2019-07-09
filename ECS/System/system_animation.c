#include <stdio.h>
#include "system.h"
#include "system_collision.h"
#include "../../logger.h"
#include "../../deltatimer.h"
#include "../Scene/scene.h"
#include "../Component/component.h"
#include "../../IsoEngine/isoEngine.h"
#include "system_animation.h"

#define SYSTEM_ANIMATION_MASK_SET1 (COMPONENT_SET1_ANIMATION)

//local global pointer to the scene
static sceneT *scn = NULL;

//local global pointer to the data
static componentAnimationT *animComponents = NULL;
static componentRender2DT *renderComponents = NULL;

//local global variable for system failure
static int systemFailedToInitialize = 1;

static void updateComponentPointers()
{
    //if there is no scene
    if(scn == NULL){
        //return out of the function
        return;
    }
    //Error handling is done in the scene.c file.

    //get the render 2D component pointer
    animComponents = (componentAnimationT*)ecsSceneGetComponent(scn,COMPONENT_SET1_ANIMATION);

    //get the render 2D component pointer
    renderComponents = (componentRender2DT*)ecsSceneGetComponent(scn,COMPONENT_SET1_RENDER2D);
}

int systemAnimationInit(void *scene)
{
    systemFailedToInitialize = 0;

    writeToLog("Initializing collision system...","runlog.txt");

    if(scene == NULL){
        writeToLog("Error in Function systemAnimationInit() - Animation system failed to initialize: Parameter 'void *scene' is NULL ","runlog.txt");
        systemFailedToInitialize = 1;
        return 0;
    }

    scn = (sceneT*)scene;

    //check if the scene has collision components
    animComponents = (componentAnimationT*)ecsSceneGetComponent(scn,COMPONENT_SET1_ANIMATION);
    //if not
    if(animComponents == NULL)
    {
        //log the error
        writeToLog("Error in Function systemAnimationInit() - Animation system failed to initialize: The scene does not have animation components (COMPONENT_SET1_ANIMATION)","runlog.txt");
        systemFailedToInitialize = 1;
        return 0;
    }
/*
    //check if the scene has render 2D components
    renderComponents = (componentRender2DT*)ecsSceneGetComponent(scn,COMPONENT_SET1_RENDER2D);
    //if not
    if(renderComponents == NULL)
    {
        //log the error
        writeToLog("Error in Function systemAnimationInit() - Collision system failed to initialize: The scene does not have render 2D components (COMPONENT_SET1_RENDER2D)","runlog.txt");
        systemFailedToInitialize = 1;
        return 0;
    }
    writeToLog("Initializing Collision System... DONE","runlog.txt"); */
    //return 1, successfully initialized the system
    return 1;
}

void systemAnimationUpdate()
{
    //if the system failed to initialize
    if(systemFailedToInitialize == 1){
        return;
    }
    //if the component pointers has been reallocated in the scene
    if(scn->componentPointersRealloced == 1){
        updateComponentPointers();
    }
}

void systemAnimationUpdateEntity(Uint32 entity)
{
    int currentFrameIndex = 0;

    //if the system failed to initialize
    if(systemFailedToInitialize == 1){
        return;
    }
    //if the entity has the animation component
    if(scn->entities[entity].componentSet1 & SYSTEM_ANIMATION_MASK_SET1){
        //if the animation has any animations
        if(animComponents[entity].numAnimations >0)
        {
            //if it is time to go to the next frame
            if(timerUpdate(&animComponents[entity].animations[animComponents[entity].animationState].frameTime))
            {
                //go to the next frame
                animComponents[entity].animations[animComponents[entity].animationState].currentFrame++;

                //if the animation has reached its end
                if(animComponents[entity].animations[animComponents[entity].animationState].currentFrame >=
                   animComponents[entity].animations[animComponents[entity].animationState].numFrames)
                {
                    //reset the frame to the first one
                    animComponents[entity].animations[animComponents[entity].animationState].currentFrame = 0;
                }
                //copy the frame index, (for easier readability of the code on the timerInit row below)
                currentFrameIndex = animComponents[entity].animations[animComponents[entity].animationState].currentFrame;

                //set the timer duration to the frame's frame time
                timerInit(&animComponents[entity].animations[animComponents[entity].animationState].frameTime,
                animComponents[entity].animations[animComponents[entity].animationState].frames[currentFrameIndex].frameTimeMilliSeconds);
            }
        }
    }
}

void systemAnimationFree(){
    //the system is not allocation anything, so there is nothing to free
}
