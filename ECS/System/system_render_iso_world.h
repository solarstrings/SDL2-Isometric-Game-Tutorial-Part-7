#ifndef __RENDER_ISOMETRIC_SYSTEM_H__
#define __RENDER_ISOMETRIC_SYSTEM_H__

//local struct for an on-screen entity
typedef struct entityOnScreenPosT
{
    Uint32 entityID;        //entity ID to draw
    int cartesianYPos;      //entity Cartesian y position
    int row;                //which row the entity is standing on
}entityOnScreenPosT;

//Local struct to store entities that will be drawn on the screen
typedef struct entitiesOnScreenT
{
    entityOnScreenPosT *entityList;     //list with entities on the screen
    Uint32 maxEntities;                 //current max entities on screen
    Uint32 numEntities;                 //number entities on screen
    Uint32 numEntitiesLastRender;       //number of entities last render call. Used by the collision system
    Uint32 currentEntityToDraw;         //current index in the sortedIndexList we want to draw
}entitiesOnScreenT;


int systemRenderIsoMetricWorldInit(void *scene);
void systemRenderIsometricWorld();
void systemFreeRenderIsoWorldSystem();
void systemRenderIsometricWorldSortEntity(Uint32 entity);
entitiesOnScreenT *systemRenderIsometricWorldGetEntitiesOnScreen(int layer);

#endif // __RENDER_ISOMETRIC_SYSTEM_H__

