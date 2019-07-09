#include <stdlib.h>
#include "component_position.h"
#include "../../logger.h"
#include "../Scene/scene.h"

componentPositionT *componentNewPosition()
{
    Uint32 i=0,j=0;
    //allocate memory for new position components
    componentPositionT *newPosComponent = malloc(sizeof(struct componentPositionT)*NUM_INITIAL_ENTITIES);

    //if memory allocation failed
    if(newPosComponent==NULL)
    {
        //write the error to the logfile
        writeToLog("Error in Function componentNewPosition() - Could not allocate memory for position components! ","runlog.txt");
        return NULL;
    }

    //loop through all the entities
    for(i=0;i<NUM_INITIAL_ENTITIES;++i){
        //set x & y position
        newPosComponent[i].x = -1000;
        newPosComponent[i].y = -1000;
        for(j=0;j<3;++j){
            newPosComponent[i].oldx[j] = -1000;
            newPosComponent[i].oldy[j] = -1000;
        }
        newPosComponent[i].xOffset = 0;
        newPosComponent[i].yOffset = 0;
    }

    //return the pointer to the position components
    return newPosComponent;
}

void componentPositionAllocateMoreMemory(sceneT *scene,int componentIndex)
{
    int j=0;
    componentPositionT *newComponentPosition = realloc((componentPositionT*)scene->components[componentIndex].data,sizeof(struct componentPositionT)*scene->maxEntities);
    if(newComponentPosition == NULL){
        //write the error to the logfile
        writeToLog("Error in Function componentPositionAllocateMoreMemory() - Could not allocate more memory for new position components! ","runlog.txt");
        //mark that memory allocation failed
        scene->memallocFailed = 1;
        return;
    }
    //initialize the component for the new entities
    for(j=scene->numEntities;j<scene->maxEntities;++j){
        newComponentPosition[j].x = 0;
        newComponentPosition[j].y = 0;
        newComponentPosition[j].xOffset = 0;
        newComponentPosition[j].yOffset = 0;
    }
    //point the pointer to the new data
    scene->components[componentIndex].data = newComponentPosition;
}

void componentFreePosition(componentPositionT *positionComponent)
{
    //if the position component is not NULL
    if(positionComponent!=NULL){
        //free it
        free(positionComponent);
    }
}

void componentPositionSetPosition(componentPositionT *positionComponent,Uint32 entity,float x,float y)
{
    if(positionComponent!=NULL){
        positionComponent[entity].x = x;
        positionComponent[entity].y = y;
    }
    else{
        //write the error to the logfile
        writeToLog("Error in Function componentPositionSetPosition() - Cannot set position, parameter: 'componentPositionT *positionComponent' is NULL!","runlog.txt");
        return;
    }
}


void componentPositionSetOffset(componentPositionT *positionComponent,Uint32 entity,float x,float y)
{
    if(positionComponent!=NULL){
        positionComponent[entity].xOffset = x;
        positionComponent[entity].yOffset = y;
    }
    else{
        //write the error to the logfile
        writeToLog("Error in Function componentPositionSetOffset() - Cannot set position offset, parameter: 'componentPositionT *positionComponent' is NULL!","runlog.txt");
        return;
    }
}

void componentPositionAddOldPositionToStack(componentPositionT *positionComponent, Uint32 entity)
{
    if(positionComponent!=NULL){
        positionComponent[entity].oldx[2] = positionComponent[entity].oldx[1];
        positionComponent[entity].oldx[1] = positionComponent[entity].oldx[0];
        positionComponent[entity].oldx[0] = positionComponent[entity].x;

        positionComponent[entity].oldy[2] = positionComponent[entity].oldy[1];
        positionComponent[entity].oldy[1] = positionComponent[entity].oldy[0];
        positionComponent[entity].oldy[0] = positionComponent[entity].y;
    }
    else{
        //write the error to the logfile
        writeToLog("Error in Function componentPositionAddOldPositionToStack() - parameter: 'componentPositionT *positionComponent' is NULL!","runlog.txt");
        return;
    }
}
