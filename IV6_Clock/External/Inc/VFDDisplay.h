/*
 * VFDDisplay.h
 *
 *  Created on: 2026. jún. 21.
 *      Author: jgabo
 */

#ifndef INC_VFDDISPLAY_H_
#define INC_VFDDISPLAY_H_

#include "main.h"
#include "MCP23S17.h"

extern MCP23S17SPI RMI01_Hours;
extern MCP23S17SPI RMI02_Minutes;
extern MCP23S17SPI RMI03_Seconds;

void VFD_Show(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t bl_hours, uint8_t bl_minutes, uint8_t bl_seconds);
void VFD_ShowTemp(uint16_t temp);
void VFD_ShowHum(uint16_t hum);
void VFD_ShowMainMenu(uint8_t main_id);
void VFD_ShowAlarmStatus(uint8_t alarm_on, uint8_t bl_state);

#endif /* INC_VFDDISPLAY_H_ */
