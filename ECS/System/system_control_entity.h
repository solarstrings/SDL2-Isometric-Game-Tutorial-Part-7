#ifndef __CONTROL_ENTITY_SYSTEM_H__
#define __CONTROL_ENTITY_SYSTEM_H__

int systemControlEntityInit(void *scene);
void systemControlEntity();
void systemControlEntitySetEntityToControlByID(sceneT *scene,Uint32 entityID);
void systemControlEntitySetEntityToControlByNameTag(sceneT *scene,char *nameTag);
Uint32 systemControlEntityGetControlledEntity();
void systemFreeControlEntitySystem();
#endif // __CONTROL_ISOMETRIC_SYSTEM_H__

