/*
 * VFDDisplay.c
 *
 *  Created on: 2026. jún. 21.
 *      Author: jgabo
 */

#include "VFDDisplay.h"

static const uint8_t VFDSecDigits[10] = {0x3F, 0x06, 0x9B, 0x8F, 0xA6, 0xAD, 0xBD, 0x07, 0xBF, 0xAF}; // Panel tervezési hiba miatt a másodperC egyesek kijelzésénél 3-as lába nem jó helyen van!
static const uint8_t VFDDigits[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

static void VFD_ShowText(uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5, uint8_t c6){

	MCP23S17_WritePort(&RMI01_Hours, ADDR_GPIOA, c2);
	MCP23S17_WritePort(&RMI01_Hours, ADDR_GPIOB, c1);

	MCP23S17_WritePort(&RMI02_Minutes, ADDR_GPIOA, c4);
	MCP23S17_WritePort(&RMI02_Minutes, ADDR_GPIOB, c3);

	MCP23S17_WritePort(&RMI03_Seconds, ADDR_GPIOA, c6);
	MCP23S17_WritePort(&RMI03_Seconds, ADDR_GPIOB, c5);
}

void VFD_Show(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t bl_hours, uint8_t bl_minutes, uint8_t bl_seconds){

	uint8_t h_tens, h_units;
	uint8_t m_tens, m_units;
	uint8_t s_tens, s_units;

	if(bl_hours == 1){
		h_tens = 0x00;
		h_units = 0x00;
	}
	else{
		h_tens = VFDDigits[hours / 10];
		h_units = VFDDigits[hours % 10];

	}
	if(bl_minutes == 1){
		m_tens = 0x00;
		m_units = 0x00;
	}
	else{
		m_tens = VFDDigits[minutes / 10];
		m_units = VFDDigits[minutes % 10];

	}

	if(bl_seconds== 1){
		s_tens = 0x00;
		s_units = 0x00;
	}
	else{
		s_tens = VFDDigits[seconds / 10];
		s_units = VFDSecDigits[seconds % 10];

	}

	VFD_ShowText(h_tens, h_units, m_tens, m_units, s_tens, s_units);

}

void VFD_ShowTemp(uint16_t temp){

	uint8_t t_sign = (temp < 0) ? 0x40 : 0x00;
	uint16_t abs_t = (temp < 0) ? -temp : temp;

	uint8_t t_tens_val = (abs_t / 100) % 10;
	uint8_t t_tens = (t_tens_val == 0) ? 0x00 : VFDDigits[t_tens_val];
	uint8_t t_units = VFDDigits[(abs_t / 10) % 10];
	uint8_t t_tenths = VFDDigits[abs_t %10];

	VFD_ShowText(0x78, t_sign, t_tens, t_units, t_tenths, 0x39);
}
void VFD_ShowHum(uint16_t hum){

	uint8_t h_tens_val = (hum / 100) % 10;
	uint8_t h_tens = (h_tens_val == 0) ? 0x00 : VFDDigits[h_tens_val];
	uint8_t h_units = VFDDigits[(hum / 10) % 10];
	uint8_t h_tenths = VFDDigits[hum % 10];

	VFD_ShowText(0x76, 0x00, h_tens, h_units, h_tenths, 0xB3);

}
void VFD_ShowMainMenu(uint8_t main_id){

	if(main_id == 0) VFD_ShowText(0x39, 0x38, 0x5c, 0x39, 0x70, 0x00);
	else if(main_id == 1) VFD_ShowText(0x5E, 0x77, 0x78, 0x79, 0x00, 0x00);
	else if(main_id == 2) VFD_ShowText(0x77, 0x38, 0x77, 0x50, 0x00, 0x00);

}
void VFD_ShowAlarmStatus(uint8_t alarm_on, uint8_t bl_state){

	if(bl_state == 0){
		VFD_ShowText(0x77, 0x38, 0x00, 0x00, 0x00, 0x00);
	}
	else{
		if(alarm_on) VFD_ShowText(0x77, 0x38, 0x00, 0x00, 0x3F, 0x94);
		else VFD_ShowText(0x77, 0x38, 0x00, 0x00, 0x3F, 0xB1);
	}

}





