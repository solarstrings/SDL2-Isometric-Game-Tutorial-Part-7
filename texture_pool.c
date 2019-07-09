#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "texture_pool.h"
#include "logger.h"

static void texturePoolGetOnlyFilename(char *filenameAndPath,char *filename)
{
    int len = 0;
    int i=0,j=0,index=0;
    char msg[512];
    if(filenameAndPath == NULL || filename == NULL)
    {
        //log it as an error
        sprintf(msg,"Error in texturePoolGetOnlyFilename(): filenameAndPath or filename is NULL!");
        writeToLog(msg,"runlog.txt");
        return;
    }
    //get length of string
    len = strlen(filenameAndPath);

    //loop the end to the string and go towards the beginning
    for(i=len;i>0;--i)
    {
        //if the forward slash is found
        if(filenameAndPath[i] == '/')
        {
            //set the index to the position in the string one character before
            //the forward slash
            index = i+1;
            //break out of the for loop
            break;
        }
    }

    //copy all characters after the / forward slash into the filename
    for(i=index;i<len;++i){
        filename[j] = filenameAndPath[i];
        //update filename
        j++;
    }
    //write end of string
    filename[j] = '\0';
}

texturePoolT *texturePoolNewTexturePool()
{
    char msg[512];
    int i=0;
    texturePoolT *newPool = malloc(sizeof(struct texturePoolT));

    //if memory allocation failed
    if(newPool == NULL)
    {
        //log it as an error
        sprintf(msg,"Error in texturePoolNewTexturePool(): Could not allocate memory for new texture pool!");
        writeToLog(msg,"runlog.txt");
        //return NULL
        return NULL;
    }

    //initialize max and number of textures to 0
    newPool->maxTextures = NUM_INITIAL_TEXTURES_IN_TEXTUREPOOL;
    newPool->numTextures = 0;

    //allocate memory for texture containers
    newPool->textures = malloc(sizeof(struct textureContainerT)*NUM_INITIAL_TEXTURES_IN_TEXTUREPOOL);

    //if textures is NULL
    if(newPool->textures==NULL){

        //free the texture pool
        free(newPool);
        //log it as an error
        sprintf(msg,"Error in texturePoolNewTexturePool(): Could not allocate memory for texture containers!");
        writeToLog(msg,"runlog.txt");
        //return NULL
        return NULL;
    }

    //initialize all texture containers
    for(i=0;i<NUM_INITIAL_TEXTURES_IN_TEXTUREPOOL;++i)
    {
        //set the texture and name to NULL
        newPool->textures[i].name = NULL;
        newPool->textures[i].texture = NULL;
    }
    return newPool;
}

void texturePoolAddTextureToTexturePool(texturePoolT *texturePool,char *filename)
{
    char msg[512];
    int filenameLen = 0;
    int i=0;

    textureContainerT *newTextures = NULL;

    //if the filename parameter is NULL
    if(filename == NULL){
        //log it as an error
        sprintf(msg,"Error in texturePoolAddTextureToTexturePool(): Parameter: 'char *filename' is NULL! Cannot load a NULL texture.");
        writeToLog(msg,"runlog.txt");
        //return
        return;
    }

    //if the texturePool parameter is NULL
    if(texturePool == NULL){
        //log it as an error
        sprintf(msg,"Error in texturePoolAddTextureToTexturePool(): Parameter: 'texturePoolT *texturePool' is NULL! Texture:%s was not added to pool",filename);
        writeToLog(msg,"runlog.txt");
        //return
        return;
    }

    //allocate more memory if needed
    if(texturePool->numTextures >= texturePool->maxTextures){
        //try to allocate memory for 100 more textures
        texturePool->maxTextures += 100;
        newTextures = realloc(texturePool->textures,sizeof(struct textureContainerT)*texturePool->maxTextures);

        //if memory allocation failed
        if(newTextures == NULL){
            //log it as an error
            sprintf(msg,"Error in texturePoolAddTextureToTexturePool(): Could not re-allocate memory for more textures in the texture pool! Texture:%s was not added to the texture pool!",filename);
            writeToLog(msg,"runlog.txt");

            //rollback number of textures
            texturePool->maxTextures -=100;

            //return NULL
            return;
        }
        //initialize the new textures created by the realloc
        for(i=texturePool->numTextures;i<texturePool->maxTextures;++i)
        {
            newTextures[i].name = NULL;
            newTextures[i].texture = NULL;
        }

        //point the texturePool textures memory to the newTextures
        texturePool->textures = newTextures;
    }

    //get the length of the filename
    filenameLen = strlen(filename)+1;
    //allocate memory for the texture name
    texturePool->textures[texturePool->numTextures].name = malloc(sizeof(char)*filenameLen);

    //if memory allocation failed
    if(texturePool->textures[texturePool->numTextures].name == NULL)
    {
        //log it as an error
        sprintf(msg,"Error in texturePoolAddTextureToTexturePool(): Could not allocate memory for the texture name. Texture:%s not added to the pool",filename);
        writeToLog(msg,"runlog.txt");
        //return
        return;
    }

    //get the name of the texture
    texturePoolGetOnlyFilename(filename,texturePool->textures[texturePool->numTextures].name);

    //allocate memory for the new texture
    texturePool->textures[texturePool->numTextures].texture = malloc(sizeof(struct textureT));

    //if memory allocation failed
    if(texturePool->textures[texturePool->numTextures].texture == NULL){
        //log it as an error
        sprintf(msg,"Error in texturePoolAddTextureToTexturePool(): Could not allocate memory for the texture. Texture:%s not added to the pool",filename);
        writeToLog(msg,"runlog.txt");
        //return
        return;
    }
    //if the texture init failed
    if(textureInit(texturePool->textures[texturePool->numTextures].texture,0,0,0,NULL,NULL,SDL_FLIP_NONE) == 0)
    {
        //Log what went wrong
        sprintf(msg,"WARNING: Texture:%s was NOT loaded to texture pool.",filename);
        writeToLog(msg,"runlog.txt");
        //return out of the function
        return;
    }

    //if the texture loading failed
    if(loadTexture(texturePool->textures[texturePool->numTextures].texture,filename)==0)
    {
        //free the memory allocated for the texture name
        free(texturePool->textures[texturePool->numTextures].name);
        //the texture load function will log the error so just return
        return;
    }
    sprintf(msg,"Texture:%s loaded to texture pool.",filename);
    writeToLog(msg,"runlog.txt");
    //increase number of textures in the pool
    texturePool->numTextures++;
}

void texturePoolRemoveTextureFromPool(texturePoolT *texturePool,char *filename)
{
    int i=0;
    char msg[512];

    //if the filename parameter is NULL
    if(filename == NULL){
        //log it as an error
        sprintf(msg,"Error in texturePoolRemoveTextureFromPool(): Parameter: 'char *filename' is NULL!");
        writeToLog(msg,"runlog.txt");
        //return
        return;
    }

    //if the texturePool parameter is NULL
    if(texturePool == NULL){
        //log it as an error
        sprintf(msg,"Error in texturePoolRemoveTextureFromPool(): Parameter: 'texturePoolT *texturePool' is NULL!");
        writeToLog(msg,"runlog.txt");
        //return
        return;
    }
    //loop through the textures
    for(i=0;i<texturePool->numTextures;++i)
    {
        //if the texture was found
        if(strcmp(texturePool->textures[i].name,filename)==0)
        {
            //Free the texture
            free(texturePool->textures[i].texture);

            //free the texture name
            free(texturePool->textures[i].name);

            //if the texture is not the last texture in the pool
            if(i!=texturePool->numTextures)
            {
                //move the last texture to this position in the texture pool
                texturePool->textures[i] = texturePool->textures[texturePool->numTextures-1];
            }

            //decrease number of textures with one
            texturePool->numTextures--;
        }
    }

    //If we reach here, the texture was not found, so we log it as an warning
    sprintf(msg,"Warning in texturePoolRemoveTextureFromPool(): Texture:%s was not found in the texture pool, texture was not removed!",filename);
    writeToLog(msg,"runlog.txt");

}

textureT *texturePoolGetTextureFromPool(texturePoolT *texturePool,char *filename)
{
    int i=0;
    char msg[512];

    //if the filename parameter is NULL
    if(filename == NULL){
        //log it as an error
        sprintf(msg,"Error in texturePoolGetTextureFromPool(): Parameter: 'char *filename' is NULL!");
        writeToLog(msg,"runlog.txt");
        //return NULL
        return NULL;
    }

    //if the texturePool parameter is NULL
    if(texturePool == NULL){
        //log it as an error
        sprintf(msg,"Error in texturePoolGetTextureFromPool(): Parameter: 'texturePoolT *texturePool' is NULL!");
        writeToLog(msg,"runlog.txt");
        //return NULL
        return NULL;
    }

    //loop through the textures
    for(i=0;i<texturePool->numTextures;++i)
    {
        //if the texture was found
        if(strcmp(texturePool->textures[i].name,filename)==0)
        {
            //return the texture pointer
            return texturePool->textures[i].texture;
        }
    }
    //if the texture could not be found, log it as an error
    sprintf(msg,"Warning in texturePoolGetTextureFromPool(): Could not find texture:%s",filename);
    writeToLog(msg,"runlog.txt");
    //return NULL
    return NULL;
}

void texturePoolFreeTexturePool(texturePoolT *texturePool)
{
    int i=0;
    //if the texture pool is allocated
    if(texturePool != NULL)
    {
        //if the texture pointer is allocated
        if(texturePool->textures!=NULL)
        {
            //loop through all the texture containers
            for(i=0;i<texturePool->numTextures;++i)
            {
                //if the texture in the texture container is allocated
                if(texturePool->textures[i].texture != NULL){
                    textureDelete(texturePool->textures[i].texture);
                    //free it
                    free(texturePool->textures[i].texture);
                }
                //if the name in the texture container is allocated
                if(texturePool->textures[i].name != NULL){
                    //free it
                    free(texturePool->textures[i].name);
                }
            }
            //free the allocated texture containers
            free(texturePool->textures);
        }
        //free the texture pool
        free(texturePool);
    }
}
