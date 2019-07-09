#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include "IsoEngine/isoEngine.h"
#include "renderer.h"
#include "texture.h"
#include "logger.h"

void setupRect(SDL_Rect *rect,int x,int y,int w,int h)
{
    rect->x = x;
    rect->y = y;
    rect->w = w;
    rect->h = h;
}

int loadTexture(textureT *texture, char *filename)
{
    char msg[200];

    if(texture == NULL){
        sprintf(msg,"Error in loadTexture(): Parameter: 'textureT *texture' is NULL!");
        writeToLog(msg,"runlog.txt");
        return 0;
    }

    SDL_Surface *tmpSurface = IMG_Load(filename);

    if(tmpSurface == NULL){
        sprintf(msg,"Error in loadTexture(): Could not load image:%s! SDL_image Error:%s\n",filename,IMG_GetError());
        writeToLog(msg,"runlog.txt");
        return 0;
    }
    else{
        texture->texture = SDL_CreateTextureFromSurface(getRenderer(),tmpSurface);

        if(texture->texture == NULL){
            sprintf(msg,"Error in loadTexture(): Could not load image:%s! SDL_image Error:%s\n",filename,IMG_GetError());
            writeToLog(msg,"runlog.txt");
            SDL_FreeSurface(tmpSurface);
            return 0;
        }
        else{
            texture->width = tmpSurface->w;
            texture->height = tmpSurface->h;
        }
        SDL_FreeSurface(tmpSurface);
        return 1;
    }
    return 0;
}

int textureInit(textureT *texture, int x,int y, double angle, SDL_Point *center, SDL_Rect *cliprect, SDL_RendererFlip fliptype)
{
    texture->x = x;
    texture->y = y;
    texture->angle = angle;
    texture->fliptype = fliptype;

    if(cliprect!=NULL){
        texture->cliprect = *cliprect;
    }
    else{
        setupRect(&texture->cliprect,0,0,100,100);
    }
    if(center !=NULL){
        texture->center = *center;
    }

    return 1;
}

void textureRenderXYClip(textureT *texture, int x, int y, SDL_Rect *cliprect)
{
    char msg[200];
    if(texture==NULL){
        sprintf(msg,"Warning in textureRenderXYClip(): passed texture was null!\n");
        writeToLog(msg,"runlog.txt");
        return;
    }
    texture->x = x;
    texture->y = y;
    texture->cliprect = *cliprect;
    SDL_Rect quad = { texture->x, texture->y, texture->width, texture->height };

    quad.w = texture->cliprect.w;
    quad.h = texture->cliprect.h;


    SDL_RenderCopyEx(getRenderer(),texture->texture,&texture->cliprect,&quad,texture->angle, &texture->center,texture->fliptype);
}
void textureRenderXYClipScale(textureT *texture, int x, int y, SDL_Rect *cliprect,float scale)
{
    float w,h;
    float diffx,diffy;
    SDL_Rect quad;
    w=(float)texture->width*scale;
    h=(float)texture->height*scale;

    diffx = (x*scale) - x;
    diffy = (y*scale) - y;

    setupRect(&quad,(x*scale)-diffx,(y*scale)-diffy,w,h);

    if(cliprect!=NULL)
    {
        texture->cliprect = *cliprect;

        quad.w = (int)texture->cliprect.w*scale;
        quad.h = (int)texture->cliprect.h*scale;

        //if the current scale is larger or smaller than 1.0
        if(scale <1.0 || scale >1.0){
            //increase the width and height of the quad to not get
            //rendering artifacts
            quad.h +=1;
            quad.w +=1;
        }
        //Center point is passed in as NULL. If/When a function is needed to change center point on a sprite, we'll add one.
        SDL_RenderCopyEx(getRenderer(),texture->texture,&texture->cliprect,&quad,texture->angle,NULL,texture->fliptype);
    }
    //if the cliprect is NULL
    else
    {
        //Center point is passed in as NULL. If/When a function is needed to change center point on a sprite, we'll add one.
        //Draw without clip rectangle
        SDL_RenderCopyEx(getRenderer(),texture->texture,NULL,&quad,texture->angle,NULL,texture->fliptype);
    }
}

void textureDelete(textureT *texture)
{
    if(texture!=NULL)
    {
        //if the texture is allocated
        if(texture->texture != NULL){
            SDL_DestroyTexture(texture->texture);
        }
    }
}
