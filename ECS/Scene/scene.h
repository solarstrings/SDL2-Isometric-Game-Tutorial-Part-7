#ifndef __SCENE_H_
#define __SCENE_H_

#include "../Entity/entity.h"
#include "../Component/component.h"
#include "../System/system.h"
#include "../../IsoEngine/isoEngine.h"

#define NUM_INITIAL_SYSTEMS 1
#define NUM_INITIAL_ENTITIES 1

#define SCENE_NAME_LENGTH   100

//scene struct
typedef struct sceneT
{
    char name[SCENE_NAME_LENGTH];   //Name of the scene
    entityT *entities;              //the entities in the scene
    Uint32 numEntities;             //current number of entities
    Uint32 maxEntities;             //current max allocated entities

    componentT *components;         //the components available in the scene
    Uint32 numComponents;           //current number of components
    Uint32 maxComponents;           //current max allocated components

    systemT *systems;               //the registered systems in the scene
    Uint32 numSystems;              //current number of systems running
    Uint32 maxSystems;              //current max allocated systems

    int memallocFailed;             //if a memory allocation failure has occurred.
    int systemInitFailed;           //if a system has failed to initialize
    int deleteScene;                //if the scene should be deleted and removed from memory
    int exitScene;                  //if the user wants to exit the scene
    int consumeLessCPU;             //if the system should use less CPU ( SDL_Delay(1) )

    int sceneHasInputMouseComponent;        //if the scene has the mouse input component
    //If the scene does not have keyboard input, we have to add functionality to the scene so that it can be closed (Esc key or SLD_QUIT event)
    int sceneHasInputSystem;                //if the scene has an input system
    int sceneHasInputKeyboardComponent;     //if the scene has the keyboard input component
    int componentPointersRealloced;         //if the components in the scene was reallocated

    isoEngineT *isoEngine;          //Pointer to isometric engine
}sceneT;

sceneT *ecsSceneCreateNewScene(char *name);
void ecsSceneFreeScene(sceneT *scene);
Uint32 ecsSceneAddEntityToScene(sceneT* scene, Uint32 componentSet1);//,Uint32 componentSet2);
int ecsSceneAddComponentToScene(sceneT *scene,componentTypeE componentType);
void ecsSceneRemoveEntityFromScene(sceneT* scene, Uint32 entityID);
Uint32 ecsSceneGetComponentIndex(sceneT *scene,Uint32 componentFlag);
void *ecsSceneGetComponent(sceneT *scene,Uint32 componentFlag);

Uint32 ecsSceneGetNumEntities(sceneT *scene);
int ecsSceneAddSystemToScene(sceneT *scene,systemTypeE systemType);
int ecsSceneInitSystemsInScene(sceneT *scene);

void ecsSceneUpdateSystemsInScene(sceneT *scene);
void ecsGetSystemName(systemTypeE systemType,char *name);
void ecsGetComponentName(componentTypeE componentType,char *name);

int ecsGetComponentBit(componentTypeE value);
void ecsSceneSetCPUDelay(sceneT *scene, int value);

#endif // __scene_H_


