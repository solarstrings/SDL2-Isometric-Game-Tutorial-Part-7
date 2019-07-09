#ifndef __COMPONENT_H_
#define __COMPONENT_H_

#include "component_position.h"
#include "component_velocity.h"
#include "component_render2D.h"
#include "component_inputkeyboard.h"
#include "component_inputmouse.h"
#include "component_nametag.h"
#include "component_collision.h"
#include "component_animation.h"

// components types.
// Each component is a bit-mask in an unsigned int.
// The limit is 32 bits / unsigned int

typedef enum componentTypeE
{
    //  ___________________________________________________
    // | IMPORTANT!!                                       |
    // | Every time you add a component to this list       |
    // | you must also increase COMPONENT_COUNT with 1     |
    // |___________________________________________________|
    //
    COMPONENT_TYPE_COUNT        = 8,        // number of components
    COMPONENT_NONE              = 0,        //

    //Point each component to one bit in the unsigned int. ( X = the bit the component is pointing to )
    // These are connected to the Uint32 componentSet1 in the entityT struct
    COMPONENT_SET1_RENDER2D     = 1u << 0,  // binary: 0000 0000 0000 0000 0000 0000 0000 000X
    COMPONENT_SET1_KEYBOARD     = 1u << 1,  // binary: 0000 0000 0000 0000 0000 0000 0000 00X0
    COMPONENT_SET1_MOUSE        = 1u << 2,  // binary: 0000 0000 0000 0000 0000 0000 0000 0X00
    COMPONENT_SET1_POSITION     = 1u << 3,  // binary: 0000 0000 0000 0000 0000 0000 0000 X000
    COMPONENT_SET1_VELOCITY     = 1u << 4,  // binary: 0000 0000 0000 0000 0000 0000 000X 0000
    COMPONENT_SET1_NAMETAG      = 1u << 5,  // binary: 0000 0000 0000 0000 0000 0000 00X0 0000
    COMPONENT_SET1_COLLISION    = 1u << 6,  // binary: 0000 0000 0000 0000 0000 0000 0X00 0000
    COMPONENT_SET1_ANIMATION    = 1u << 7,  // binary: 0000 0000 0000 0000 0000 0000 X000 0000

    // These are connected to the Uint32 componentSet2 in the entityT struct
    //COMPONENT_SET2_UNKNOWN1       = 1u << 0,   // binary: 0000 0000 0000 0000 0000 0000 0000 000X
    //COMPONENT_SET2_UNKNOWN2       = 1u << 1,   // binary: 0000 0000 0000 0000 0000 0000 0000 00X0
    //COMPONENT_SET2_UNKNOWN3       = 1u << 2,   // binary: 0000 0000 0000 0000 0000 0000 0000 0X00
    //COMPONENT_SET2_UNKNOWN4       = 1u << 3,   // binary: 0000 0000 0000 0000 0000 0000 0000 X000
    //etc..
}componentTypeE;

// component struct
typedef struct componentT {
  componentTypeE type;      //the type of component it is
  void        *data;        //the component data
}componentT;

#endif // __COMPONENT_H_
