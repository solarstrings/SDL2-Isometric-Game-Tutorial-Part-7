#include <stdlib.h>
#include "component_velocity.h"
#include "../../logger.h"
#include "../Scene/scene.h"

componentVelocityT *componentNewVelocity()
{
    Uint32 i = 0;

    //allocate memory for new velocity components
    componentVelocityT *newVelComponent = malloc(sizeof(struct componentVelocityT)*NUM_INITIAL_ENTITIES);

    //if memory allocation failed
    if(newVelComponent==NULL)
    {
        //write the error to the logfile
        writeToLog("Error in Function componentNewVelocity() - Could not allocate memory for velocity components! ","runlog.txt");
        return NULL;
    }

    //loop through all the entities
    for(i=0;i<NUM_INITIAL_ENTITIES;++i){
        //initialize variables
        newVelComponent[i].x = 0;
        newVelComponent[i].y = 0;
        newVelComponent[i].maxVelocity = 1000;
        newVelComponent[i].friction = 1;
    }

    //return the pointer to the velocity components
    return newVelComponent;
}

void componentVelocityAllocateMoreMemory(sceneT *scene,int componentIndex)
{
    int j=0;
    componentVelocityT *newComponentVelocity = realloc((componentVelocityT*)scene->components[componentIndex].data,sizeof(struct componentVelocityT)*scene->maxEntities);
    if(newComponentVelocity == NULL){
        //write the error to the logfile
        writeToLog("Error in Function componentVelocityAllocateMoreMemory() - Could not allocate more memory for velocity components! ","runlog.txt");
        //mark that memory allocation failed
        scene->memallocFailed = 1;
        return;
    }
    //initialize the component for the new entities
    for(j=scene->numEntities;j<scene->maxEntities;++j){
        newComponentVelocity[j].x = 0;
        newComponentVelocity[j].y = 0;
        newComponentVelocity[j].friction = 1;
        newComponentVelocity[j].maxVelocity = 1000;
    }
    //point the data pointer to the new data
    scene->components[componentIndex].data = newComponentVelocity;
}
void componentFreeVelocity(componentVelocityT *velocityComponent)
{
    //if the velocity component is not NULL
    if(velocityComponent!=NULL){
        //free it
        free(velocityComponent);
    }
}

void componentVelocitySetVelocity(componentVelocityT *velocityComponents,Uint32 entity,float x,float y)
{
    if(velocityComponents != NULL)
    {
        velocityComponents[entity].x =  x;
        velocityComponents[entity].y =  y;
    }
}

void componentVelocitySetMaxVelocity(componentVelocityT *velocityComponent,Uint32 entity,int maxVelocity)
{
    //if the velocity component is not NULL
    if(velocityComponent==NULL){
        //write the error to the logfile
        writeToLog("Error in Function componentVelocitySetMaxVelocity() - Parameter:'componentVelocityT *velocityComponent' is NULL.","runlog.txt");
        return;
    }

    if(maxVelocity<0){
        //write the error to the logfile
        writeToLog("Error in Function componentVelocitySetMaxVelocity() - Parameter:'int maxVelocity' cannot have a negative value. Aborting.","runlog.txt");
        return;
    }
    velocityComponent[entity].maxVelocity = maxVelocity;

}
void componentVelocitySetFriction(componentVelocityT *velocityComponent,Uint32 entity,float friction)
{
    //if the velocity component is not NULL
    if(velocityComponent==NULL){
        //write the error to the logfile
        writeToLog("Error in Function componentVelocitySetMaxVelocity() - Parameter:'componentVelocityT *velocityComponent' is NULL.","runlog.txt");
        return;
    }

    if(friction<0){
        //write the error to the logfile
        writeToLog("Error in Function componentVelocitySetMaxVelocity() - Parameter:'float friction' cannot have a negative value. Aborting.","runlog.txt");
        return;
    }
    velocityComponent[entity].friction = friction;
}
