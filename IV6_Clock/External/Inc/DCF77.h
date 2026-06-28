/*
 * DCF77.h
 *
 *  Created on: 2026. jún. 9.
 *      Author: jgabo
 */

#ifndef INC_DCF77_H_
#define INC_DCF77_H_

#include "main.h"
#include "stdbool.h"

typedef struct {
    uint8_t hour;
    uint8_t minute;
    uint8_t day;
    uint8_t month;
    uint8_t year;
    bool    is_updated;
} DCF77_Time_t;

extern volatile DCF77_Time_t dcf_latest_time;

void DCF77_InterruptHandler(GPIO_PinState pin_state, uint32_t current_time);
void DCF77_ProcessBuffer(uint64_t buffer);

#endif /* INC_DCF77_H_ */
