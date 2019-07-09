#include <stdlib.h>
#include <stdio.h>
#include "component_render2D.h"
#include "component_position.h"
#include "../../logger.h"
#include "../Scene/scene.h"

componentRender2DT *componentNewRender2D()
{
    Uint32 i=0;
    //allocate memory for new position components
    componentRender2DT *newRenderComponent = malloc(sizeof(struct componentRender2DT)*NUM_INITIAL_ENTITIES);

    //if memory allocation failed
    if(newRenderComponent==NULL)
    {
        //write the error to the logfile
        writeToLog("Error in Function componentNewRender2D() - Could not allocate memory for render components! ","runlog.txt");
        return NULL;
    }

    //loop through all the entities
    for(i=0;i<NUM_INITIAL_ENTITIES;++i){
        //set texture to NULL
        newRenderComponent[i].texture = NULL;
        newRenderComponent[i].layer = -1;
    }

    //return the pointer to the position components
    return newRenderComponent;
}

void componentRender2DAllocateMoreMemory(sceneT *scene,int componentIndex)
{
    int j=0;
    componentRender2DT *newComponentRender2D = realloc((componentRender2DT*)scene->components[componentIndex].data,sizeof(struct componentRender2DT)*scene->maxEntities);
    if(newComponentRender2D == NULL){
        //write the error to the logfile
        writeToLog("Error in Function componentRender2DAllocateMoreMemory() - Could not allocate more memory for render2D components! ","runlog.txt");
        //mark that memory allocation failed
        scene->memallocFailed = 1;
        return;
    }
    //initialize the component for the new entities
    for(j=scene->numEntities;j<scene->maxEntities;++j){
        newComponentRender2D[j].texture = NULL;
        newComponentRender2D[j].layer = -1;
    }
    //point the data pointer to the new data
    scene->components[componentIndex].data = newComponentRender2D;
}

void componentFreeRender2D(componentRender2DT *render2Dcomponent)
{
    //textures are not stored in the components themselves, therefore we will
    //not free the memory the texture pointer is pointing to.

    //if the position component is not NULL
    if(render2Dcomponent!=NULL){
        //free it
        free(render2Dcomponent);
    }
}

void componentRender2DSetTextureAndClipRect(componentRender2DT *render2DComponent,Uint32 entity,textureT *texture,SDL_Rect *clipRect)
{
    //if the render 2D component does not exist
    if(render2DComponent == NULL){
        //write the error to the log file
        writeToLog("Error in Function componentRender2DSetTextureAndClipRect() - Parameter: 'componentRender2DT *render2DComponent' is NULL!  ","runlog.txt");
        return;
    }

    //if the provided texture is NULL
    if(texture==NULL){
        //write the error to the log file
        writeToLog("Error in Function componentRender2DSetTextureAndClipRect() - Cannot set texture! Parameter: 'textureT *texture' is NULL!","runlog.txt");
    }
    //set the texture
    render2DComponent[entity].texture = texture;

    //if the clipRect is NULL
    if(clipRect == NULL){
        //set the whole texture as the clip rectangle
        setupRect(&render2DComponent[entity].texture->cliprect,0,0,texture->width,texture->height);
    }
    else{
        //set the clip rectangle
        setupRect(&render2DComponent[entity].texture->cliprect,clipRect->x,clipRect->y,clipRect->w,clipRect->h);
    }
}

void componentRender2DSetClipRect(componentRender2DT *render2DComponent,Uint32 entity,SDL_Rect *clipRect)
{
    char msg[512];

    //if the render 2D component does not exist
    if(render2DComponent == NULL){
        //write the error to the log file
        writeToLog("Error in Function componentRender2DSetClipRect() - Parameter: 'componentRender2DT *render2DComponent' is NULL!  ","runlog.txt");
        return;
    }
    if(render2DComponent[entity].texture == NULL){
        sprintf(msg,"Error in Function componentRender2DSetClipRect() - render2DComponent for entity:%d has no texture! render2DComponent is NULL!",entity);
        //write the error to the log file
        writeToLog(msg,"runlog.txt");
        return;
    }
    //if the clipRect is NULL
    if(clipRect == NULL){
        //set the whole texture as the clip rectangle
        setupRect(&render2DComponent[entity].texture->cliprect,0,0,render2DComponent[entity].texture->width,render2DComponent[entity].texture->height);
    }
    else{
        //set the clip rectangle
        setupRect(&render2DComponent[entity].texture->cliprect,clipRect->x,clipRect->y,clipRect->w,clipRect->h);
    }
}

void componentRender2DSetLayer(componentRender2DT *render2DComponent,Uint32 entity,int layer)
{
    if(layer<0){
        layer = 0;
    }
    render2DComponent[entity].layer = layer;
}
