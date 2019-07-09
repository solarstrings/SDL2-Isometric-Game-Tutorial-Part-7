#ifndef __COMPONENT_INPUT_MOUSE_H_
#define __COMPONENT_INPUT_MOUSE_H_
#include <SDL2/SDL.h>

#define COMPONENT_INPUTMOUSE_STATE_RELEASED  0
#define COMPONENT_INPUTMOUSE_STATE_PRESSED   1
#define COMPONENT_INPUTMOUSE_STATE_MOUSE_WHEEL_NONE      0
#define COMPONENT_INPUTMOUSE_STATE_MOUSEWHEEL_UP        1
#define COMPONENT_INPUTMOUSE_STATE_MOUSEWHEEL_DOWN      2

//forward declaration of sceneT, allows us to use the sceneT struct without causing a cross-referencing header error
typedef struct sceneT sceneT;

//Enum of mouse input actions
typedef enum inputMouseActionE
{
    COMPONENT_INPUTMOUSE_ACTION_LEFTBUTTON      = 0,
    COMPONENT_INPUTMOUSE_ACTION_MIDDLEBUTTON    = 1,
    COMPONENT_INPUTMOUSE_ACTION_RIGHTBUTTON     = 2,
    COMPONENT_INPUTMOUSE_ACTION_MOUSEWHEEL      = 3,
}inputMouseActionE;

typedef struct inputMouseActionT
{
    char *name;                     //name of the action
    char state;                     //current state of the action
    char oldState;                  //last state
    inputMouseActionE mouseAction;  //the mouse action
}inputMouseActionT;

typedef struct componentInputMouseT
{
    int numActions;                 //number of mouse actions
    int maxActions;                 //max number of mouse actions
    inputMouseActionT *actions;     //pointer to the mouse actions
    char active;                    //if the mouse component is active
}componentInputMouseT;

componentInputMouseT *componentNewInputMouse();
void componentInputMouseAllocateMoreMemory(sceneT *scene,int componentIndex);
void componentFreeInputMouse(componentInputMouseT *componentInputMouse,int numEntities);
void componentInputMouseSetActiveState(componentInputMouseT *componentInputMouse,Uint32 entity,int value);
void componentInputMouseAddAction(componentInputMouseT *componentInputMouse,int entity,char *name,inputMouseActionE mouseAction);
int componentInputMouseGetActionIndex(componentInputMouseT *componentInputMouse,Uint32 entity,char *actionName);

#endif // __COMPONENT_INPUT_MOUSE_H_



