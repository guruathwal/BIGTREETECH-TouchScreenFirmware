#ifndef _SCREEN_SAVER_H_
#define _SCREEN_SAVER_H_

#include  <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void blockScreensaver(bool state);
void loopScreensaver(void);
void menuScreenSaver(void);

#ifdef __cplusplus
}
#endif

#endif
