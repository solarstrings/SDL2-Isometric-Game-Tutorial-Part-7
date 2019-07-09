#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "component_inputkeyboard.h"
#include "../../logger.h"
#include "../Scene/scene.h"

componentInputKeyboardT *componentNewInputKeyboard()
{
    int i=0;
    componentInputKeyboardT *newKeyboardComponent = malloc(sizeof(struct componentInputKeyboardT)*NUM_INITIAL_ENTITIES);
    if(newKeyboardComponent == NULL){
        writeToLog("Error in componentNewInputKeyboard() - Could not allocate memory for keyboard input component!","runlog.txt");
        return NULL;
    }
    for(i=0;i<NUM_INITIAL_ENTITIES;++i){
        newKeyboardComponent[i].actions = NULL;     //set actions to NULL
        newKeyboardComponent[i].numActions = 0;     //number of actions
        newKeyboardComponent[i].maxActions = 0;     //current max number of actions
        newKeyboardComponent[i].active = 0;         //the component state is set to not active
        newKeyboardComponent[i].previousActions[0] = -1;//previous keyboard action
        newKeyboardComponent[i].previousActions[1] = -1;//previous keyboard action
        timerInit(&newKeyboardComponent[i].actionRelease,150);
    }

    return newKeyboardComponent;
}

void componentInputKeyboardAllocateMoreMemory(sceneT *scene,int componentIndex)
{
    int j=0;
    componentInputKeyboardT *newComponentInputKeyboard = realloc((componentInputKeyboardT*)scene->components[componentIndex].data,
                                                                 sizeof(struct componentInputKeyboardT)*scene->maxEntities);
    if(newComponentInputKeyboard == NULL){
        //write the error to the logfile
        writeToLog("Error in Function componentInputKeyboardAllocateMoreMemory() - Could not allocate more memory for input keyboard components! ","runlog.txt");
        //mark that memory allocation failed
        scene->memallocFailed = 1;
        return;
    }

    //initialize the component for the new entities
    for(j=scene->numEntities;j<scene->maxEntities;++j){
        newComponentInputKeyboard[j].actions = NULL;
        newComponentInputKeyboard[j].active = 0;
        newComponentInputKeyboard[j].maxActions = 0;
        newComponentInputKeyboard[j].numActions = 0;
        newComponentInputKeyboard[j].previousActions[0] = -1;
        newComponentInputKeyboard[j].previousActions[1] = -1;
        timerInit(&newComponentInputKeyboard[j].actionRelease,150);
    }
    //point the data pointer to the new data
    scene->components[componentIndex].data = newComponentInputKeyboard;
}

void componentInputKeyboardAddAction(componentInputKeyboardT *componentInputKeyboard,int entity,char *name,SDL_Scancode scanCode)
{
    char msg[512];
    int actionIndex=-1;
    inputKeyboardActionT *newKeyboardAction = NULL;
    if(componentInputKeyboard == NULL){
        sprintf(msg,"Warning in componentInputKeyboardAddAction() - Parameter: componentInputKeyboard is NULL. Action:%s is not added!",name);
        writeToLog(msg,"runlog.txt");
        return;
    }
    //if the action already exist
    actionIndex = componentInputKeyboardGetActionIndex(componentInputKeyboard,entity,name);
    if(actionIndex!=-1){
        //change the key scan code for the action
        componentInputKeyboard[entity].actions[actionIndex].scanCode = scanCode;
        //return out of the function
        return;
    }

    //if memory needs to be allocated
    if(componentInputKeyboard[entity].numActions >= componentInputKeyboard[entity].maxActions)
    {
        //there are no actions
        if(componentInputKeyboard[entity].actions == NULL){
            //allocate memory for five actions
            componentInputKeyboard[entity].maxActions+=5;
            componentInputKeyboard[entity].actions = malloc(sizeof(struct inputKeyboardActionT)*componentInputKeyboard[entity].maxActions);
            if(componentInputKeyboard[entity].actions == NULL){
                sprintf(msg,"Warning in componentInputKeyboardAddAction() - Could not allocate memory for a new Action. Action:%s was not added to entity:%d!",name,entity);
                writeToLog(msg,"runlog.txt");
                return;
            }
        }
        //if there are previous actions, re-allocate more memory
        else{
            //allocate memory for five more actions
            componentInputKeyboard[entity].maxActions+=5;
            newKeyboardAction = realloc(componentInputKeyboard[entity].actions,sizeof(struct inputKeyboardActionT)*componentInputKeyboard[entity].maxActions);
            if(newKeyboardAction == NULL){
                sprintf(msg,"Warning in componentInputKeyboardAddAction() - Could not re-allocate more memory for keyboard actions. Action:%s was not added!",name);
                writeToLog(msg,"runlog.txt");
                return;
            }
            //point the actions to the new memory
            componentInputKeyboard[entity].actions = newKeyboardAction;
        }
    }

    //add the keyboard action
    //allocate memory for the name
    componentInputKeyboard[entity].actions[componentInputKeyboard[entity].numActions].name = malloc(sizeof(char)*strlen(name)+1);
    //copy the name
    sprintf(componentInputKeyboard[entity].actions[componentInputKeyboard[entity].numActions].name,"%s",name);
    //set the keyboard scan code
    componentInputKeyboard[entity].actions[componentInputKeyboard[entity].numActions].scanCode = scanCode;

    //set the key states
    componentInputKeyboard[entity].actions[componentInputKeyboard[entity].numActions].state = COMPONENT_INPUTKEYBOARD_STATE_RELEASED;
    componentInputKeyboard[entity].actions[componentInputKeyboard[entity].numActions].oldState = COMPONENT_INPUTKEYBOARD_STATE_RELEASED;

    //Update number of actions
    componentInputKeyboard[entity].numActions++;

}

int componentInputKeyboardGetActionIndex(componentInputKeyboardT *componentInputKeyboard,Uint32 entity,char *actionName)
{
    int i=0;
    //if the keyboard input component is valid
    if(componentInputKeyboard != NULL)
    {
        //loop through the input keyboard actions of the entity
        for(i=0;i<componentInputKeyboard[entity].numActions;++i)
        {
            //if the name matches
            if(strcmp(actionName,componentInputKeyboard[entity].actions[i].name)==0)
            {
                //return the index
                return i;
            }
        }
    }
    return -1;
}

void componentFreeInputKeyboard(componentInputKeyboardT *componentInputKeyboard,int maxEntities)
{
    int i=0,j=0;
    if(componentInputKeyboard !=NULL)
    {
        //loop through all entities
        for(j=0;j<maxEntities;++j)
        {
            //if actions have been added
            if(componentInputKeyboard[j].actions!=NULL)
            {
                //loop through the actions
                for(i=0;i<componentInputKeyboard[j].numActions;++i)
                {
                    //if a name was given to an action
                    if(componentInputKeyboard[j].actions[i].name!=NULL)
                    {
                        //free the allocated memory
                        free(componentInputKeyboard[j].actions[i].name);
                    }
                }
            }
            //free the actions
            free(componentInputKeyboard[j].actions);
        }
        //free the input keyboard component
        free(componentInputKeyboard);
    }
}

void componentInputKeyboardSetActiveState(componentInputKeyboardT *componentInputKeyboard,Uint32 entity,int value)
{
    char msg[512];
    if(componentInputKeyboard !=NULL)
    {
        //if the value is 1 or 0
        if(value==1 || value == 0){
            componentInputKeyboard[entity].active = value;
        }
        else{
            sprintf(msg,"Warning in componentInputKeyboardSetActiveState() - Cannot set state for entity:%d, parameter:int value must be 1 or 0. Invalid passed value was:%d",entity,value);
            writeToLog(msg,"runlog.txt");
        }
    }
}
void componentKeyboardInitActionReleaseTimer(componentInputKeyboardT *componentInputKeyboard,Uint32 entity)
{
    if(componentInputKeyboard !=NULL)
    {
        timerInit(&componentInputKeyboard[entity].actionRelease,150);
    }
}
void componentKeyboardUpdateActionReleaseTimer(componentInputKeyboardT *componentInputKeyboard,Uint32 entity)
{
    if(componentInputKeyboard !=NULL)
    {
        if(timerUpdate(&componentInputKeyboard[entity].actionRelease))
        {
            componentInputKeyboard[entity].previousActions[1] = componentInputKeyboard[entity].previousActions[0];
            componentInputKeyboard[entity].previousActions[0] = -1;
        }
    }
}
