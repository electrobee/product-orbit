#ifndef TURNTABLE_H
#define TURNTABLE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TT_STEPS_PER_CIRCLE	438

typedef enum {
	TT_OK,
	TT_NOT_CONNECTED,
	TT_NOT_RESPONDING
} tt_error_t;

tt_error_t ttError();
void ttClearError();

tt_error_t ttOpen();
void ttClose();

int16_t ttGetPosition();
int16_t ttGetTarget();
void ttSetPosition(int16_t position);
void ttSetTarget(int16_t target);
void ttSetV0(uint8_t v0); // initial speed 0..255
void ttSetVmax(uint8_t vmax); // max speed 0..255
void ttStop();

#ifdef __cplusplus
}
#endif

#endif /* TURNTABLE_H */
