#ifndef __ISO_MAP_H_
#define __ISO_MAP_H_

#include <SDL2/SDL.h>
#include "../texture.h"

#define MAP_NAME_LENGTH 50
#define NUM_TILES_PER_ROW_IN_TILESET    23
#define NUM_TILE_LEVELS_PER_LAYER       6

typedef struct isoTileSetT
{
    int tileSetLoaded;
    int numTileClipRects;
    textureT *tilesTex;
    SDL_Rect *tileClipRects;
}isoTileSetT;

typedef struct isoMapT
{
    int mapHeight;
    int mapWidth;
    int numLayers;
    int tileSize;
    int tileSizeX;
    int tileSizeY;
    int *mapData;
    char name[MAP_NAME_LENGTH];
    isoTileSetT *tileSet;
}isoMapT;

isoMapT* isoMapCreateNewMap(char *mapName,int width,int height,int numLayers,int tileSize,int perlinSeed, int terrainHeight);
void isoMapFreeMap(isoMapT *isoMap);
int isoMapLoadTileSet(isoMapT *isoMap,textureT *texture,int tileWidth,int tileHeight);
int isoMapGetTile(isoMapT *isoMap,int x,int y,int layer);
void isoMapSetTile(isoMapT *isoMap,int x,int y,int layer,int value);

#endif // __ISO_MAP_H_

