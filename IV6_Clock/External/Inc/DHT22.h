/*
 * DHT22.h
 *
 *  Created on: 2026. jún. 12.
 *      Author: Juhász Gábor
 */

#ifndef INC_DHT22_H_
#define INC_DHT22_H_

#include "main.h"

typedef struct{
	GPIO_TypeDef *DataPort;
	uint16_t DataPin;
	int16_t Temperature;
	uint16_t Humidity;
}DHT22_Data;

void DHT22_Init();
uint8_t DHT22_Read(DHT22_Data *sensor);

#endif /* INC_DHT22_H_ */
