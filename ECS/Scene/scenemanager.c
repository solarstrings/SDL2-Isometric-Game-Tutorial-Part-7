#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../logger.h"
#include "../../deltatimer.h"
#include "scenemanager.h"
#include "scene.h"

sceneManagerT *ecsSceneManagerCreateNewSceneManager()
{
    sceneManagerT *sm = malloc(sizeof(struct sceneManagerT));
    if(sm == NULL){
        //write the error to the log file
        writeToLog("Error in Function ecsSceneManagerCreateNewSceneManager() - Could not allocate memory for a new scene manager!","runlog.txt");
        return NULL;
    }
    //allocate scene pointers, note the: "struct sceneT*"
    sm->scenes = malloc(sizeof(struct sceneT*)*NUM_INITIAL_SCENES);
    if(sm->scenes == NULL){
        writeToLog("Error in Function ecsSceneManagerCreateNewSceneManager() - Cfould not allocate memory for scenes!","runlog.txt");
        free(sm);
        return NULL;
    }
    sm->numScenes = 0;
    sm->maxScenes = NUM_INITIAL_SCENES;
    sm->activeScene = SCENE_MANAGER_NO_ACTIVE_SCENE;
    return sm;
}

void ecsSceneManagerSetActiveScene(sceneManagerT *sceneManager,char *sceneName)
{
    int i=0;
    char msg[512];

    //if the passed scene manager is NULL
    if(sceneManager==NULL){
        //write it to the log file
        writeToLog("Error in Function ecsSceneManagerSetActiveScene() - Parameter 'sceneManagerT *sceneManager' is NULL!","runlog.txt");
        return;
    }
    //loop through all the scenes
    for(i=0;i<sceneManager->numScenes;++i){
        //if the scene was found
        if(strcmp(sceneManager->scenes[i]->name,sceneName)==0)
        {
            //set the it to the active one
            sceneManager->activeScene = i;

            //Initialize the scene systems
            ecsSceneInitSystemsInScene(sceneManager->scenes[i]);

            //exit the function
            return;
        }
    }
    //if the scene was not found, log a warning about it.
    sprintf(msg,"Error in Function ecsSceneManagerSetActiveScene() - Scene:%s was not found! Active scene was not changed!",sceneName);
    fprintf(stderr,"%s",msg);
    writeToLog(msg,"runlog.txt");
}

void ecsSceneManagerCreateSceneInSceneManager(sceneManagerT *sceneManager,char *sceneName)
{
    char msg[512];
    sceneT **newScenes = NULL;

    //if the passed scene manager is NULL
    if(sceneManager==NULL){
        //write it to the log file
        writeToLog("Error in Function ecsSceneManagerAddSceneToSceneManager() - Parameter: 'sceneManagerT *sceneManager' is NULL!","runlog.txt");
        return;
    }

    //if more memory is needed
    if(sceneManager->numScenes>=sceneManager->maxScenes){
        //add five more scenes
        sceneManager->maxScenes+=5;
        //re-allocate memory for the scene pointers
        newScenes = realloc(sceneManager->scenes,sizeof(struct sceneT*)*sceneManager->maxScenes);

        //if memory allocation failed
        if(newScenes==NULL){
            sprintf(msg,"Error in Function ecsSceneManagerAddSceneToSceneManager() - Could not allocate memory for more scenes! Scene:%s was not added!",sceneName);
            writeToLog(msg,"runlog.txt");
            sceneManager->maxScenes-=5;
            return;
        }

        //point the scene pointer to the new memory
        sceneManager->scenes = newScenes;
    }

    //create a new scene
    //if something went wrong, the ecsCreateNewScene() will log the error to file
    sceneManager->scenes[sceneManager->numScenes] = ecsSceneCreateNewScene(sceneName);

    //if all went well
    if(sceneManager->scenes[sceneManager->numScenes] != NULL){
        //increase number of scenes with one
        sceneManager->numScenes++;
    }
}

void ecsSceneManagerRemoveSceneFromSceneManager(sceneManagerT *sceneManager,char *sceneName)
{
    int i=0;
    char msg[512];

    //if the passed in scene manager is NULL
    if(sceneManager==NULL){
        //write it to the log file
        writeToLog("Error in Function ecsSceneManagerRemoveSceneFromSceneManager() - Parameter: 'sceneManagerT *sceneManager' is NULL!","runlog.txt");
        return;
    }

    //loop through all the scenes
    for(i=0;i<sceneManager->numScenes;++i){
        //if the scene was found
        if(strcmp(sceneManager->scenes[i]->name,sceneName)==0)
        {
            //if there is only one scene or it's the last scene
            if(sceneManager->numScenes==1 || i==sceneManager->numScenes){
                //Free the scene
                ecsSceneFreeScene(sceneManager->scenes[i]);

                //decrease number of scenes.
                sceneManager->numScenes--;

                //we removed the scene, exit the function
                return;
            }

            //free the current scene
            ecsSceneFreeScene(sceneManager->scenes[i]);

            //copy the last scene to the current scene position.
            sceneManager->scenes[i] = sceneManager->scenes[sceneManager->numScenes];
            //We don't remove any pointers in the last position, since they will be overwritten
            //next time a scene is added to that position in the list

            //decrease number of scenes.
            sceneManager->numScenes--;
            //we removed the scene, exit the function
            return;
        }
    }
    //if we reach the end without removing anything
    //Log that the scene was not removed, since it could not be found.
    sprintf(msg,"Warning in Function ecsSceneManagerRemoveSceneFromSceneManager() - Scene:%s was not found. No scene was removed!",sceneName);
    writeToLog(msg,"runlog.txt");
}

sceneT *ecsSceneManagerGetPointerToScene(sceneManagerT *sceneManager,char *sceneName)
{
    int i=0;
    char msg[512];
    //if the passed in scene manager is NULL
    if(sceneManager==NULL){
        //write it to the log file
        writeToLog("Error in Function ecsSceneManagerGetPointerToScene() - Parameter: 'sceneManagerT *sceneManager' is NULL!","runlog.txt");
        return NULL;
    }
    //if the sceneName is NULL
    if(sceneName == NULL){
        //write it to the log file
        writeToLog("Error in Function ecsSceneManagerGetPointerToScene() - Parameter: 'char *sceneName' is NULL!","runlog.txt");
        return NULL;
    }
    //loop through all the scenes
    for(i=0;i<sceneManager->numScenes;++i){
        //if the scene was found
        if(strcmp(sceneManager->scenes[i]->name,sceneName)==0)
        {
            //return it
            return sceneManager->scenes[i];
        }
    }
    //if the scene was not found, log a warning about it.
    sprintf(msg,"Error in Function ecsSceneManagerGetPointerToScene() - Scene:%s was not found! Returning NULL",sceneName);
    writeToLog(msg,"runlog.txt");

    //return NULL
    return NULL;
}

void ecsSceneManagerRunActiveScene(sceneManagerT *sceneManager)
{
    //event to handle keyboard if the scene is without keyboard component and input system
    SDL_Event event;
    int handleExit = 0;
    char msg[512];

    //if the scene is NULL
    if(sceneManager == NULL)
    {
        //write error to the log file and exit out of the function
        writeToLog("Error in Function ecsSceneManagerRunScene() - parameter: 'sceneManagerT *sceneManager' is NULL","runlog.txt");
        return;
    }
    //if there is no active scene
    if(sceneManager->activeScene == SCENE_MANAGER_NO_ACTIVE_SCENE){
        //log it as info
        sprintf(msg,"ecsSceneManagerRunScene() - No active scene is set! (activeScene = SCENE_MANAGER_NO_ACTIVE_SCENE )");
        writeToLog(msg,"runlog.txt");
        fprintf(stdout,"ecsSceneManagerRunScene() - No active scene is set! (activeScene = SCENE_MANAGER_NO_ACTIVE_SCENE )");
    }

    //If the scene is NULL
    if(sceneManager->scenes[sceneManager->activeScene] == NULL){
        //log it as an error
        sprintf(msg,"Error in ecsSceneManagerRunScene() - Active Scene:%d is NULL! Cannot switch scene.",sceneManager->activeScene);
        writeToLog(msg,"runlog.txt");
        fprintf(stdout,"Error in ecsSceneManagerRunScene() - Active Scene:%d is NULL! Cannot switch scene.",sceneManager->activeScene);
    }

    //if the scene is without keyboard component
    if(sceneManager->scenes[sceneManager->activeScene]->sceneHasInputKeyboardComponent == 0)
    {
        sprintf(msg,"ecsSceneManagerRunScene() - Scene:%s has no keyboard component! - Functionality to exit scene by pressing ESC will be added",sceneManager->scenes[sceneManager->activeScene]->name);
        writeToLog(msg,"runlog.txt");
        fprintf(stdout,"Scene:%s has no keyboard component! - Functionality to exit scene by pressing ESC will be added\n",sceneManager->scenes[sceneManager->activeScene]->name);
        handleExit = 1;
    }
    //if the scene is without an input system
    if(sceneManager->scenes[sceneManager->activeScene]->sceneHasInputSystem == 0){
        sprintf(msg,"ecsSceneManagerRunScene() - Scene:%s has no input system! - Functionality to exit scene by pressing ESC will be added",sceneManager->scenes[sceneManager->activeScene]->name);
        writeToLog(msg,"runlog.txt");
        fprintf(stdout,"Scene:%s has no input system! - Functionality to exit scene by pressing ESC will be added\n",sceneManager->scenes[sceneManager->activeScene]->name);
        handleExit = 1;
    }

    //if systems for the scene has failed to initialize
    if(sceneManager->scenes[sceneManager->activeScene]->systemInitFailed == 1)
    {
        //log it as an error
        sprintf(msg,"Error in ecsSceneManagerRunScene() - Cannot switch scene! Systems has failed to initialize for scene:%s!",sceneManager->scenes[sceneManager->activeScene]->name);
        fprintf(stdout,"%s",msg);
        writeToLog(msg,"runlog.txt");
    }
    //as long as exitScene is false
    while(!sceneManager->scenes[sceneManager->activeScene]->exitScene)
    {
        //If the scene is without keyboard input, add exit handling to the scene
        if(handleExit == 1)
        {
            //Poll events
            while (SDL_PollEvent(&event)){
                //if the user is calling SDL_QUIT
                if (event.type == SDL_QUIT){
                    sceneManager->scenes[sceneManager->activeScene]->exitScene = 1;
                }
                //if a key was released
                if(event.type == SDL_KEYUP){
                    //if the user is pressing escape
                    if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE){
                        sceneManager->scenes[sceneManager->activeScene]->exitScene = 1;
                    }
                }
            }
        }

        //if the scene should utilize less CPU
        if(sceneManager->scenes[sceneManager->activeScene]->consumeLessCPU==1){
            //don't be a CPU hog! Delay a little bore processing next frame
            SDL_Delay(1);
        }

        //update the delta timer
        deltaTimerUpdate();

        //update all the systems in the scene
        ecsSceneUpdateSystemsInScene(sceneManager->scenes[sceneManager->activeScene]);
    }
}

void ecsSceneManagerFreeSceneManager(sceneManagerT *sceneManager)
{
    int i=0;
    //if the scene manager has allocated memory
    if(sceneManager != NULL){
        //if the list of scenes is allocated
        if(sceneManager->scenes != NULL){
            //loop through all the scenes
            for(i=0;i<sceneManager->numScenes;++i){
                //if the scene is allocated
                if(sceneManager->scenes[i] != NULL){
                    //free the scene
                    ecsSceneFreeScene(sceneManager->scenes[i]);
                }
            }
            //free the scenes
            free(sceneManager->scenes);
        }
        //free the scene manager
        free(sceneManager);
    }
}
