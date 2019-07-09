#ifndef __ISOENGINE_H_
#define __ISOENGINE_H_
#include <SDL2/SDL.h>
#include "isoMap.h"

typedef enum isoEngineGameModeE
{
        GAME_MODE_OVERVIEW = 0,
        GAME_MODE_OBJECT_FOCUS,
        NUM_GAME_MODES,
}isoEngineGameModeE;

typedef struct point2DT
{
    float x;
    float y;
}point2DT;

typedef struct isoEngineT
{
    int scrollX;
    int scrollY;
    int mapScrollSpeed;
    point2DT mapScroll2Dpos;
    float zoomLevel;
    SDL_Rect mouseRect;
    point2DT mousePoint;
    point2DT tilePos;
    int lastTileClicked;
    isoMapT *isoMap;
    int gameMode;
}isoEngineT;

isoEngineT *isoEngineNewIsoEngine();
void isoEngineInit(isoEngineT *isoEngine, int tileSizeInPixels);
void isoEngineFreeIsoEngine(isoEngineT *isoEngine);
void IsoEngineSetMapSize(isoEngineT *isoEngine,int width, int height);
void isoEngineConvert2dToIso(point2DT *point);
void isoEngineConvertIsoTo2D(point2DT *point);
void isoEngineGetTileCoordinates(isoEngineT *isoEngine,point2DT *point,point2DT *point2DCoord);

void isoEngineConvertIsoCameraToCartesian(isoEngineT *isoEngine,point2DT *cartesianCamPos);
void isoEngineConvertCartesianCameraToIsometric(isoEngineT *isoEngine,point2DT *cartesianCamPos);

void isoEngineConvertIsoPoint2DToCartesian(isoEngineT *isoEngine,point2DT *isoPoint,point2DT *cartesianPoint);

void isoEngineUpdateMousePos(isoEngineT *isoEngine);
void isoEngineScrollMapWithMouse(isoEngineT *isoEngine);
void isoEngineDrawIsoMouse(isoEngineT *isoEngine);
void isoEngineDrawIsoMap(isoEngineT *isoEngine);
void isoEngineGetMouseTilePos(isoEngineT *isoEngine, point2DT *mouseTilePos);
void isoEngineCenterMapToTileUnderMouse(isoEngineT *isoEngine);
void isoEngineCenterMap(isoEngineT *isoEngine,point2DT *objectPoint,point2DT *objSize);
void isoEngineGetMouseTileClick(isoEngineT *isoEngine);
void isoEngineZoomIn(isoEngineT *isoEngine);
void isoEngineZoomOut(isoEngineT *isoEngine);
void isoEngineSetGameMode(isoEngineT *isoEngine,isoEngineGameModeE gameMode);

#endif // __ISOENGINE_H_
