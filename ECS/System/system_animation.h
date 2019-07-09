#ifndef __ANIMATION_SYSTEM_H_
#define __ANIMATION_SYSTEM_H_

int systemAnimationInit(void *scene);
void systemAnimationUpdate();
void systemAnimationUpdateEntity(Uint32 entity);
void systemAnimationFree();

#endif //__ANIMATION_SYSTEM_H__
