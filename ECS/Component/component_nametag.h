#ifndef __COMPONENT_NAMETAG_H_
#define __COMPONENT_NAMETAG_H_

typedef struct componentNameTagT
{
    char *name;
}componentNameTagT;

componentNameTagT *componentNewNameTag();
void componentNameTagAllocateMoreMemory(sceneT *scene,int componentIndex);
void componentNameTagSetName(componentNameTagT *nameTagComponent,Uint32 entity,char *name);
void componentFreeNameTag(componentNameTagT *nameTagComponent,Uint32 numEntities);
int componentNameTagGetEntityIDFromEntityByName(componentNameTagT *nameTagComponents,char *entityName,Uint32 numEntities);
#endif //__COMPONENT_NAMETAG_H_
