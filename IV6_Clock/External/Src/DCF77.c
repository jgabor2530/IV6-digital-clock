/*
 * DCF77.c
 *
 *  Created on: 2026. jún. 9.
 *      Author: Juhász Gábor
 */

#include "DCF77.h"

static volatile uint32_t PulseStart = 0;
static volatile uint32_t PulseLength = 0;
static volatile uint32_t GapLength = 0;
static volatile uint64_t Buffer = 0;
static volatile uint32_t BitIndex = 0;
static volatile bool Synced = 0;
volatile DCF77_Time_t dcf_latest_time = {0};

static volatile uint32_t dcf_period = 0;
static volatile uint32_t dcf_last_edge = 0;

static bool DCF77_CheckParity(uint64_t buffer, uint8_t start_bit, uint8_t end_bit, uint8_t parity_bit) {

	uint8_t ones_count = 0;

    for (uint8_t i = start_bit; i <= end_bit; i++) {
        if ((buffer >> i) & 1) {
            ones_count++;
        }
    }
    uint8_t expected_parity = (buffer >> parity_bit) & 1;

    return (ones_count % 2) == expected_parity;
}

void DCF77_InterruptHandler(GPIO_PinState pin_state, uint32_t current_time){

	if(pin_state == GPIO_PIN_SET){

		GapLength = current_time - PulseStart;

		if(GapLength > 1500 && GapLength < 2500){

			Synced = true;
			BitIndex = 0;
			Buffer = 0;

		}
		dcf_period = current_time - dcf_last_edge;
		dcf_last_edge = current_time;
		PulseStart = current_time;
	}
	else{
		PulseLength = current_time-PulseStart;

		if(Synced == true){
			if(PulseLength < 70 && PulseLength > 130){
				BitIndex++;
			}
			else if(PulseLength < 170 && PulseLength > 230){
				Buffer |= ((uint64_t)1 << BitIndex);
				BitIndex++;
			}
			else{
				Synced = false;
			}

		if(BitIndex == 59){
			DCF77_ProcessBuffer(Buffer);
			Synced = false;
			}
		}
	}
}

void DCF77_ProcessBuffer(uint64_t buffer){
	    if (((buffer >> 0) & 1) != 0) return;
	    if (((buffer >> 20) & 1) != 1) return;

	    if (!DCF77_CheckParity(buffer, 21, 27, 28)) return;
	    if (!DCF77_CheckParity(buffer, 29, 34, 35)) return;
	    if (!DCF77_CheckParity(buffer, 36, 57, 58)) return;

	    uint8_t min_bcd = (buffer >> 21) & 0x7F;
	    uint8_t min_dec = (min_bcd & 0x0F) + ((min_bcd >> 4) * 10);

	    uint8_t hour_bcd = (buffer >> 29) & 0x3F;
	    uint8_t hour_dec = (hour_bcd & 0x0F) + ((hour_bcd >> 4) * 10);

	    uint8_t day_bcd = (buffer >> 36) & 0x3F;
	    uint8_t day_dec = (day_bcd & 0x0F) + ((day_bcd >> 4) * 10);

	    uint8_t month_bcd = (buffer >> 45) & 0x1F;
	    uint8_t month_dec = (month_bcd & 0x0F) + ((month_bcd >> 4) * 10);

	    uint8_t year_bcd = (buffer >> 50) & 0xFF;
	    uint8_t year_dec = (year_bcd & 0x0F) + ((year_bcd >> 4) * 10);

	    dcf_latest_time.minute = (min_bcd & 0x0F) + ((min_bcd >> 4) * 10);
	    dcf_latest_time.hour   = (hour_bcd & 0x0F) + ((hour_bcd >> 4) * 10);
	    dcf_latest_time.day    = (day_bcd & 0x0F) + ((day_bcd >> 4) * 10);
	    dcf_latest_time.month  = (month_bcd & 0x0F) + ((month_bcd >> 4) * 10);
	    dcf_latest_time.year   = (year_bcd & 0x0F) + ((year_bcd >> 4) * 10);
	    dcf_latest_time.is_updated = true;
}
