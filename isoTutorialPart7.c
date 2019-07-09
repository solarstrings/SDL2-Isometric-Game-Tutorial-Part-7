/*
 *  Isometric Game Tutorial Part 7
 *  Author: Johan Forsblom
 *  Written: 2018-11
 *
 *  This tutorial covers the following:
 *
 *  Terrain Generation (Hills)
 *
 *  Usage:
 *  Space bar - toggle between Overview mode / Object focus mode
 *  Move the character with w,a,s,d
 *  Zoom in and out with the mouse wheel
 *
 *  Overview mode:
 *  Scroll map with the mouse close to the edges
 *
 *  Object focus mode & Overview mode:
 *  Left click on the map for "tile picking" (shows the selected tile up in the top left corner of the screen)
 *
 ******************************************************************************************************************
 *
 *   Copyright(C) 2018 Johan Forsblom
 *
 *   You may use the code for whatever reason you want. If you do a commercial project and took inspiration from or
 *   copied this code, all i require is a place in the credits section of your game/program :)
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 *   LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "initclose.h"
#include "renderer.h"
#include "texture.h"
#include "texture_pool.h"
#include "IsoEngine/isoEngine.h"
#include "ECS/Scene/scenemanager.h"
#include "logger.h"
#include "fontpool.h"

#define MAP_HEIGHT 640
#define MAP_WIDTH 640

typedef struct gameT
{
    sceneManagerT *sceneManager;
    texturePoolT *texturePool;
    fontPoolT *fontPool;
}gameT;

gameT game;

static componentInputKeyboardT *inputKeyboard = NULL;
static componentInputMouseT *inputMouse = NULL;
static componentNameTagT *nameTag = NULL;
static componentPositionT *position = NULL;
static componentRender2DT *render = NULL;
static componentVelocityT *velocity = NULL;
static componentCollisionT *collision = NULL;
static componentAnimationT *animationComp = NULL;

static void updateComponentPointers(sceneT *scene,int firstRun)
{
    if(scene->componentPointersRealloced == 1 || firstRun == 1)
    {
        ///get the pointers to the components
        //get the name tag components pointer from the scene
        nameTag = (componentNameTagT*)ecsSceneGetComponent(scene,COMPONENT_SET1_NAMETAG);
        //get the keyboard components pointer from the scene
        inputKeyboard = (componentInputKeyboardT*)ecsSceneGetComponent(scene,COMPONENT_SET1_KEYBOARD);
        //get the mouse components pointer from the scene
        inputMouse = (componentInputMouseT*)ecsSceneGetComponent(scene,COMPONENT_SET1_MOUSE);
        //get the position components pointer from the scene
        position = (componentPositionT*)ecsSceneGetComponent(scene,COMPONENT_SET1_POSITION);
        //get the render2D components pointer from the scene
        render = (componentRender2DT*)ecsSceneGetComponent(scene,COMPONENT_SET1_RENDER2D);
        //get the velocity components pointer from the scene
        velocity = (componentVelocityT*)ecsSceneGetComponent(scene,COMPONENT_SET1_VELOCITY);
        //get the collision components pointer from the scene
        collision = (componentCollisionT*)ecsSceneGetComponent(scene,COMPONENT_SET1_COLLISION);
        //get the animation components pointer from the scene
        animationComp = (componentAnimationT*)ecsSceneGetComponent(scene,COMPONENT_SET1_ANIMATION);
    }
}

void init()
{
    int entity;
    int entityAnimation;
    int i=0;
    char msg[512];
    SDL_Rect tmpRect;
    //set the log file directory
    setLoggerDirectory("logs");
    //delete the old log since the previous run
    deleteLog("runlog.txt");

    //Create a new pool to hold all textures
    game.texturePool = texturePoolNewTexturePool();
    //if memory allocation failed
    if(game.texturePool == NULL){
        //exit the program
        exit(-1);
    }
    game.fontPool = fontPoolNewFontPool(4);

    //if memory allocation failed
    if(game.fontPool == NULL) {
        texturePoolFreeTexturePool(game.texturePool);
        closeDownSDL();
        exit(-1);
    }

    ///Load the textures to the texture pool
    texturePoolAddTextureToTexturePool(game.texturePool,"data/isotiles.png");
    texturePoolAddTextureToTexturePool(game.texturePool,"data/character.png");
    texturePoolAddTextureToTexturePool(game.texturePool,"data/isotree.png");

    //set the font pool pointer to the game.fontPool so we can access it from other files
    fontPoolSetFontPoolPointer(game.fontPool);
    //add the fonts to the font pool
    fontPoolAddFont(game.fontPool,"data/cleanfont.png","cleanFont",9,9,7);
    fontPoolAddFont(game.fontPool,"data/nuFont_32x32.png","nuFont",32,32,34);
    fontPoolAddFont(game.fontPool,"data/8-bit_wonder_64x64.png","8bitWonderFont",64,64,64);
    fontPoolAddFont(game.fontPool,"data/bitmgothic_64x64.png","gothicFont",64,64,35);

    //it is possible to adjust the space for an individual character in the font as well.
    //Important, use single quotes ' ' and not double quotes " ", since it's a character and not a string.
    //The E character was hard to read at lower zoom levels, so adding extra space to it, made it readable
    fontPoolSetCharacterFontSpace(game.fontPool,"nuFont",'E',38);
    //The L character had too much space behind it
    fontPoolSetCharacterFontSpace(game.fontPool,"nuFont",'L',30);

    game.sceneManager = ecsSceneManagerCreateNewSceneManager();
    if(game.sceneManager == NULL){
        texturePoolFreeTexturePool(game.texturePool);
        exit(-1);
    }
    ecsSceneManagerCreateSceneInSceneManager(game.sceneManager,"testScene");
    sceneT *testScene = ecsSceneManagerGetPointerToScene(game.sceneManager,"testScene");
    if(testScene == NULL){
        exit(-1);
    }

///ADD COMPONENTS TO THE SCENE
    ecsSceneAddComponentToScene(testScene,COMPONENT_SET1_POSITION);
    ecsSceneAddComponentToScene(testScene,COMPONENT_SET1_VELOCITY);
    ecsSceneAddComponentToScene(testScene,COMPONENT_SET1_KEYBOARD);
    ecsSceneAddComponentToScene(testScene,COMPONENT_SET1_MOUSE);
    ecsSceneAddComponentToScene(testScene,COMPONENT_SET1_RENDER2D);
    ecsSceneAddComponentToScene(testScene,COMPONENT_SET1_NAMETAG);
    ecsSceneAddComponentToScene(testScene,COMPONENT_SET1_COLLISION);
    ecsSceneAddComponentToScene(testScene,COMPONENT_SET1_ANIMATION);

    //if there was a memory allocation problem anywhere in the scene
    if(testScene->memallocFailed == 1){
        //free the scene and log the error
        sprintf(msg,"Memory allocation error(s) while setting up scene:%s, aborting...",testScene->name);
        writeToLog(msg,"runlog.txt");
        ecsSceneFreeScene(testScene);
        exit(-1);
    }
    updateComponentPointers(testScene,1);

///ADD SYSTEMS TO THE SCENE
    ecsSceneAddSystemToScene(testScene,SYSTEM_INPUT);
    ecsSceneAddSystemToScene(testScene,SYSTEM_MOVE);
    ecsSceneAddSystemToScene(testScene,SYSTEM_COLLISION);
    ecsSceneAddSystemToScene(testScene,SYSTEM_ANIMATION);
    ecsSceneAddSystemToScene(testScene,SYSTEM_RENDER_ISOMETRIC_WORLD);      //Run render system before isometric control system
    ecsSceneAddSystemToScene(testScene,SYSTEM_CONTROL_ISOMETRIC_WORLD);     //Otherwise funny artifacts can occur for drawing
    ecsSceneAddSystemToScene(testScene,SYSTEM_CONTROL_ENTITY);

/// ISOMETRIC CONTROL ENTITY
    //Add isometric control entity to the scene
    entity = ecsSceneAddEntityToScene(testScene,
             COMPONENT_SET1_KEYBOARD | COMPONENT_SET1_NAMETAG | COMPONENT_SET1_MOUSE);

    //every time we add a new entity to the scene, we have to check if the component pointers should be updated
    updateComponentPointers(testScene,0);

    //set the name of the entity
    componentNameTagSetName(nameTag,entity,"isometricControls");

    //add the arrow direction keys to the isometric controls entity
    componentInputKeyboardAddAction(inputKeyboard,entity,"up",SDL_SCANCODE_UP);
    componentInputKeyboardAddAction(inputKeyboard,entity,"down",SDL_SCANCODE_DOWN);
    componentInputKeyboardAddAction(inputKeyboard,entity,"left",SDL_SCANCODE_LEFT);
    componentInputKeyboardAddAction(inputKeyboard,entity,"right",SDL_SCANCODE_RIGHT);

    //add mouse actions to the isometric control entity
    componentInputMouseAddAction(inputMouse,entity,"mouseWheel",COMPONENT_INPUTMOUSE_ACTION_MOUSEWHEEL);
    componentInputMouseAddAction(inputMouse,entity,"leftButton",COMPONENT_INPUTMOUSE_ACTION_LEFTBUTTON);
    componentInputMouseAddAction(inputMouse,entity,"rightButton",COMPONENT_INPUTMOUSE_ACTION_RIGHTBUTTON);
    componentInputMouseAddAction(inputMouse,entity,"middleButton",COMPONENT_INPUTMOUSE_ACTION_MIDDLEBUTTON);
    componentInputKeyboardAddAction(inputKeyboard,entity,"toggleGameMode",SDL_SCANCODE_SPACE);

    //activate the mouse input
    componentInputMouseSetActiveState(inputMouse,entity,1);
    //Activate keyboard input (otherwise input won't be applied on the component)
    componentInputKeyboardSetActiveState(inputKeyboard,entity,1);
//// END OF ISOMETRIC CONTROL ENTITY

/// PLAYER ENTITY
    //Add the player entity to the scene
    entity = ecsSceneAddEntityToScene(testScene,
             COMPONENT_SET1_POSITION | COMPONENT_SET1_VELOCITY |
             COMPONENT_SET1_NAMETAG | COMPONENT_SET1_KEYBOARD |
             COMPONENT_SET1_RENDER2D | COMPONENT_SET1_COLLISION |
             COMPONENT_SET1_ANIMATION);

    //every time we add a new entity to the scene, we have to check if the component pointers should be updated
    updateComponentPointers(testScene,0);

    //set the name of the entity
    componentNameTagSetName(nameTag,entity,"player1");
    //set the player start position
    componentPositionSetPosition(position,entity,0,0);
    //move the player y position up a bit
    componentPositionSetOffset(position,entity,0,-38);
    //set friction to 100
    componentVelocitySetFriction(velocity,entity,100);

    //set keys to control the player
    componentInputKeyboardAddAction(inputKeyboard,entity,"up",SDL_SCANCODE_W);
    componentInputKeyboardAddAction(inputKeyboard,entity,"down",SDL_SCANCODE_S);
    componentInputKeyboardAddAction(inputKeyboard,entity,"left",SDL_SCANCODE_A);
    componentInputKeyboardAddAction(inputKeyboard,entity,"right",SDL_SCANCODE_D);

    //enable the keyboard input for the player entity
    componentInputKeyboardSetActiveState(inputKeyboard,entity,1);

    //tell the entity control system to work on the player entity
    systemControlEntitySetEntityToControlByID(testScene,entity);

    //set the kind of collisions the entity will handle
    componentCollisionSetCollisionType(collision,entity,COLLISIONTYPE_WORLD_AND_ENTITY);
    setupRect(&tmpRect,0,0,8,8);
    componentCollisionSetCollisionRectange(collision,entity,&tmpRect);

    //set the clip rectangle (forward frame of the character)
    setupRect(&tmpRect,350,0,70,70);

    //set the texture to render
    componentRender2DSetTextureAndClipRect(render,entity,texturePoolGetTextureFromPool(game.texturePool,"character.png"),&tmpRect);
    componentPositionSetOffset(position,entity,0,-4);
    componentRender2DSetLayer(render,entity,1);

    // IDLE ANIMATIONS - CURRENTLY JUST 1 FRAME

    entityAnimation = componentAnimationCreateNewAnimation(animationComp,entity,texturePoolGetTextureFromPool(game.texturePool,"character.png"),"idleUpLeft");
    componentAnimationAddAnimationFrames(animationComp,entity,entityAnimation,70,70,1,0,100);
    entityAnimation = componentAnimationCreateNewAnimation(animationComp,entity,texturePoolGetTextureFromPool(game.texturePool,"character.png"),"idleUp");
    componentAnimationAddAnimationFrames(animationComp,entity,entityAnimation,70,70,1,1,100);
    entityAnimation = componentAnimationCreateNewAnimation(animationComp,entity,texturePoolGetTextureFromPool(game.texturePool,"character.png"),"idleUpRight");
    componentAnimationAddAnimationFrames(animationComp,entity,entityAnimation,70,70,1,2,100);
    entityAnimation = componentAnimationCreateNewAnimation(animationComp,entity,texturePoolGetTextureFromPool(game.texturePool,"character.png"),"idleRight");
    componentAnimationAddAnimationFrames(animationComp,entity,entityAnimation,70,70,1,3,100);
    entityAnimation = componentAnimationCreateNewAnimation(animationComp,entity,texturePoolGetTextureFromPool(game.texturePool,"character.png"),"idleDownRight");
    componentAnimationAddAnimationFrames(animationComp,entity,entityAnimation,70,70,1,4,100);
    entityAnimation = componentAnimationCreateNewAnimation(animationComp,entity,texturePoolGetTextureFromPool(game.texturePool,"character.png"),"idleDown");
    componentAnimationAddAnimationFrames(animationComp,entity,entityAnimation,70,70,1,5,100);
    entityAnimation = componentAnimationCreateNewAnimation(animationComp,entity,texturePoolGetTextureFromPool(game.texturePool,"character.png"),"idleDownLeft");
    componentAnimationAddAnimationFrames(animationComp,entity,entityAnimation,70,70,1,6,100);
    entityAnimation = componentAnimationCreateNewAnimation(animationComp,entity,texturePoolGetTextureFromPool(game.texturePool,"character.png"),"idleLeft");
    componentAnimationAddAnimationFrames(animationComp,entity,entityAnimation,70,70,1,7,100);

    // WALK ANIMATIONS
    entityAnimation = componentAnimationCreateNewAnimation(animationComp,entity,texturePoolGetTextureFromPool(game.texturePool,"character.png"),"walkDown");
    componentAnimationAddAnimationFrames(animationComp,entity,entityAnimation,70,70,8,8,100);
    entityAnimation = componentAnimationCreateNewAnimation(animationComp,entity,texturePoolGetTextureFromPool(game.texturePool,"character.png"),"walkUp");
    componentAnimationAddAnimationFrames(animationComp,entity,entityAnimation,70,70,8,16,100);
    entityAnimation = componentAnimationCreateNewAnimation(animationComp,entity,texturePoolGetTextureFromPool(game.texturePool,"character.png"),"walkLeft");
    componentAnimationAddAnimationFrames(animationComp,entity,entityAnimation,70,70,8,24,100);
    entityAnimation = componentAnimationCreateNewAnimation(animationComp,entity,texturePoolGetTextureFromPool(game.texturePool,"character.png"),"walkRight");
    componentAnimationAddAnimationFrames(animationComp,entity,entityAnimation,70,70,8,32,100);
    entityAnimation = componentAnimationCreateNewAnimation(animationComp,entity,texturePoolGetTextureFromPool(game.texturePool,"character.png"),"walkDownLeft");
    componentAnimationAddAnimationFrames(animationComp,entity,entityAnimation,70,70,8,40,100);
    entityAnimation = componentAnimationCreateNewAnimation(animationComp,entity,texturePoolGetTextureFromPool(game.texturePool,"character.png"),"walkDownRight");
    componentAnimationAddAnimationFrames(animationComp,entity,entityAnimation,70,70,8,48,100);
    entityAnimation = componentAnimationCreateNewAnimation(animationComp,entity,texturePoolGetTextureFromPool(game.texturePool,"character.png"),"walkUpRight");
    componentAnimationAddAnimationFrames(animationComp,entity,entityAnimation,70,70,8,56,100);
    entityAnimation = componentAnimationCreateNewAnimation(animationComp,entity,texturePoolGetTextureFromPool(game.texturePool,"character.png"),"walkUpLeft");
    componentAnimationAddAnimationFrames(animationComp,entity,entityAnimation,70,70,8,64,100);

    //set the animation state to idle down
    componentAnimationSetAnimationState(animationComp,entity,"idleDown");
    animationComp[entity].direction = ENTITY_WORLD_DIRECTION_DOWN;

//// END OF PLAYER ENTITY

/// TREES ON THE MAP!

    writeToLog("Adding trees to scene...","runlog.txt");
    for(i=0;i<1000;++i)
    {
        //Add a tree entity to the scene
        entity = ecsSceneAddEntityToScene(testScene,
             COMPONENT_SET1_POSITION | COMPONENT_SET1_NAMETAG | COMPONENT_SET1_RENDER2D | COMPONENT_SET1_VELOCITY | COMPONENT_SET1_COLLISION);

        //every time we add a new entity to the scene, we have to check if the component pointers should be updated
        updateComponentPointers(testScene,0);

        //give the tree a name
        sprintf(msg,"tree %d",i);
        componentNameTagSetName(nameTag,entity,msg);

        //set the tree start position
        componentPositionSetPosition(position,entity,rand()%MAP_WIDTH*32,rand()%MAP_HEIGHT*32);

        //componentVelocitySetVelocity(velocity,entity,10+rand()%100,10+rand()%100);
        componentVelocitySetFriction(velocity,entity,0);
        //move the tree offset y position up a bit
        componentPositionSetOffset(position,entity,0,-96);

        //create the collision rectangle for the tree
        setupRect(&tmpRect,0,0,20,20);
        componentCollisionSetCollisionRectange(collision,entity,&tmpRect);

        //set the texture to render
        componentRender2DSetTextureAndClipRect(render,entity,texturePoolGetTextureFromPool(game.texturePool,"isotree.png"),NULL);
        componentRender2DSetLayer(render,entity,1);
    }

    //Setup the isometric engine
    testScene->isoEngine = isoEngineNewIsoEngine();
    if(testScene->isoEngine == NULL){
        ecsSceneManagerFreeSceneManager(game.sceneManager);
        closeDownSDL();
        exit(1);
    }
    //create an empty map
    testScene->isoEngine->isoMap = isoMapCreateNewMap("Testmap",MAP_WIDTH,MAP_HEIGHT,2,64,1232,20);
    if(testScene->isoEngine->isoMap == NULL){
        ecsSceneManagerFreeSceneManager(game.sceneManager);
        closeDownSDL();
        exit(1);
    }
    //load the isometric tile set from the texture pool
    isoMapLoadTileSet(testScene->isoEngine->isoMap,texturePoolGetTextureFromPool(game.texturePool,"isotiles.png"),64,80);

    //set isometric game mode to focus at the selected entity
    isoEngineSetGameMode(testScene->isoEngine,GAME_MODE_OBJECT_FOCUS);
}


int main(int argc, char *argv[])
{
    //get rid of compiler warnings for argc and argv
    argv = argv;
    (void)argc;

    initSDL("Isometric Game Tutorial - Part 7 - By Johan Forsblom");
    init();

    SDL_ShowCursor(0);
    SDL_SetWindowGrab(getWindow(),SDL_TRUE);
    SDL_WarpMouseInWindow(getWindow(),WINDOW_WIDTH/2,WINDOW_HEIGHT/2);

    ecsSceneManagerSetActiveScene(game.sceneManager, "testScene");
    ecsSceneManagerRunActiveScene(game.sceneManager);
    ecsSceneManagerFreeSceneManager(game.sceneManager);
    texturePoolFreeTexturePool(game.texturePool);
    fontPoolFreeFontPool(game.fontPool);
    closeDownSDL();
    return 0;
}
