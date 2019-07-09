#ifndef TEXTURE_POOL_H_
#define TEXTURE_POOL_H_
#include <SDL2/SDL.h>
#include "texture.h"

#define NUM_INITIAL_TEXTURES_IN_TEXTUREPOOL 1

typedef struct textureContainerT
{
    textureT *texture;      //pointer to the texture data
    char *name;             //name of the texture
}textureContainerT;

typedef struct texturePoolT
{
    int numTextures;                //number of textures
    int maxTextures;                //current max number of textures
    textureContainerT *textures;    //pointer to the textures
}texturePoolT;

texturePoolT *texturePoolNewTexturePool();
void texturePoolAddTextureToTexturePool(texturePoolT *texturePool,char *filename);
void texturePoolRemoveTextureFromPool(texturePoolT *texturePool,char *filename);
textureT *texturePoolGetTextureFromPool(texturePoolT *texturePool,char *filename);
void texturePoolFreeTexturePool(texturePoolT *texturePool);

#endif // TEXTURE_POOL_H_
