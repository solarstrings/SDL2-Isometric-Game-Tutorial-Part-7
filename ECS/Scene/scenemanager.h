#ifndef __SCENE_MANAGER_H_
#define __SCENE_MANAGER_H_
#include "scene.h"

#define NUM_INITIAL_SCENES 5

#define SCENE_MANAGER_NO_ACTIVE_SCENE -1

typedef struct sceneManagerT
{
    int numScenes;          //current number of scenes
    int maxScenes;          //current max scenes
    int activeScene;        //current active scene
    sceneT **scenes;        //list of scenes (pointer to an array with the different scenes)
}sceneManagerT;

sceneManagerT *ecsSceneManagerCreateNewSceneManager();
void ecsSceneManagerSetActiveScene(sceneManagerT *sceneManager,char *sceneName);
void ecsSceneManagerCreateSceneInSceneManager(sceneManagerT *sceneManager,char *sceneName);
void ecsSceneManagerRemoveSceneFromSceneManager(sceneManagerT *sceneManager,char *sceneName);
sceneT *ecsSceneManagerGetPointerToScene(sceneManagerT *sceneManager,char *sceneName);
void ecsSceneManagerRunActiveScene(sceneManagerT *sceneManager);
void ecsSceneManagerFreeSceneManager(sceneManagerT *sceneManager);

#endif // __SCENE_MANAGER_H_
