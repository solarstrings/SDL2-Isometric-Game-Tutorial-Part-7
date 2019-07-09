#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../../logger.h"
#include "../Scene/scene.h"
#include "component_nametag.h"

componentNameTagT *componentNewNameTag()
{
    int i=0;
    componentNameTagT *nameTag = malloc(sizeof(struct componentNameTagT)*NUM_INITIAL_ENTITIES);

    if(nameTag == NULL){
        //write the error to the logfile
        writeToLog("Error in Function componentNewNameTag() - Could not allocate memory for name tag components! ","runlog.txt");
        return NULL;
    }

    //loop through all the entities
    for(i=0;i<NUM_INITIAL_ENTITIES;++i){
        //set the name tag to NULL
        nameTag->name = NULL;
    }
    //return the name tag
    return nameTag;
}
void componentNameTagAllocateMoreMemory(sceneT *scene,int componentIndex)
{
    int j=0;
    componentNameTagT *newComponentNameTag = realloc((componentNameTagT*)scene->components[componentIndex].data,sizeof(struct componentNameTagT)*scene->maxEntities);
    if(newComponentNameTag == NULL){
        //write the error to the logfile
        writeToLog("Error in Function componentNameTagAllocateMoreMemory() - Could not allocate more memory for name tag components! ","runlog.txt");
        //mark that memory allocation failed
        scene->memallocFailed = 1;
        return;
    }
    //initialize the component for the new entities
    for(j=scene->numEntities;j<scene->maxEntities;++j){
        newComponentNameTag[j].name = NULL;
    }
    //point the data pointer to the new data
    scene->components[componentIndex].data = newComponentNameTag;
}

void componentNameTagSetName(componentNameTagT *nameTagComponent,Uint32 entity,char *name)
{
    int length = 0;
    char msg[512];
    //if the nametag exist
    if(nameTagComponent==NULL)
    {
        //write the error to the logfile
        writeToLog("Error in Function componentNameTagSetName() - Parameter: 'componentNameTagT *nameTagComponent' is NULL ","runlog.txt");
        return;
    }
    //if the provided name is not NULL
    if(name==NULL)
    {
        //write the error to the logfile
        writeToLog("Error in Function componentNameTagSetName() - Parameter: 'char *name' is NULL ","runlog.txt");
        return;
    }
    //if the name is allocated, then the user is changing the name.
    if(nameTagComponent[entity].name != NULL){
        //free it
        free(nameTagComponent[entity].name);
    }

    //get the length of the name
    length = strlen(name)+1;

    //allocate memory for the name
    nameTagComponent[entity].name = malloc(sizeof(char)*length);

    //if the memory allocation failed
    if(nameTagComponent[entity].name == NULL){
        //write the error to the logfile
        sprintf(msg,"Error in Function componentNameTagSetName() - Could not allocate memory for name:%s on entity:%d",name,entity);
        writeToLog(msg,"runlog.txt");
        return;
    }
    //set the name
    sprintf(nameTagComponent[entity].name,"%s",name);
}

void componentNameTagGetEntityNameTag(componentNameTagT *nameTagComponent,Uint32 entity,char *tagName)
{
    char msg[512];
    if(nameTagComponent == NULL)
    {
        sprintf(msg,"Error in Function componentNameTagGetEntityNameTag() - Parameter: 'componentNameTagT *nameTagComponent' is NULL!");
        writeToLog(msg,"runlog.txt");
    }
    if(tagName == NULL){
        sprintf(msg,"Error in Function componentNameTagGetEntityNameTag() - Parameter: 'char *tagName' is NULL!");
        writeToLog(msg,"runlog.txt");
    }

    //if there is no name on the component
    if(nameTagComponent[entity].name==NULL){
        //simply write an end line character to the string
        tagName[0] = '\0';
    }
    //otherwise
    else{
        //write the name tag to you tagName pointer
        sprintf(tagName,"%s",nameTagComponent[entity].name);
    }
}

void componentFreeNameTag(componentNameTagT *nameTagComponent,Uint32 maxEntities)
{
    Uint32 i=0;
    //if the position component is not NULL
    if(nameTagComponent!=NULL){
        for(i=0;i<maxEntities;++i)
        {
            //if the name is not NULL
            if(nameTagComponent[i].name!=NULL){
                //free the name
                free(nameTagComponent[i].name);
            }
        }
        //free the tag component
        free(nameTagComponent);
    }
}

int componentNameTagGetEntityIDFromEntityByName(componentNameTagT *nameTagComponents,char *entityName,Uint32 numEntities)
{
    Uint32 i=0;
    char msg[512];
    if(nameTagComponents == NULL)
    {
        sprintf(msg,"Error in Function componentNameTagGetEntityIDFromEntityByName() - Parameter: 'componentNameTagT *nameTagComponents' is NULL!");
        writeToLog(msg,"runlog.txt");
        return -1;
    }
    if(entityName == NULL)
    {
        sprintf(msg,"Error in Function componentNameTagGetEntityIDFromEntityByName() - Parameter: 'char *entityName' is NULL!");
        writeToLog(msg,"runlog.txt");
        return -1;
    }

    for(i=0;i<numEntities;++i)
    {
        //make sure that the name is not a NULL pointer
        if(nameTagComponents[i].name!=NULL)
        {
            //if the name tag of the component matches the entity name
            if(strcmp(nameTagComponents[i].name,entityName)==0)
            {
                //return the index
                return i;
            }
        }
    }
    return -1;
}
