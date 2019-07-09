#ifndef __TEXTURE_H_
#define __TEXTURE_H_
#include <SDL2/SDL.h>

typedef struct textureT
{
    int x;
    int y;
    int width;
    int height;
    double angle;
    SDL_Point center;
    SDL_Rect cliprect;
    SDL_RendererFlip fliptype;
    SDL_Texture *texture;
}textureT;

int loadTexture(textureT *texture, char *filename);
int textureInit(textureT *texture, int x,int y, double angle, SDL_Point *center, SDL_Rect *cliprect, SDL_RendererFlip fliptype);
void textureRenderXYClip(textureT *texture, int x, int y, SDL_Rect *cliprect);
void textureRenderXYClipScale(textureT *texture, int x, int y, SDL_Rect *cliprect,float scale);
void textureDelete(textureT *texture);
void setupRect(SDL_Rect *rect,int x,int y,int w,int h);

#endif // __TEXTURE_H_
