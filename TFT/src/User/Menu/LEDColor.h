#ifndef _LED_COLOR_H_
#define _LED_COLOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// data structures
#define LED_VECT_SIZE 6

// LED color components vector:
// R: red
// G: green
// B: blue
// W: white;     NEOPIXEL or RGB(W)
// P: intensity; NEOPIXEL
// I: index;     NEOPIXEL
typedef uint8_t LED_VECT[LED_VECT_SIZE];

void restoreLEDValues(void);
void menuLEDColor(void);

#ifdef __cplusplus
}
#endif

#endif
