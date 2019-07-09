#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "component_inputmouse.h"
#include "../../logger.h"
#include "../Scene/scene.h"

componentInputMouseT *componentNewInputMouse()
{
    int i=0;
    componentInputMouseT *newMouseComponent = malloc(sizeof(struct componentInputMouseT)*NUM_INITIAL_ENTITIES);
    if(newMouseComponent == NULL){
        writeToLog("Error in componentNewInputMouse() - Could not allocate memory for mouse input component!","runlog.txt");
        return NULL;
    }
    //loop through all the entities
    for(i=0;i<NUM_INITIAL_ENTITIES;++i){
        newMouseComponent[i].actions = NULL;        //set actions to NULL
        newMouseComponent[i].numActions = 0;        //number of actions
        newMouseComponent[i].maxActions = 0;        //current max actions
        newMouseComponent[i].active = 0;            //if this component is active
    }
    return newMouseComponent;
}

void componentInputMouseAllocateMoreMemory(sceneT *scene,int componentIndex)
{
    int j=0;
    componentInputMouseT *newComponentInputMouse = realloc((componentInputMouseT*)scene->components[componentIndex].data,sizeof(struct componentInputMouseT)*scene->maxEntities);
    if(newComponentInputMouse == NULL){
        //write the error to the logfile
        writeToLog("Error in Function componentInputMouseAllocateMoreMemory() - Could not allocate more memory for input mouse components! ","runlog.txt");
        //mark that memory allocation failed
        scene->memallocFailed = 1;
        return;
    }
    //initialize the component for the new entities
    for(j=scene->numEntities;j<scene->maxEntities;++j){
        newComponentInputMouse[j].actions = NULL;
        newComponentInputMouse[j].active = 0;
        newComponentInputMouse[j].maxActions = 0;
        newComponentInputMouse[j].numActions = 0;
    }
    //point the data pointer to the new data
    scene->components[componentIndex].data = newComponentInputMouse;
}
void componentFreeInputMouse(componentInputMouseT *componentInputMouse,int maxEntities)
{
    int i=0,j=0;
    if(componentInputMouse !=NULL)
    {
        //if actions have been added
        if(componentInputMouse->actions!=NULL)
        {
            //loop through the current number of entities
            for(j=0;j<maxEntities;++j)
            {
                //loop through the actions
                for(i=0;i<componentInputMouse[j].numActions;++i)
                {
                    //if a name was given to an action
                    if(componentInputMouse[j].actions[i].name!=NULL)
                    {
                        //free the allocated memory
                        free(componentInputMouse[j].actions[i].name);
                    }
                }
            }
            //free the actions
            free(componentInputMouse->actions);
        }
        //free the input mouse component
        free(componentInputMouse);
    }
}

void componentInputMouseAddAction(componentInputMouseT *componentInputMouse,int entity,char *name,inputMouseActionE mouseAction)
{
    char msg[512];
    int actionIndex=-1;
    inputMouseActionT *newMouseAction = NULL;
    if(componentInputMouse == NULL){
        sprintf(msg,"Warning in componentInputMouseAddAction() - Parameter: 'componentInputMouseT *componentInputMouse' is NULL. Action:%s is not added!",name);
        writeToLog(msg,"runlog.txt");
        return;
    }

    //The mouse functions differently from the
    //if the action already exist
    actionIndex = componentInputMouseGetActionIndex(componentInputMouse,entity,name);
    if(actionIndex!=-1){
        sprintf(msg,"Warning in componentInputMouseAddAction() - Action:%s already exist, action is not added!",name);
        writeToLog(msg,"runlog.txt");
        return;
    }

    //if memory needs to be allocated
    if(componentInputMouse[entity].numActions >= componentInputMouse[entity].maxActions)
    {
        //if there are no actions
        if(componentInputMouse[entity].actions == NULL){
            //allocate memory for five actions
            componentInputMouse[entity].maxActions+=5;
            componentInputMouse[entity].actions = malloc(sizeof(struct inputMouseActionT)*componentInputMouse[entity].maxActions);
            if(componentInputMouse[entity].actions == NULL){
                sprintf(msg,"Warning in componentInputMouseAddAction() - Could not allocate memory for a new Action. Action:%s was not added to entity:%d!",name,entity);
                writeToLog(msg,"runlog.txt");
                return;
            }
        }
        //if there are previous actions, re-allocate more memory
        else{
            //allocate memory for five more actions
            componentInputMouse[entity].maxActions+=5;
            newMouseAction = realloc(componentInputMouse[entity].actions,sizeof(struct inputMouseActionT)*componentInputMouse[entity].maxActions);
            if(newMouseAction == NULL){
                sprintf(msg,"Warning in componentInputMouseAddAction() - Could not re-allocate more memory for mouse actions. Action:%s was not added!",name);
                writeToLog(msg,"runlog.txt");
                return;
            }
            //point the actions to the new memory
            componentInputMouse[entity].actions = newMouseAction;
        }
    }


    //add the mouse action
    //allocate memory for the name
    componentInputMouse[entity].actions[componentInputMouse[entity].numActions].name = malloc(sizeof(char)*strlen(name)+1);
    //copy the name
    sprintf(componentInputMouse[entity].actions[componentInputMouse[entity].numActions].name,"%s",name);
    //set the mouse action
    componentInputMouse[entity].actions[componentInputMouse[entity].numActions].mouseAction = mouseAction;

    //set the mouse action states
    componentInputMouse[entity].actions[componentInputMouse[entity].numActions].state = COMPONENT_INPUTMOUSE_STATE_RELEASED;
    componentInputMouse[entity].actions[componentInputMouse[entity].numActions].oldState = COMPONENT_INPUTMOUSE_STATE_RELEASED;

    //Update number of actions
    componentInputMouse[entity].numActions++;

}

int componentInputMouseGetActionIndex(componentInputMouseT *componentInputMouse,Uint32 entity,char *actionName)
{
    int i=0;
    //if the keyboard input component is valid
    if(componentInputMouse != NULL)
    {
        //loop through the input keyboard actions of the entity
        for(i=0;i<componentInputMouse[entity].numActions;++i)
        {
            //if the name matches
            if(strcmp(actionName,componentInputMouse[entity].actions[i].name)==0)
            {
                //return the index
                return i;
            }
        }
    }
    return -1;
}

void componentInputMouseSetActiveState(componentInputMouseT *componentInputMouse,Uint32 entity,int value)
{
    char msg[512];
    if(componentInputMouse !=NULL)
    {
        //if the value is 1 or 0
        if(value==1 || value == 0){
            componentInputMouse[entity].active = value;
        }
        else{
            sprintf(msg,"Warning in componentInputMouseSetActiveState() - Cannot set state for entity:%d, parameter:'int value' must be 1 or 0. Invalid passed value was:%d",entity,value);
            writeToLog(msg,"runlog.txt");
        }
    }
}

