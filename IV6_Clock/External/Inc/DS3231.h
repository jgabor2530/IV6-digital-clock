/*
 * DS3231.h
 *
 *  Created on: 2026. máj. 17.
 *      Author: Juhász Gábor
 */

#ifndef INC_DS3231_H_
#define INC_DS3231_H_

#include "main.h"
#include "stdbool.h"
typedef struct {

	I2C_HandleTypeDef *hi2c;
	bool CR_EOSC;
	bool CR_BBSQW;
	bool CR_CONV;
	bool CR_RS1;
	bool CR_RS2;
	bool CR_INTCN;
	bool CR_A2IE;
	bool CR_A1IE;
	bool HMode;

}DS3231_RTC;

typedef enum {
	ALARM1_EVERY_SECOND,
	ALARM1_MATCH_SECOND,
	ALARM1_MATCH_MIN_SEC,
	ALARM1_MATCH_HR_MIN_SEC,
	ALARM1_MATCH_DATE_HR_MIN_SEC,
	ALARM1_MATCH_DAY_HR_MIN_SEC
}DS3231Alarm1Mode;

typedef enum {
	ALARM2_EVERY_MINUTE,
	ALARM2_MATCH_MIN,
	ALARM2_MATCH_HR_MIN,
	ALARM2_MATCH_DATE_HR_MIN,
	ALARM2_MATCH_DAY_HR_MIN
}DS3231Alarm2Mode;

//REGISTER ADDRESSES
#define ADDR_READ 0xD1
#define ADDR_WRITE 0xD0
#define ADDR_CONTREG 0X0E
#define ADDR_SEC 0x00
#define ADDR_DAY 0x03
#define ADDR_ALM1_SEC 0x07
#define ADDR_ALM2_MIN 0x0B
#define ADDR_STATUSREG 0X0F
#define ADDR_TEMP 0X11

#define ALARM1 1
#define ALARM2 2

void DS3231_Init(DS3231_RTC *device);
void DS3231_SetTime(DS3231_RTC *device,uint8_t Data[],bool hrFormat, bool fAMPM);
void DS3231_GetTime(DS3231_RTC *device, uint8_t *Data, bool *hrFormat,bool *fAMPM);
void DS3231_SetDate(DS3231_RTC *device, uint8_t Data[]);
void DS3231_GetDate(DS3231_RTC *device, uint8_t Data[]);
void DS3231_SetAlarm1(DS3231_RTC *device,uint8_t AlarmTime[], DS3231Alarm1Mode mode, bool hrFormat, bool fAMPM);
void DS3231_SetAlarm2(DS3231_RTC *device,uint8_t AlarmTime[], DS3231Alarm2Mode mode, bool hrFormat, bool fAMPM);
bool DS3231_CheckAlarm(DS3231_RTC *device, uint8_t alarmNum);
void DS3231_ClearAlarm(DS3231_RTC *device, uint8_t alarmNum);
uint16_t DS3231_GetTemp(DS3231_RTC *device);
#endif /* INC_DS3231_H_ */
