#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "isoEngine.h"
#include "isoMap.h"
#include "../texture.h"
#include "../logger.h"
#include "perlinNoise.h"

static void isoGenerateMap(isoMapT *isoMap, int perlinSeed, int terrainHeight);
static void generatePerlinNoiseMap(isoMapT *isoMap, float *noiseMap,int perlinSeed,int truncateTerrainHeight);
static void rewriteNoiseMapTerrainHeight(isoMapT *isoMap, float *noiseMap, int truncateTerrainHeight);
static void drawPerlinNoiseTerrain(isoMapT *isoMap,float *noiseMap);
static void drawGreenGrass(isoMapT *isoMap);
static int tileIsWithinTerrainHeight(int tileValue,int terrainHeight);
static int countNumberOfNeighbouringTiles(isoMapT *isoMap,int x,int y,int layer,int terrainHeight);
static void deleteSingleTiles(isoMapT *isoMap,int layer,float *noiseMap);
static int bitWiseCalculateTile(isoMapT *isoMap,int x,int y,int layer,int terrainHeight);
static void autoTileTerrain(isoMapT *isoMap, int layer);
static int bitWiseCalculateInnerTiles(isoMapT *isoMap,int x,int y,int layer,int terrainHeight);
static void correctMinorErrorsInSlopes(isoMapT *isoMap);
static void deleteTilesAtMapEdges(isoMapT *isoMap);

isoMapT* isoMapCreateNewMap(char *mapName,int width,int height,int numLayers,int tileSize,int perlinSeed, int terrainHeight)
{
    //Set failsafe values
    if(height<=0){
        height = 10;
    }

    if(width<=0){
        width = 10;
    }

    if(numLayers<=0){
        numLayers = 1;
    }

    //allocate memory for the map
    isoMapT *isoMap = malloc(sizeof(struct isoMapT));
    if(isoMap == NULL){
        writeToLog("Error in function: isoMapCreateEmptyMap() - Could not allocate memory for isometric map!","runlog.txt");
        return NULL;
    }
    //allocate memory for map data (the actual tiles)
    isoMap->mapData = malloc(1 + width * height * numLayers * sizeof(int));
    if(isoMap->mapData == NULL){
        writeToLog("Error in function: isoMapCreateEmptyMap() - Could not allocate memory for isometric map data!","runlog.txt");
        return NULL;
    }
    memset(isoMap->mapData,-1,width * height *numLayers *sizeof(int));

    //allocate memory for the tile set
    isoMap->tileSet = malloc(sizeof(struct isoTileSetT));
    if(isoMap->tileSet == NULL)
    {
        writeToLog("Error in function: isoMapCreateEmptyMap() - Could not allocate memory for tile set!","runlog.txt");
        return NULL;
    }

    isoMap->tileSet->numTileClipRects = 0;
    isoMap->tileSet->tilesTex = NULL;

    isoMap->tileSet->tileClipRects = NULL;
    isoMap->tileSet->tileSetLoaded = 0;

    isoMap->mapHeight = height;
    isoMap->mapWidth = width;
    isoMap->numLayers = numLayers;

    if(mapName == NULL){
        sprintf(isoMap->name,mapName,"Unnamed map");
    }
    else
    {
        strncpy(isoMap->name,mapName,MAP_NAME_LENGTH-1);
    }
    //Divide the tile size by two
    isoMap->tileSize = tileSize/2;
    isoGenerateMap(isoMap,perlinSeed,terrainHeight);
    return isoMap;
}

void isoMapFreeMap(isoMapT *isoMap)
{
    if(isoMap != NULL)
    {
        if(isoMap->mapData!=NULL)
        {
            free(isoMap->mapData);
        }
        if(isoMap->tileSet!=NULL)
        {
            if(isoMap->tileSet->tileClipRects!=NULL){
                free(isoMap->tileSet->tileClipRects);
            }
            //Freeing textures is handled by the texture pool, so we don't
            //free the memory the texture pointer is pointing too.
            free(isoMap->tileSet);
        }
        free(isoMap);
    }
}

int isoMapLoadTileSet(isoMapT *isoMap,textureT *texture,int tileWidth,int tileHeight)
{
    int x=0,y=0;
    int w,h;
    int numTilesX;
    int numTilesY;
    int i = 0;
    SDL_Rect tmpRect;

    if(isoMap == NULL)
    {
        writeToLog("Error in function: isoMapLoadTileSet() - Parameter: 'isoMapT *isoMap' is NULL!","runlog.txt");
        return -1;
    }
    //if the texture is NULL
    if(texture == NULL){
        writeToLog("Error in function: isoMapLoadTileSet() - Parameter: 'textureT *texture' is NULL!","runlog.txt");
        return -1;
    }
    //if a tile set already has been loaded
    if(isoMap->tileSet->tileClipRects!=NULL){
        free(isoMap->tileSet->tileClipRects);
    }

    isoMap->tileSet->tilesTex = texture;

    //get width and height
    w = isoMap->tileSet->tilesTex->width;
    h = isoMap->tileSet->tilesTex->height;

    if(w<tileWidth){
        writeToLog("Error in function: isoMapLoadTileSet() - Texture width is smaller than the tile width! Aborting!","runlog.txt");
        return -1;
    }
    if(h<tileHeight){
        writeToLog("Error in function: isoMapLoadTileSet() - Texture height is smaller than the tile height! Aborting!","runlog.txt");
        return -1;
    }
    //calculate the number of tiles that fit in the texture
    numTilesX = floor(w/tileWidth);
    numTilesY = floor(h/tileHeight);

    //set the number of clip rectangles
    isoMap->tileSet->numTileClipRects = numTilesX * numTilesY;

    //allocate memory for the tile clip rectangles
    isoMap->tileSet->tileClipRects = (SDL_Rect*)malloc(sizeof(SDL_Rect)*isoMap->tileSet->numTileClipRects);

    //loop through the texture
    while(1)
    {
        //setup the clip rectangle
        setupRect(&tmpRect,x,y,tileWidth,tileHeight);

        //copy the rectangle
        isoMap->tileSet->tileClipRects[i] = tmpRect;

        //go to the next tile tile the image
        x+=tileWidth;

        //if the x position passed the width of the texture
        if(x>=w){
            //reset the x position
            x = 0;
            //go to the next row in the image
            y += tileHeight;
            if(y>=h){
                //break out of the while loop. No more clip rectangles to create.
                break;
            }
        }
        i++;
    }
    return 1;
}

int isoMapGetTile(isoMapT *isoMap,int x,int y,int layer)
{
    if(x < 0 || x > isoMap->mapWidth-1 || y < 0 || y > isoMap->mapHeight-1 || layer < 0 || layer > isoMap->numLayers){
        return -1;
    }
    return isoMap->mapData[(y * isoMap->mapWidth + x) * isoMap->numLayers + layer];
}

void isoMapSetTile(isoMapT *isoMap,int x,int y,int layer,int value)
{
    if(isoMap == NULL)
    {
        return;
    }

    if(x < 0 || x > isoMap->mapWidth-1 || y < 0 || y > isoMap->mapHeight-1 || layer < 0 || layer > isoMap->numLayers){
        return;
    }
    isoMap->mapData[(y * isoMap->mapWidth + x) * isoMap->numLayers + layer] = value;
}

static void rewriteNoiseMapTerrainHeight(isoMapT *isoMap, float *noiseMap, int truncateTerrainHeight)
{
    int x=0,y=0,value=0;

    //loop over the map
    for(y=0;y<isoMap->mapHeight;++y)
    {
        for(x=0;x<isoMap->mapWidth;++x)
        {
            //get the value from the noise map
            value = noiseMap[y*isoMap->mapWidth+x];

            //remap the height value
            if(value == truncateTerrainHeight)          { value = 6; }
            else if(value == truncateTerrainHeight-1)   { value = 6; }
            else if(value == truncateTerrainHeight-2)   { value = 5; }
            else if(value == truncateTerrainHeight-3)   { value = 4; }
            else if(value == truncateTerrainHeight-4)   { value = 3; }
            else if(value == truncateTerrainHeight-5)   { value = 2; }
            else if(value == truncateTerrainHeight-6)   { value = 1; }
            else { value = 0;}

            //store the value in the noise map
            noiseMap[y*isoMap->mapWidth+x] = value;
        }
    }

}

static void generatePerlinNoiseMap(isoMapT *isoMap, float *noiseMap,int perlinSeed,int truncateTerrainHeight)
{
    int x=0,y=0;
    float perlinHeightValue=0;
    int maxHeight=0;
    int value=0;

    //loop over the map
    for(y=0;y<isoMap->mapHeight;++y)
    {
        for(x=0;x<isoMap->mapWidth;++x)
        {
            perlinHeightValue = pnoise3d(y * 0.04, x * 0.04, 0, 0.02, 1, perlinSeed);
            value = (int)((perlinHeightValue + 1) * (truncateTerrainHeight * 0.5));

            //keep values within lower range
            if(value < 0) {
                value = 0;
            }
            //keep the highest height value
            if(value > maxHeight) {
                maxHeight = value;
            }
            //store the value in the noise map
            noiseMap[y*isoMap->mapWidth+x] = value;
        }
    }
    fprintf(stdout,"Maxheight:%d",maxHeight);
}

static void drawPerlinNoiseTerrain(isoMapT *isoMap,float *noiseMap)
{
    int x=0,y=0,value=0;

    //loop over the map
    for(y=0;y<isoMap->mapHeight;++y)
    {
        for(x=0;x<isoMap->mapWidth;++x)
        {
            value = -1 + noiseMap[y*isoMap->mapWidth+x];
            isoMapSetTile(isoMap,x,y,0,1 + ((NUM_TILES_PER_ROW_IN_TILESET * value))+15);
        }
    }
}

static void drawGreenGrass(isoMapT *isoMap)
{
    int x=0,y=0,value=0;

    //loop over the map
    for(y=0;y<isoMap->mapHeight;++y)
    {
        for(x=0;x<isoMap->mapWidth;++x)
        {
            value = isoMapGetTile(isoMap,x,y,0);
            if(value <0) {
                isoMapSetTile(isoMap,x,y,0,1);
            }
        }
    }
}

//This function checks if a tile is part of a terrain height in the tile set.
//Each row in the tile set is equal to a terrain height
static int tileIsWithinTerrainHeight(int tileValue,int terrainHeight)
{
    int minTileValue = terrainHeight * NUM_TILES_PER_ROW_IN_TILESET-1;
    int maxTileValue = minTileValue + NUM_TILES_PER_ROW_IN_TILESET-1;

    if(tileValue >= minTileValue && tileValue <= maxTileValue){
        return 1;
    }
    //if not, return -1
    return -1;
}

static int countNumberOfNeighbouringTiles(isoMapT *isoMap,int x,int y,int layer,int terrainHeight)
{
    int value = 0;
    int tileValue = -2;

    if(terrainHeight > NUM_TILE_LEVELS_PER_LAYER || terrainHeight <0) {
        return 0;
    }

    //if the tile above (up) is not empty
    tileValue = isoMapGetTile(isoMap,x,y-1,layer);
    if((tileIsWithinTerrainHeight(tileValue,terrainHeight) != -1)
    || (tileIsWithinTerrainHeight(tileValue,terrainHeight+1) != -1)) { value++; }

    //if the tile to the right is not empty
    tileValue = isoMapGetTile(isoMap,x+1,y,layer);
    if((tileIsWithinTerrainHeight(tileValue,terrainHeight) != -1)
    || (tileIsWithinTerrainHeight(tileValue,terrainHeight+1) != -1)) { value++; }

    //if the tile below is not empty
    tileValue = isoMapGetTile(isoMap,x,y+1,layer);
    if((tileIsWithinTerrainHeight(tileValue,terrainHeight) != -1)
    || (tileIsWithinTerrainHeight(tileValue,terrainHeight+1) != -1)) { value++; }

    //if the tile to the left is not empty
    tileValue = isoMapGetTile(isoMap,x-1,y,layer);
    if((tileIsWithinTerrainHeight(tileValue,terrainHeight) != -1)
    || (tileIsWithinTerrainHeight(tileValue,terrainHeight+1) != -1)) { value++; }

    return value;
}

static void deleteSingleTiles(isoMapT *isoMap,int layer,float *noiseMap)
{
    int numEmptyNeighbouringTiles = 0;
    int x=0,y=0,terrainHeight=0,value=0;

    //loop over the map
    for(y=0;y<isoMap->mapHeight;++y)
    {
        for(x=0;x<isoMap->mapWidth;++x)
        {
            for(terrainHeight = NUM_TILE_LEVELS_PER_LAYER; terrainHeight>=0; --terrainHeight)
            {
                //if the value is of the current terrain height
                if(tileIsWithinTerrainHeight(isoMapGetTile(isoMap,x,y,layer),terrainHeight) > -1) {
                    numEmptyNeighbouringTiles = countNumberOfNeighbouringTiles(isoMap,x,y,layer,terrainHeight);
                    if(numEmptyNeighbouringTiles<2) {
                        value = isoMapGetTile(isoMap,x,y,layer);
                        isoMapSetTile(isoMap,x,y,layer,value-NUM_TILES_PER_ROW_IN_TILESET);
                    }
                }
            }
        }
    }
}

static int bitWiseCalculateTile(isoMapT *isoMap,int x,int y,int layer,int terrainHeight)
{
    int value = 0;
    int tileValue = 0;

    //make sure terrain is within lower bounds
    if(terrainHeight <0 || terrainHeight > NUM_TILE_LEVELS_PER_LAYER) {
        return -2;
    }

    //if the tile above (up) is not empty
    tileValue = isoMapGetTile(isoMap,x,y-1,layer);
    if((tileIsWithinTerrainHeight(tileValue,terrainHeight) > -1)
    || (tileIsWithinTerrainHeight(tileValue,terrainHeight+1) > -1)) { value++; }

    //if the tile to the right is not empty
    tileValue = isoMapGetTile(isoMap,x+1,y,layer);
    if((tileIsWithinTerrainHeight(tileValue,terrainHeight) > -1)
    || (tileIsWithinTerrainHeight(tileValue,terrainHeight+1) > -1)) { value+=2; }

    //if the tile below is not empty
    tileValue = isoMapGetTile(isoMap,x,y+1,layer);
    if((tileIsWithinTerrainHeight(tileValue,terrainHeight) > -1)
    || (tileIsWithinTerrainHeight(tileValue,terrainHeight+1) > -1)) { value+=4; }

    //if the tile to the left is not empty
    tileValue = isoMapGetTile(isoMap,x-1,y,layer);
    if((tileIsWithinTerrainHeight(tileValue,terrainHeight) > -1)
    || (tileIsWithinTerrainHeight(tileValue,terrainHeight+1) >-1)) { value+=8; }

    return value;
}

static int bitWiseCalculateInnerTiles(isoMapT *isoMap,int x,int y,int layer,int terrainHeight)
{
    int value = -1;

    int tileOffset = (terrainHeight * NUM_TILES_PER_ROW_IN_TILESET);

    //since the autoTileTerrain() function adds 1 to the tile set it should draw, we find the tile we want in
    //the tile set, and subtract 1 from the value.

    //inner tiles up left
    if(isoMapGetTile(isoMap,x,y-1,layer) == 8 + tileOffset && isoMapGetTile(isoMap,x-1,y,layer) == 7 + tileOffset) { return 8; }
    else if(isoMapGetTile(isoMap,x,y-1,layer) == 8 + tileOffset && isoMapGetTile(isoMap,x-1,y,layer) == 15 +tileOffset) { return 8; }
    else if(isoMapGetTile(isoMap,x,y-1,layer) == 7 + tileOffset && isoMapGetTile(isoMap,x-1,y,layer) == 15 +tileOffset) { return 8; }
    else if(isoMapGetTile(isoMap,x,y-1,layer) == 7 + tileOffset && isoMapGetTile(isoMap,x-1,y,layer) == 7 +tileOffset) { return 8; }

    //inner tiles up right
    else if(isoMapGetTile(isoMap,x,y-1,layer) == 14 + tileOffset && isoMapGetTile(isoMap,x+1,y,layer) == 15 +tileOffset) { return 18; }
    else if(isoMapGetTile(isoMap,x,y-1,layer) == 14 + tileOffset && isoMapGetTile(isoMap,x+1,y,layer) == 13 +tileOffset) { return 18; }
    else if(isoMapGetTile(isoMap,x,y-1,layer) == 13 + tileOffset && isoMapGetTile(isoMap,x+1,y,layer) == 15 +tileOffset) { return 18; }
    else if(isoMapGetTile(isoMap,x,y-1,layer) == 13 + tileOffset && isoMapGetTile(isoMap,x+1,y,layer) == 13 +tileOffset) { return 18; }

    //inner tiles down left
    else if(isoMapGetTile(isoMap,x,y+1,layer) == 8 + tileOffset && isoMapGetTile(isoMap,x-1,y,layer) == 4 +tileOffset) { return 17; }
    else if(isoMapGetTile(isoMap,x,y+1,layer) == 4 + tileOffset && isoMapGetTile(isoMap,x-1,y,layer) == 4 +tileOffset) { return 17; }
    else if(isoMapGetTile(isoMap,x,y+1,layer) == 4 + tileOffset && isoMapGetTile(isoMap,x-1,y,layer) == 12 +tileOffset) { return 17; }
    else if(isoMapGetTile(isoMap,x,y+1,layer) == 8 + tileOffset && isoMapGetTile(isoMap,x-1,y,layer) == 12 +tileOffset) { return 17; }

    //inner tiles down right
    else if(isoMapGetTile(isoMap,x,y+1,layer) == 10 + tileOffset && isoMapGetTile(isoMap,x+1,y,layer) == 10 +tileOffset) { return 16; }
    else if(isoMapGetTile(isoMap,x,y+1,layer) == 10 + tileOffset && isoMapGetTile(isoMap,x+1,y,layer) == 12 +tileOffset) { return 16; }
    else if(isoMapGetTile(isoMap,x,y+1,layer) == 14 + tileOffset && isoMapGetTile(isoMap,x+1,y,layer) == 10 +tileOffset) { return 16; }
    else if(isoMapGetTile(isoMap,x,y+1,layer) == 14 + tileOffset && isoMapGetTile(isoMap,x+1,y,layer) == 12 +tileOffset) { return 16; }

    return -1;
}

static void autoTileInnerCornerTiles(isoMapT *isoMap, int layer)
{
    int x=0,y=0,value=0;
    int paintTile=0;
    int terrainHeight = 0;

    //loop over the map
    for(y=0;y<isoMap->mapHeight;++y)
    {
        for(x=0;x<isoMap->mapWidth;++x)
        {
            for(terrainHeight = NUM_TILE_LEVELS_PER_LAYER; terrainHeight>=0; --terrainHeight)
            {
                //if the value is one the right terrain height
                if(tileIsWithinTerrainHeight(isoMapGetTile(isoMap,x,y,layer),terrainHeight) > -1){
                    paintTile = bitWiseCalculateInnerTiles(isoMap,x,y,layer,terrainHeight);
                    if(paintTile>0){
                        isoMapSetTile(isoMap,x,y,layer,paintTile+1+(NUM_TILES_PER_ROW_IN_TILESET*terrainHeight));
                    }
                }
            }
        }
    }
}

static void autoTileTerrain(isoMapT *isoMap, int layer)
{
    int x=0,y=0,value=0;
    int paintTile=0;
    int terrainHeight = 0;

    //loop over the map
    for(y=0;y<isoMap->mapHeight;++y)
    {
        for(x=0;x<isoMap->mapWidth;++x)
        {
            for(terrainHeight = NUM_TILE_LEVELS_PER_LAYER; terrainHeight>=0; --terrainHeight)
            {
                //if the value is one the right terrain height
                if(tileIsWithinTerrainHeight(isoMapGetTile(isoMap,x,y,layer),terrainHeight) > -1){
                    paintTile = bitWiseCalculateTile(isoMap,x,y,layer,terrainHeight);
                    if(paintTile>0){
                        isoMapSetTile(isoMap,x,y,layer,paintTile+1+(NUM_TILES_PER_ROW_IN_TILESET*terrainHeight));
                    }
                }
            }
        }
    }
}

static void deleteTilesAtMapEdges(isoMapT *isoMap)
{
    int x=0,y=0,layer=0;

    //loop over the map
    for(y=0;y<isoMap->mapHeight;++y)
    {
        for(x=0;x<isoMap->mapWidth;++x)
        {
            for(layer = 0; layer<isoMap->numLayers;++layer)
            {
                if(x==0 || y==0 || x == isoMap->mapWidth-1 || y == isoMap->mapHeight-1)
                {
                    isoMapSetTile(isoMap,x,y,layer,-1);
                }
            }
        }
    }
}

static void correctMinorErrorsInSlopes(isoMapT *isoMap)
{
    int x=0,y=0,layer=0;
    int value=0;
    int paintTile=0;
    int tileHeight = 0;

    //loop over the map
    for(y=0;y<isoMap->mapHeight;++y)
    {
        for(x=0;x<isoMap->mapWidth;++x)
        {
            for(layer=0;layer<isoMap->numLayers;++layer)
            {
                for(tileHeight = 0; tileHeight < NUM_TILE_LEVELS_PER_LAYER; ++tileHeight)
                {
                    if(isoMapGetTile(isoMap,x,y,layer) == 4+(tileHeight*NUM_TILES_PER_ROW_IN_TILESET)
                    && isoMapGetTile(isoMap,x+1,y,layer) == 4+((tileHeight+1)*NUM_TILES_PER_ROW_IN_TILESET))
                    {
                        isoMapSetTile(isoMap,x+1,y,layer,3+((tileHeight+1)*NUM_TILES_PER_ROW_IN_TILESET));
                    }
                    if(isoMapGetTile(isoMap,x,y,layer) == 3+(tileHeight*NUM_TILES_PER_ROW_IN_TILESET)
                    && isoMapGetTile(isoMap,x+1,y,layer) == 4+((tileHeight+1)*NUM_TILES_PER_ROW_IN_TILESET))
                    {
                        isoMapSetTile(isoMap,x+1,y,layer,3+((tileHeight+1)*NUM_TILES_PER_ROW_IN_TILESET));
                    }

                    if(isoMapGetTile(isoMap,x,y,layer) == 10+((tileHeight)*NUM_TILES_PER_ROW_IN_TILESET)
                    && isoMapGetTile(isoMap,x,y-1,layer) == 14+(tileHeight)*NUM_TILES_PER_ROW_IN_TILESET
                    && isoMapGetTile(isoMap,x,y+1,layer) == 14+(tileHeight-1)*NUM_TILES_PER_ROW_IN_TILESET)
                    {
                        isoMapSetTile(isoMap,x,y,layer,5+((tileHeight+1)*NUM_TILES_PER_ROW_IN_TILESET));
                    }
                    if(isoMapGetTile(isoMap,x,y,layer) == 10+((tileHeight)*NUM_TILES_PER_ROW_IN_TILESET)
                    && isoMapGetTile(isoMap,x,y+1,layer) == 10+(tileHeight-1)*NUM_TILES_PER_ROW_IN_TILESET)
                    {
                        isoMapSetTile(isoMap,x,y,layer,5+((tileHeight+1)*NUM_TILES_PER_ROW_IN_TILESET));
                    }

                    if(isoMapGetTile(isoMap,x,y,layer) == 13+(tileHeight*NUM_TILES_PER_ROW_IN_TILESET)
                    && isoMapGetTile(isoMap,x-1,y,layer) == 13+((tileHeight+1)*NUM_TILES_PER_ROW_IN_TILESET))
                    {
                        isoMapSetTile(isoMap,x-1,y,layer,6+((tileHeight+1)*NUM_TILES_PER_ROW_IN_TILESET));
                    }
                    if(isoMapGetTile(isoMap,x,y,layer) == 7+(tileHeight*NUM_TILES_PER_ROW_IN_TILESET)
                    && isoMapGetTile(isoMap,x+1,y,layer) == 7+((tileHeight+1)*NUM_TILES_PER_ROW_IN_TILESET))
                    {
                        isoMapSetTile(isoMap,x+1,y,layer,2+((tileHeight+1)*NUM_TILES_PER_ROW_IN_TILESET));
                    }
                    if(isoMapGetTile(isoMap,x,y,layer) == 7+(tileHeight*NUM_TILES_PER_ROW_IN_TILESET)
                    && isoMapGetTile(isoMap,x,y+1,layer) == 7+((tileHeight+1)*NUM_TILES_PER_ROW_IN_TILESET))
                    {
                        isoMapSetTile(isoMap,x,y+1,layer,2+((tileHeight+1)*NUM_TILES_PER_ROW_IN_TILESET));
                    }
                    if(isoMapGetTile(isoMap,x,y,layer) == 2+(tileHeight*NUM_TILES_PER_ROW_IN_TILESET)
                    && isoMapGetTile(isoMap,x+1,y,layer) == 7+((tileHeight+1)*NUM_TILES_PER_ROW_IN_TILESET))
                    {
                        isoMapSetTile(isoMap,x+1,y,layer,2+((tileHeight+1)*NUM_TILES_PER_ROW_IN_TILESET));
                    }
                    if(isoMapGetTile(isoMap,x,y,layer) == 2+(tileHeight*NUM_TILES_PER_ROW_IN_TILESET)
                    && isoMapGetTile(isoMap,x,y+1,layer) == 7+((tileHeight+1)*NUM_TILES_PER_ROW_IN_TILESET))
                    {
                        isoMapSetTile(isoMap,x,y+1,layer,2+((tileHeight+1)*NUM_TILES_PER_ROW_IN_TILESET));
                    }
                }
            }
        }
    }
}


static void isoGenerateMap(isoMapT *isoMap, int perlinSeed, int terrainHeight)
{
    int x,y;
    int paintTile=0;

    //makes sure the map exists
    if(isoMap == NULL) {
        writeToLog("Error in function isoGenerateMap() - Parameter isoMap is NULL!","runlog.txt");
        return;
    }
    //allocate memory for the perlin noise map
    float *noiseMap = malloc(sizeof(float)*(isoMap->mapWidth * isoMap->mapHeight));

    //if memory allocation failed
    if(noiseMap == NULL){
        writeToLog("Error in function isoGenerateMap() - Could not allocate memory for the isometric map generation (noisemap).","runlog.txt");
        return;
    }

    //generate the perlin noise map
    generatePerlinNoiseMap(isoMap,noiseMap,perlinSeed,terrainHeight);
    rewriteNoiseMapTerrainHeight(isoMap,noiseMap,terrainHeight);
    drawPerlinNoiseTerrain(isoMap,noiseMap);
    deleteSingleTiles(isoMap,0,noiseMap);
    deleteSingleTiles(isoMap,0,noiseMap);

    autoTileTerrain(isoMap,0);
    autoTileInnerCornerTiles(isoMap,0);
    correctMinorErrorsInSlopes(isoMap);
    drawGreenGrass(isoMap);
    deleteTilesAtMapEdges(isoMap);


    /*
    isoMapSetTile(isoMap,0,3,1,2);
    isoMapSetTile(isoMap,5,3,1,2);
    isoMapSetTile(isoMap,4,3,1,2);
    isoMapSetTile(isoMap,3,3,1,2);
    isoMapSetTile(isoMap,2,3,1,2);
    isoMapSetTile(isoMap,1,3,1,2);

    for(y=0;y<isoMap->mapHeight;y+=2)
    {
        for(x=0;x<isoMap->mapWidth;x+=2)
        {
            isoMapSetTile(isoMap,x,y,0,1);
            isoMapSetTile(isoMap,x,y+1,0,1);
            isoMapSetTile(isoMap,x+1,y,0,1);
            isoMapSetTile(isoMap,x+1,y+1,0,1);
            paintTile = rand()%10;
            if(paintTile>8)
            {
                if(y<isoMap->mapHeight-4 && x<isoMap->mapWidth-4){
                    isoMapSetTile(isoMap,x,y,0,4);
                    isoMapSetTile(isoMap,x,y+1,0,4);
                    isoMapSetTile(isoMap,x+1,y,0,4);
                    isoMapSetTile(isoMap,x+1,y+1,0,4);
                }
            }
            if(paintTile==7){
                if(y<isoMap->mapHeight-4 && x<isoMap->mapWidth-4){
                    isoMapSetTile(isoMap,x,y,0,3);
                    isoMapSetTile(isoMap,x,y+1,0,3);
                    isoMapSetTile(isoMap,x+1,y,0,3);
                    isoMapSetTile(isoMap,x+1,y+1,0,3);
                }
            }
        }
    }*/
}
