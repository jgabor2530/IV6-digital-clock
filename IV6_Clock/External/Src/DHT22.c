/*
 * DHT22.c
 *
 *  Created on: 2026. jún. 12.
 *      Author: Juhász Gábor
 */

#include "DHT22.h"
#define DHT_READ_PIN() (sensor->DataPort->IDR & sensor->DataPin)

static uint32_t us_Multiplier = 12;

// BIZTONSÁGOS KÉSLELTETÉS (A volatile megakadályozza, hogy a fordító kitörölje)
static void Delay_us(uint16_t us){
	uint32_t count = us * us_Multiplier;
	while(count--) {
        __NOP();
    }
}

void DHT22_Init(void){
	us_Multiplier = HAL_RCC_GetHCLKFreq() / 4000000;
	if(us_Multiplier == 0) us_Multiplier = 1;
}

uint8_t DHT22_Read(DHT22_Data *sensor){

	uint8_t buffer[5] = {0,0,0,0,0};
	uint16_t timeout = 0;

	// 1. START JEL: Lehúzzuk GND-re 20 ms-ig (DHT11 és DHT22 is érti)
	HAL_GPIO_WritePin(sensor->DataPort, sensor->DataPin, GPIO_PIN_RESET);
	HAL_Delay(20);

	// =======================================================
	// NÉMÍTÁS: Innentől a processzor csak a szenzorra figyel!
	__disable_irq();
	// =======================================================

	// 2. VONAL FELENGEDÉSE: "1"-et írunk rá, így a felhúzó ellenállás felrántja
	HAL_GPIO_WritePin(sensor->DataPort, sensor->DataPin, GPIO_PIN_SET);

	// Várunk egy picikét (20 us), hogy a szenzor felébredjen és reagáljon
	Delay_us(20);

	// 3. VÁLASZ VÁRÁSA (NINCS TÖBBÉ IRÁNYVÁLTÁS, AZONNAL OLVASUNK!)
	timeout = 10000;
	while(DHT_READ_PIN()){
		if(--timeout == 0) { __enable_irq(); return 11; }
	}

	timeout = 10000;
	while(!DHT_READ_PIN()){
		if(--timeout == 0) { __enable_irq(); return 12; }
	}

	timeout = 10000;
	while(DHT_READ_PIN()){
		if(--timeout == 0) { __enable_irq(); return 13; }
	}

	// 4. BITEK BEOLVASÁSA
	// 4. 40 BIT BEOLVASÁSA (ADAPTÍV PULZUSMÉRÉSSEL, KÉSLELTETÉS NÉLKÜL!)
		for (int i = 0; i < 40; i++) {
	        uint32_t time_low = 0;
	        uint32_t time_high = 0;

	        // 1. Mérjük meg az Alacsony állapot hosszát (Ez mindig fix 50 mikroszekundum)
	        while(!DHT_READ_PIN()) {
	            time_low++; // Csak számoljuk a ciklusokat
	            if(time_low > 10000) { __enable_irq(); return 14; }
	        }

	        // 2. Mérjük meg a Magas állapot hosszát (Ez lesz 26 us vagy 70 us)
	        while(DHT_READ_PIN()) {
	            time_high++; // Csak számoljuk a ciklusokat
	            if(time_high > 10000) { __enable_irq(); return 15; }
	        }

	        // 3. A DÖNTÉS: Összehasonlítjuk a két számlálót!
	        // Ha a magas állapot tovább tartott (70 us), mint az alacsony (50 us), az 1-es bit!
	        if (time_high > time_low) {
	            buffer[i/8] |= (1 << (7 - (i%8)));
	        }
	        // (Ha rövidebb volt, akkor 0 marad, mert a buffert eleve 0-val töltöttük fel)
	    }

    // =======================================================
    // VÉGE AZ ADATVIHARNAK, ENGEDÉLYEZZÜK A MEGSZAKÍTÁSOKAT!
    __enable_irq();
    // =======================================================

	// 5. CHECKSUM VIZSGÁLAT
	uint8_t sum = buffer[0] + buffer[1] + buffer[2] + buffer[3];
	if (sum != buffer[4]) return 16;

	// 6. ADATOK SZÉTBONTÁSA STRUKTÚRÁBA
	sensor->Humidity = (buffer[0] << 8) | buffer[1];
	sensor->Temperature = (buffer[2] << 8) | buffer[3];

	if (sensor->Temperature & 0x8000) {
	    sensor->Temperature &= 0x7FFF;
	    sensor->Temperature *= -1;
	}

	return 1;
}
