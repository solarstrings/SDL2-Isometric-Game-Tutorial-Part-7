#ifndef __SYSTEM_H__
#define __SYSTEM_H__
#include <SDL2/SDL.h>
#include "../Entity/entity.h"
#include "system_move.h"
#include "system_input.h"
#include "system_render_iso_world.h"
#include "system_control_iso_world.h"
#include "system_control_entity.h"
#include "system_collision.h"
#include "system_animation.h"

//function pointers
typedef int (*systemInitFuncPointer)(void *scene);
typedef void (*systemUpdateFuncPointer)();
typedef void (*systemUpdateEntityFuncPointer)(Uint32 entity);
typedef void (*systemFreeFuncPointer)();

// available systems
typedef enum systemTypeE
{
    SYSTEM_NONE     = 0,                    // no system
    SYSTEM_MOVE     = 1,                    // system for moving entities
    SYSTEM_INPUT    = 2,                    // system for handling input (keyboard, mouse & gamepad)
    SYSTEM_RENDER_ISOMETRIC_WORLD   = 3,    // system for rendering the isometric world
    SYSTEM_CONTROL_ISOMETRIC_WORLD  = 4,    // system for controlling the isometric world
    SYSTEM_CONTROL_ENTITY           = 5,    // system for controlling entities
    SYSTEM_COLLISION                = 6,    // system for collision detection and handling
    SYSTEM_ANIMATION                = 7,    // system for handling entity animations
}systemTypeE;

// system struct
typedef struct systemT {
    systemTypeE type;                           // what kind of system this is
    systemUpdateEntityFuncPointer updateEntity; // function pointer to run the system for an entity
    systemInitFuncPointer init;                 // function pointer to initialize the system
    systemUpdateFuncPointer update;             // function pointer to update the system
    systemFreeFuncPointer free;                 // function pointer to memory allocated by the system
}systemT;

#endif // __SYSTEM_H__
