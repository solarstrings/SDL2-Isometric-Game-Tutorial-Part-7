#ifndef __MOVE_SYSTEM_H__
#define __MOVE_SYSTEM_H__

int systemMoveInit(void *scene);
void systemMoveUpdate();
void systemMoveUpdateEntity(Uint32 entity);
void systemFreeMoveSystem();
#endif // __MOVE_SYSTEM_H__
