#ifndef __COLLISION_SYSTEM_H__
#define __COLLISION_SYSTEM_H__

int systemCollisionInit(void *scene);
void systemCollisionUpdate();
void systemCollisionUpdateEntity(Uint32 entity);
void systemCollisionFree();
int systemCollisionBoundingBoxCollision(SDL_Rect a, SDL_Rect b);

#endif // __COLLISION_SYSTEM_H__
