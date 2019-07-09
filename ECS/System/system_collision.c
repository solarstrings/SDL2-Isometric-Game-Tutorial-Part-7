#include <stdio.h>
#include "system.h"
#include "system_collision.h"
#include "../../logger.h"
#include "../../deltatimer.h"
#include "../Scene/scene.h"
#include "../Component/component.h"
#include "../../IsoEngine/isoEngine.h"
#include "system_render_iso_world.h"

#define SYSTEM_COLLISION_MASK_SET1 (COMPONENT_SET1_POSITION | COMPONENT_SET1_VELOCITY | COMPONENT_SET1_COLLISION | COMPONENT_SET1_RENDER2D)

//local global functions
static void handleEntityWorldCollision(Uint32 entity);
static void handleEnityToEntityCollision(Uint32 entity);
static void checkPointCollision(Uint32 entity,int x,int y);
static void createWorldCollisionRect(Uint32 entity);

//local global pointer to the data
static componentPositionT *posComponents = NULL;
static componentVelocityT *velComponents = NULL;
static componentRender2DT *renderComponents = NULL;
static componentCollisionT *colComponents = NULL;
static entitiesOnScreenT *onScreenEntities = NULL;



//local global pointer to the scene
static sceneT *scn = NULL;

//local global pointer to the isoEngine
static isoEngineT *isoEngine = NULL;

//local global variable for system failure
static int systemFailedToInitialize = 1;

static void updateComponentPointers()
{
    //if there is no scene
    if(scn == NULL){
        //return out of the function
        return;
    }
    //Error handling is done in the scene.c file.

    //get the position component pointer
    posComponents = (componentPositionT*)ecsSceneGetComponent(scn,COMPONENT_SET1_POSITION);

    //get the velocity component pointer
    velComponents = (componentVelocityT*)ecsSceneGetComponent(scn,COMPONENT_SET1_VELOCITY);

    //get the collision component pointer
    colComponents = (componentCollisionT*)ecsSceneGetComponent(scn,COMPONENT_SET1_COLLISION);

    //get the render 2D component pointer
    renderComponents = (componentRender2DT*)ecsSceneGetComponent(scn,COMPONENT_SET1_RENDER2D);
}

int systemCollisionInit(void *scene)
{
    systemFailedToInitialize = 0;

    writeToLog("Initializing collision system...","runlog.txt");

    if(scene == NULL){
        writeToLog("Error in Function systemCollisionInit() - Collision system failed to initialize: Parameter 'void *scene' is NULL ","runlog.txt");
        systemFailedToInitialize = 1;
        return 0;
    }

    scn = (sceneT*)scene;

    //if the passed scene is NULL
    if(scn->isoEngine == NULL){
        writeToLog("Error in Function systemCollisionInit() - Collision system failed to initialize: The scene does not have the isometric engine! scene->isoEngine is NULL","runlog.txt");
        systemFailedToInitialize = 1;
        return 0;
    }
    isoEngine = scn->isoEngine;
    //if the isoEngine is without a map
    if(isoEngine->isoMap == NULL)
    {
        writeToLog("Error in Function systemCollisionInit() - Collision system failed to initialize: The isoEngine does not have a map! isoEngine->isoMap is NULL","runlog.txt");
        systemFailedToInitialize = 1;
        return 0;
    }

    //check if the scene has position components
    posComponents = (componentPositionT*)ecsSceneGetComponent(scn,COMPONENT_SET1_POSITION);
    //if not
    if(posComponents == NULL)
    {
        //log the error
        writeToLog("Error in Function systemCollisionInit() - Collision system failed to initialize: The scene does not have position components (COMPONENT_SET1_POSITION)","runlog.txt");
        systemFailedToInitialize = 1;
        return 0;
    }
    //check if the scene has velocity components
    velComponents = (componentVelocityT*)ecsSceneGetComponent(scn,COMPONENT_SET1_VELOCITY);
    //if not
    if(velComponents == NULL)
    {
        //log the error
        writeToLog("Error in Function systemCollisionInit() - Collision system failed to initialize: The scene does not have velocity components (COMPONENT_SET1_VELOCITY)","runlog.txt");
        systemFailedToInitialize = 1;
        return 0;
    }

    //check if the scene has collision components
    colComponents = (componentCollisionT*)ecsSceneGetComponent(scn,COMPONENT_SET1_COLLISION);
    //if not
    if(colComponents == NULL)
    {
        //log the error
        writeToLog("Error in Function systemCollisionInit() - Collision system failed to initialize: The scene does not have collision components (COMPONENT_SET1_COLLISION)","runlog.txt");
        systemFailedToInitialize = 1;
        return 0;
    }

    //check if the scene has render 2D components
    renderComponents = (componentRender2DT*)ecsSceneGetComponent(scn,COMPONENT_SET1_RENDER2D);
    //if not
    if(renderComponents == NULL)
    {
        //log the error
        writeToLog("Error in Function systemCollisionInit() - Collision system failed to initialize: The scene does not have render 2D components (COMPONENT_SET1_RENDER2D)","runlog.txt");
        systemFailedToInitialize = 1;
        return 0;
    }
    writeToLog("Initializing Collision System... DONE","runlog.txt");
    //return 1, successfully initialized the system
    return 1;
}

void systemCollisionUpdate()
{
    //if there is no scene
    if(scn == NULL) {
        return;
    }

    //if the component pointers has been reallocated in the scene
    if(scn->componentPointersRealloced == 1){
        updateComponentPointers();
    }
    onScreenEntities = systemRenderIsometricWorldGetEntitiesOnScreen(1);
}

void systemCollisionUpdateEntity(Uint32 entity)
{
    //if the system failed to initialize
    if(systemFailedToInitialize == 1){
        return;
    }

    //if the entity has the position, velocity, render2D and collision component
    if(scn->entities[entity].componentSet1 & SYSTEM_COLLISION_MASK_SET1)
    {
        //reset is colliding to 0;
        colComponents[entity].isColliding = 0;
        //if collision detection is not active for the entity
        if(colComponents[entity].collisionType == COLLISIONTYPE_DEACTIVATED){
            //exit out of the function
            return;
        }
        //if the entity can collide with the world
        if(colComponents[entity].collisionType == COLLISIONTYPE_WORLD
        || colComponents[entity].collisionType == COLLISIONTYPE_WORLD_AND_ENTITY)
        {
            handleEntityWorldCollision(entity);
        }
        if(colComponents[entity].collisionType == COLLISIONTYPE_ENTITY
        || colComponents[entity].collisionType == COLLISIONTYPE_WORLD_AND_ENTITY){
            handleEnityToEntityCollision(entity);
        }
    }
}

static void handleEntityWorldCollision(Uint32 entity)
{
    //check the bottom bottom rectangle points for the sprite collision
    checkPointCollision(entity,0,colComponents[entity].rect.h); //bottom left corner
    checkPointCollision(entity,colComponents[entity].rect.w,0); //bottom right corner
}
static void createWorldCollisionRect(Uint32 entity)
{
    point2DT point;
    //get the entity world position
    point.x = (posComponents[entity].x*isoEngine->zoomLevel)+isoEngine->scrollX;
    point.y = (posComponents[entity].y*isoEngine->zoomLevel)+isoEngine->scrollY;
    isoEngineConvert2dToIso(&point);
    //apply the offset
    point.x += posComponents[entity].xOffset*isoEngine->zoomLevel;
    point.y += posComponents[entity].yOffset*isoEngine->zoomLevel;

    //create the collision rectangle
    //x,y start position for the rectanble
    //multiply with 0.5 to get the center of the texture
    colComponents[entity].worldRect.x = point.x +((renderComponents[entity].texture->cliprect.w*0.5)*isoEngine->zoomLevel)
                                                -((colComponents[entity].rect.w*0.5)*isoEngine->zoomLevel);
    //start at the bottom of the rectangle
    colComponents[entity].worldRect.y = point.y +((renderComponents[entity].texture->cliprect.h)*isoEngine->zoomLevel)
                                                -((colComponents[entity].rect.h)*isoEngine->zoomLevel);
    //width and height of the collision rectangle
    colComponents[entity].worldRect.w = colComponents[entity].rect.w*isoEngine->zoomLevel;
    colComponents[entity].worldRect.h = colComponents[entity].rect.h*isoEngine->zoomLevel;
}

static void handleEnityToEntityCollision(Uint32 entity)
{
    Uint32 i = 0;

    createWorldCollisionRect(entity);
    for(i=0;i<onScreenEntities->numEntitiesLastRender;++i)
    {
        //if the entity is not it self
        if(onScreenEntities->entityList[i].entityID!=entity){
            createWorldCollisionRect(onScreenEntities->entityList[i].entityID);

            //if there is a collision
            if(systemCollisionBoundingBoxCollision(colComponents[entity].worldRect,colComponents[onScreenEntities->entityList[i].entityID].worldRect))
            {
                posComponents[entity].x = posComponents[entity].oldx[0];
                posComponents[entity].y = posComponents[entity].oldy[0];
                colComponents[entity].isColliding = 1;
            }
        }
    }
}

static void checkPointCollision(Uint32 entity,int x,int y)
{
    point2DT point;
    int tile = 0;

    //check the width of the rectangle upwards
    point.x = (posComponents[entity].x + x)/isoEngine->isoMap->tileSize;
    point.y = (posComponents[entity].y + y)/isoEngine->isoMap->tileSize;

    //get the tile under the entity
    tile = isoMapGetTile(isoEngine->isoMap,point.x,point.y,renderComponents[entity].layer);

    //if the tile is valid
    if(tile!=-1){
        //TODO: Add list of tiles that can be collided with
        if(tile == 2){
            posComponents[entity].x = posComponents[entity].oldx[0];
            posComponents[entity].y = posComponents[entity].oldy[0];
            //mark that the entity is colliding
            colComponents[entity].isColliding = 1;
        }
    }

    //check the width of the rectangle downwards
    point.x = (posComponents[entity].x - x)/isoEngine->isoMap->tileSize;
    point.y = (posComponents[entity].y - y)/isoEngine->isoMap->tileSize;

    //get the tile under the entity
    tile = isoMapGetTile(isoEngine->isoMap,point.x,point.y,renderComponents[entity].layer);

    //if the tile is valid
    if(tile!=-1){
        //TODO: Add list of tiles that can be collided with
        if(tile == 2){
            posComponents[entity].x = posComponents[entity].oldx[0];
            posComponents[entity].y = posComponents[entity].oldy[0];
            //mark that the entity is colliding
            colComponents[entity].isColliding = 1;
        }
    }
}

int systemCollisionBoundingBoxCollision(SDL_Rect a,SDL_Rect b)
{
    //TODO: describe the collisions here
    if(b.x + b.w < a.x) return 0;
    if(b.x > a.x + a.w) return 0;
    if(b.y + b.h < a.y) return 0;
    if(b.y > a.y + a.h) return 0;
    return 1;
}

void systemCollisionFree()
{
    //nothing to free, the system is not allocating any memory
}
