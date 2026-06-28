/*
 * DS3231.c
 *
 *  Created on: 2026. máj. 17.
 *      Author: Juhász Gábor
 */

#include "DS3231.h"

//PRIVATE FUNCTIONS DECLARATE
static void DS3231_WriteData(DS3231_RTC *device, uint8_t RegAddr, uint8_t Data[], uint8_t size);
static void DS3231_ReadData(DS3231_RTC *device, uint8_t RegAddr, uint8_t Data[], uint8_t size);
static uint8_t DS3231_DECtoBCD(uint8_t DECNUM);
static uint8_t DS3231_BCDtoDEC(uint8_t BCDNUM);

//PRIVATE FUNCTIONS
static void DS3231_WriteData(DS3231_RTC *device, uint8_t RegAddr, uint8_t Data[], uint8_t size) {

	if(HAL_I2C_GetState(device->hi2c) == HAL_I2C_STATE_READY){
		HAL_I2C_Mem_Write(device->hi2c, ADDR_WRITE, RegAddr, I2C_MEMADD_SIZE_8BIT, Data, size, 1000);
	}
}

static void DS3231_ReadData(DS3231_RTC *device, uint8_t RegAddr, uint8_t Data[], uint8_t size) {

	if(HAL_I2C_GetState(device->hi2c) == HAL_I2C_STATE_READY){
		HAL_I2C_Mem_Read(device->hi2c, ADDR_READ, RegAddr, I2C_MEMADD_SIZE_8BIT, Data, size, 1000);
	}
}

static uint8_t DS3231_DECtoBCD(uint8_t DECNUM) {

	return (uint8_t) ((DECNUM / 10 * 16) + (DECNUM % 10));

}

static uint8_t DS3231_BCDtoDEC(uint8_t BCDNUM) {

	return (int) ((BCDNUM / 16 * 10) + (BCDNUM % 16));
}

//PUBLIC FUNCTIONS
void DS3231_Init(DS3231_RTC *device) {

	uint8_t ControlReg = 0;

	ControlReg |= ((uint8_t) device->CR_EOSC << 7);
	ControlReg |= ((uint8_t) device->CR_BBSQW << 6);
	ControlReg |= ((uint8_t) device->CR_CONV << 5);
	ControlReg |= ((uint8_t) device->CR_RS2 << 4);
	ControlReg |= ((uint8_t) device->CR_RS1 << 3);
	ControlReg |= ((uint8_t) device->CR_INTCN << 2);
	ControlReg |= ((uint8_t) device->CR_A2IE << 1);
	ControlReg |= ((uint8_t) device->CR_A1IE);

	DS3231_WriteData(device, ADDR_CONTREG, &ControlReg, 1);

}

void DS3231_SetTime(DS3231_RTC *device, uint8_t Data[], bool hrFormat, bool fAMPM) {

	uint8_t BCDTIME[3];

	BCDTIME[0] = DS3231_DECtoBCD(Data[0]);
	BCDTIME[1] = DS3231_DECtoBCD(Data[1]);

	if (hrFormat == true) {
		BCDTIME[2] = ((uint8_t) hrFormat << 6) | ((uint8_t) fAMPM << 5) | DS3231_DECtoBCD(Data[2]);
	} else {
		BCDTIME[2] = ((uint8_t) hrFormat << 6) | DS3231_DECtoBCD(Data[2]);
	}
	DS3231_WriteData(device, ADDR_SEC, BCDTIME, 3);

}

void DS3231_GetTime(DS3231_RTC *device, uint8_t *Data, bool *hrFormat, bool *fAMPM) {

	uint8_t BCDData[3];
	uint8_t Mask;

	DS3231_ReadData(device, ADDR_SEC, BCDData, 3);

	Data[0] = DS3231_BCDtoDEC(BCDData[0]);
	Data[1] = DS3231_BCDtoDEC(BCDData[1]);

	if (BCDData[2] & (1 << 6)) {

		*hrFormat = true;
		Mask = 0x1F;

		if (BCDData[2] & (1 << 5)) {
			*fAMPM = true;
		} else {
			*fAMPM = false;
		}
	} else {
		*hrFormat = false;
		Mask = 0x3F;
		*fAMPM = false;
	}

	Data[2] = DS3231_BCDtoDEC(BCDData[2] & Mask);
}

void DS3231_SetDate(DS3231_RTC *device,uint8_t Data[]){

	uint8_t BCDDATE[4];

	BCDDATE[0] = DS3231_DECtoBCD(Data[0]);
	BCDDATE[1] = DS3231_DECtoBCD(Data[1]);
	BCDDATE[2] = DS3231_DECtoBCD(Data[2]);
	BCDDATE[3] = DS3231_DECtoBCD(Data[3]);

	DS3231_WriteData(device, ADDR_DAY, BCDDATE, 4);
}

void DS3231_GetDate(DS3231_RTC *device, uint8_t Data[]){

	uint8_t BCDDATE[4];

	DS3231_ReadData(device, ADDR_DAY, BCDDATE, 4);

	Data[0] = DS3231_BCDtoDEC(BCDDATE[0]);
	Data[1] = DS3231_BCDtoDEC(BCDDATE[1]);
	Data[2] = DS3231_BCDtoDEC(BCDDATE[2] & 0x1F);
	Data[3] = DS3231_BCDtoDEC(BCDDATE[3]);

}

void DS3231_SetAlarm1(DS3231_RTC *device,uint8_t AlarmTime[], DS3231Alarm1Mode mode, bool hrFormat, bool fAMPM){

	uint8_t BCDALARM[4];
	uint8_t A1M1 = 0, A1M2 = 0, A1M3 = 0, A1M4 = 0;
	uint8_t DYDT = 0;

	switch(mode){

	case ALARM1_EVERY_SECOND:
		A1M1 = 1; A1M2 = 1; A1M3 = 1; A1M4 = 1; break;
	case ALARM1_MATCH_SECOND:
		A1M1 = 0; A1M2 = 1; A1M3 = 1; A1M4 = 1; break;
	case ALARM1_MATCH_MIN_SEC:
		A1M1 = 0; A1M2 = 0; A1M3 = 1; A1M4 = 1; break;
	case ALARM1_MATCH_HR_MIN_SEC:
		A1M1 = 0; A1M2 = 0; A1M3 = 0; A1M4 = 1; break;
	case ALARM1_MATCH_DATE_HR_MIN_SEC:
		A1M1 = 0; A1M2 = 0; A1M3 = 0; A1M4 = 0; DYDT = 0; break;
	case ALARM1_MATCH_DAY_HR_MIN_SEC:
		A1M1 = 1; A1M2 = 1; A1M3 = 1; A1M4 = 1; DYDT = 0; break;
	}

	BCDALARM[0] = (A1M1 << 7) | DS3231_DECtoBCD(AlarmTime[0]);
	BCDALARM[1] = (A1M2 << 7) | DS3231_DECtoBCD(AlarmTime[1]);

	if(hrFormat == false){
		BCDALARM[2] = (A1M3 << 7) | ((uint8_t)hrFormat << 6) | DS3231_DECtoBCD(AlarmTime[2]);
	}
	else{
		BCDALARM[2] = (A1M3 << 7) | ((uint8_t)hrFormat << 6) | ((uint8_t)fAMPM << 5) | DS3231_DECtoBCD(AlarmTime[2]);
	}
	BCDALARM[3] = (A1M4 << 7) | (DYDT << 6) | DS3231_DECtoBCD(AlarmTime[3]);

	DS3231_WriteData(device, ADDR_ALM1_SEC, BCDALARM, 4);
}

void DS3231_SetAlarm2(DS3231_RTC *device,uint8_t AlarmTime[], DS3231Alarm2Mode mode, bool hrFormat, bool fAMPM){

	uint8_t BCDALARM[3];
	uint8_t A2M2 = 0, A2M3 = 0, A2M4 = 0;
	uint8_t DYDT = 0;

	switch(mode){
	case ALARM2_EVERY_MINUTE:
		A2M2 = 1; A2M3 = 1; A2M4 = 1; break;
	case ALARM2_MATCH_MIN:
		A2M2 = 0; A2M3 = 0; A2M4 = 1; break;
	case ALARM2_MATCH_HR_MIN:
		A2M2 = 0; A2M3 = 0; A2M4 = 0; break;
	case ALARM2_MATCH_DATE_HR_MIN:
		A2M2 = 	0; A2M3 = 0; A2M4 = 0; DYDT = 0; break;
	case ALARM2_MATCH_DAY_HR_MIN:
		A2M2 = 0; A2M3 = 0; A2M4 = 0; DYDT = 1; break;
	}

	BCDALARM[0] = (A2M2 << 7) | DS3231_DECtoBCD(AlarmTime[0]);

	if(hrFormat == false){
		BCDALARM[1] = (A2M3 << 7) | ((uint8_t)hrFormat << 6) | DS3231_DECtoBCD(AlarmTime[1]);
	}
	else{
		BCDALARM[1] = (A2M3 << 7) | ((uint8_t)hrFormat << 6) | ((uint8_t)fAMPM << 5) | DS3231_DECtoBCD(AlarmTime[1]);
	}
	BCDALARM[2] = (A2M4 << 7) | (DYDT << 6) | DS3231_DECtoBCD(AlarmTime[2]);
	DS3231_WriteData(device, ADDR_ALM2_MIN, BCDALARM, 3);
}

bool DS3231_CheckAlarm(DS3231_RTC *device, uint8_t alarmNum){

	uint8_t StatusReg = 0;

	DS3231_ReadData(device, ADDR_STATUSREG, &StatusReg, 1);

	if(alarmNum == ALARM1){
		if(StatusReg & (1<<0)) return true;
	}
	else if(alarmNum == ALARM2){
		if(StatusReg & (1<<1)) return true;
	}
	return true;
}

void DS3231_ClearAlarm(DS3231_RTC *device, uint8_t alarmNum){

	uint8_t StatusReg = 0;

	DS3231_ReadData(device, ADDR_STATUSREG, &StatusReg, 1);

	if(alarmNum == ALARM1){
		StatusReg &= ~(1<<0);
	}
	else if(alarmNum == ALARM2){
		StatusReg &= ~(1<<1);
	}

	DS3231_WriteData(device, ADDR_STATUSREG, &StatusReg, 1);
}

uint16_t DS3231_GetTemp(DS3231_RTC *device){

	uint8_t TempData[2];
	DS3231_ReadData(device, ADDR_TEMP, TempData, 2);
	int8_t temp = (int8_t)TempData[0];
	uint8_t tempFrac = TempData[1] >> 6;

	return (temp*100) + (tempFrac*25);
}






