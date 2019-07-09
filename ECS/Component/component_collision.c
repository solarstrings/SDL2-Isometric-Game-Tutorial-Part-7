#include <stdio.h>
#include "component_collision.h"
#include "../../logger.h"
#include "../Scene/scene.h"

componentCollisionT *componentNewCollision()
{
    Uint32 i=0;
    //allocate memory for new collision components
    componentCollisionT *newCollisionComponent = malloc(sizeof(struct componentCollisionT)*NUM_INITIAL_ENTITIES);

    //if memory allocation failed
    if(newCollisionComponent == NULL)
    {
        writeToLog("Error in function componentNewCollision() - Could not allocate memory for collision components","runlog.txt");
        return NULL;
    }
    //loop through all entities
    for(i=0;i<NUM_INITIAL_ENTITIES;++i)
    {
        //set collision type to deactivated
        newCollisionComponent[i].collisionType = COLLISIONTYPE_DEACTIVATED;
        //create a default collision rectangle
        setupRect(&newCollisionComponent[i].rect,0,0,5,5);
        //set the is colliding flag to 0
        newCollisionComponent[i].isColliding = 0;
    }
    //return the pointer to the collision components
    return newCollisionComponent;
}

void componentCollisionAllocateMoreMemory(sceneT *scene,int componentIndex)
{
    int j=0;
    componentCollisionT *newComponentCollision = realloc((componentCollisionT*)scene->components[componentIndex].data,sizeof(struct componentCollisionT)*scene->maxEntities);
    if(newComponentCollision == NULL){
        //write the error to the logfile
        writeToLog("Error in Function componentCollisionAllocateMoreMemory() - Could not allocate more memory for collision components! ","runlog.txt");
        //mark that memory allocation failed
        scene->memallocFailed = 1;
        return;
    }
    //initialize the component for the new entities
    for(j=scene->numEntities;j<scene->maxEntities;++j){
        newComponentCollision[j].collisionType = COLLISIONTYPE_DEACTIVATED;
        setupRect(&newComponentCollision[j].rect,0,0,5,5);
        newComponentCollision[j].isColliding = 0;
    }
    //point the data pointer to the new data
    scene->components[componentIndex].data = newComponentCollision;
}

void componentCollisionSetCollisionType(componentCollisionT *collisionComponent,Uint32 entity,collisionTypeE collisionType)
{
    if(collisionComponent != NULL)
    {
        collisionComponent[entity].collisionType = collisionType;
    }
    else
    {
        writeToLog("Error in function componentCollisionSetCollisionType() - Parameter 'componentCollisionT *collisionComponent' is NULL","runlog.txt");
    }
}

void componentCollisionSetCollisionRectange(componentCollisionT *collisionComponent,Uint32 entity,SDL_Rect *collisionRect)
{
    //make sure that the pointers are not NULL
    if(collisionComponent == NULL)
    {
        writeToLog("Error in function componentCollisionSetCollisionRectange() - Parameter 'componentCollisionT *collisionComponent' is NULL","runlog.txt");
        return;
    }
    else if(collisionRect == NULL)
    {
        writeToLog("Error in function componentCollisionSetCollisionRectange() - Parameter 'SDL_Rect *collisionRect' is NULL","runlog.txt");
        return;
    }
    //dereference the collision rectangle pointer and set the collision rectangle value
    collisionComponent[entity].rect = *collisionRect;
}

void componentFreeCollision(componentCollisionT *collisionComponent)
{
    if(collisionComponent!=NULL)
    {
        free(collisionComponent);
    }
}
