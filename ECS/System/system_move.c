#include "system.h"
#include "system_move.h"
#include "../../logger.h"
#include "../../deltatimer.h"
#include "../Scene/scene.h"
#include "../Component/component.h"


//define a mask for the move system. It requires a position and velocity component.
//it works on SET1 components, so we mark that as well in the define
#define SYSTEM_MOVE_MASK_SET1 (COMPONENT_SET1_POSITION | COMPONENT_SET1_VELOCITY)

#define COMPONENT_NO_INDEX -100

//local global pointers to the data
static componentPositionT *posComponents = NULL;
static componentVelocityT *velComponents = NULL;
static sceneT *scn = NULL;
//local global variable for system failure
static int systemFailedToInitialize = 1;

static void updateComponentPointers()
{
    if(scn == NULL){
        return;
    }
    //ERROR handling is done in the scene.c file for the components. If a realloc fail, the system will shut down there.

    //get the position components pointer
    posComponents = (componentPositionT*)ecsSceneGetComponent(scn,COMPONENT_SET1_POSITION);

    //get the velocity components pointer
    velComponents = (componentVelocityT*)ecsSceneGetComponent(scn,COMPONENT_SET1_VELOCITY);
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!                                                      !!!
// !!!   Always call this function when changing a scene!   !!!
// !!!                                                      !!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Function to initialize the move system
//
// This is used to ensure that the game will not crash when
// changing scenes. This is because scenes can have different indexes
// for the components.
int systemMoveInit(void *scene)
{
    systemFailedToInitialize=0;

    writeToLog("Initializing Move System...","runlog.txt");
    //if the passed entity manager is NULL
    if(scene == NULL){
        //log it as an error
        writeToLog("Error in Function systemMoveInit() - Move system failed to initialize: *scene is NULL!","runlog.txt");

        //mark that the system failed to initialize
        systemFailedToInitialize = 1;
        //exit the function
        return 0;
    }
    //typecast the void *scene to a sceneT* pointer
    scn = (sceneT*)scene;

    //check if the scene has position components
    posComponents = (componentPositionT*)ecsSceneGetComponent(scn,COMPONENT_SET1_POSITION);
    //if not
    if(posComponents == NULL){
        //log it as an error
        writeToLog("Error in Function systemMoveInit() - Move system failed to initialize: Scene does not have position components (COMPONENT_SET1_POSITION)!","runlog.txt");
        systemFailedToInitialize = 1;
        return 0;
    }

    // //check if the scene has velocity components
    velComponents = (componentVelocityT*)ecsSceneGetComponent(scn,COMPONENT_SET1_VELOCITY);
    //if not
    if(velComponents == NULL){
        //log it as an error
        writeToLog("Error in Function systemMoveInit() - Move system failed to initialize: Scene does not have velocity components (COMPONENT_SET1_VELOCITY)!","runlog.txt");
        systemFailedToInitialize = 1;
        return 0;
    }

    //flag that the initialization went ok
    systemFailedToInitialize = 0;

    //log that the move system was successfully initialized
    writeToLog("Initializing Move System... DONE!","runlog.txt");

    //return 1, successfully initialized the system
    return 1;
}

void systemMoveUpdate()
{
    if(scn == NULL){
       return;
    }
    //if the component pointers have been reallocated in the scene
    if(scn->componentPointersRealloced == 1){
        //update the local global component pointers
        updateComponentPointers();
    }
}

void systemMoveUpdateEntity(Uint32 entity)
{

    //if the move system failed to initialize
    if(systemFailedToInitialize == 1){
        //return out of the function
        return;
    }

    //if the entity has the position and velocity component
    if(scn->entities[entity].componentSet1 & SYSTEM_MOVE_MASK_SET1)
    {
        componentPositionAddOldPositionToStack(posComponents,entity);
        //update the entity position
        posComponents[entity].x += (velComponents[entity].x * deltaTimerGetDeltatime());
        posComponents[entity].y += (velComponents[entity].y * deltaTimerGetDeltatime());

        //if the x velocity is larger than 0
        if(velComponents[entity].x > 0)
        {
            //decrease speed with friction
            velComponents[entity].x-=velComponents[entity].friction;
            //if the velocity shot over to be negative
            if(velComponents[entity].x<0){
                //set the velocity to 0
                velComponents[entity].x = 0;
            }
        }

        //if the x velocity is lower than 0
        if(velComponents[entity].x < 0)
        {
            //decrease speed with friction
            velComponents[entity].x+=velComponents[entity].friction;
            //if the velocity shot over to be positive
            if(velComponents[entity].x>0){
                //set the velocity to 0
                velComponents[entity].x = 0;
            }
        }

        //if the y velocity is larger than 0
        if(velComponents[entity].y > 0)
        {
            //decrease speed with friction
            velComponents[entity].y-=velComponents[entity].friction;
            //if the velocity shot over to be negative
            if(velComponents[entity].y<0){
                //set the velocity to 0
                velComponents[entity].y = 0;
            }
        }

        //if the y velocity is lower than 0
        if(velComponents[entity].y < 0)
        {
            //decrease speed with friction
            velComponents[entity].y+=velComponents[entity].friction;
            //if the velocity shot over to be positive
            if(velComponents[entity].y>0){
                //set the velocity to 0
                velComponents[entity].y = 0;
            }
        }

        //Uncomment line to test
        //printf("\nEntity:%d posx:%.2f posy:%.2f                      ",entity,posComponents[entity].x,posComponents[entity].y);
    }
}

void systemFreeMoveSystem()
{
    //move system is not allocating anything, so we leave it empty
}
