/*
 * MCP23S17.h
 *
 *  Created on: 2026. márc. 29.
 *      Author: Juhász Gábor
 */

#ifndef INC_MCP23S17_H_
#define INC_MCP23S17_H_

#include "main.h"

typedef struct{
	SPI_HandleTypeDef *hspi;
	GPIO_TypeDef *CS_Port;
	uint16_t CSPin;
	uint8_t Address;
	uint8_t EnableAddressing;
	uint8_t DirA;
	uint8_t DirB;
	uint8_t PullUpA;
	uint8_t PullUpB;
	uint8_t OutLatchA;
	uint8_t OutLatchB;
	uint8_t IntEnA;
	uint8_t IntEnB;
	uint8_t IntConA;
	uint8_t IntConB;
	uint8_t DefValA;
	uint8_t DefValB;

}MCP23S17SPI;

//Enum definíciók
typedef enum{
	OUTPUT = 0,
	INPUT = 1
}MCP23S17_IO;

typedef enum{
	PULLUP_ENABLE = 0,
	PULLUP_DISABLE = 1
}MCP23S17_PullUp;

typedef enum {
    MCP_INT_DISABLED = 0,
    MCP_INT_CHANGE,
    MCP_INT_FALLING,
    MCP_INT_RISING
} MCP23S17_INT_Mode;

//Register Addresses
#define ADDR_IODIRA 	0x00
#define ADDR_IODIRB 	0x01
#define	ADDR_IPOLA		0x02
#define	ADDR_IPOLB		0x03
#define	ADDR_GPINTENA	0x04
#define	ADDR_GPINTENB	0x05
#define	ADDR_DEFVALA	0x06
#define	ADDR_DEFVALB	0x07
#define	ADDR_INTCONA	0x08
#define	ADDR_INTCONB	0x09
#define	ADDR_IOCONA		0x0A
#define	ADDR_IOCONB		0x0B
#define	ADDR_GPPUA		0x0C
#define	ADDR_GPPUB		0x0D
#define	ADDR_INTFA		0x0E
#define	ADDR_INTFB		0x0F
#define	ADDR_INTCAPA	0x10
#define	ADDR_INTCAPB	0x11
#define ADDR_GPIOA		0x12
#define ADDR_GPIOB		0x13
#define ADDR_OLATA		0x14
#define ADDR_OLATB		0x15

//Values
#define VAL_IOCON 0x28

#define MCP23S17_PIN_0  0x01  // 00000001
#define MCP23S17_PIN_1  0x02  // 00000010
#define MCP23S17_PIN_2  0x04  // 00000100
#define MCP23S17_PIN_3  0x08  // 00001000
#define MCP23S17_PIN_4  0x10  // 00010000
#define MCP23S17_PIN_5  0x20  // 00100000
#define MCP23S17_PIN_6  0x40  // 01000000
#define MCP23S17_PIN_7  0x80  // 10000000
#define MCP23S17_PIN_ALL 0xFF // 11111111

void MCP23S17_Init(MCP23S17SPI *device);

//IO konfigurálás
void MCP23S17_PortMode(MCP23S17SPI *device, uint8_t PortDirReg, uint8_t PortValue);
void MCP23S17_PinMode(MCP23S17SPI *device, uint8_t PortDirReg, uint8_t PinMask, MCP23S17_IO mode);
void MCP23S17_SetPullUp(MCP23S17SPI *device, uint8_t PortPuReg, uint8_t PinMask, MCP23S17_PullUp State);
void MCP23S17_ConfigInterrupt(MCP23S17SPI*device, uint8_t PortReg, uint8_t PinMask, MCP23S17_INT_Mode Mode);

//Kimenet
void MCP23S17_WritePort(MCP23S17SPI *device, uint8_t PortReg, uint8_t PortValue);
void MCP23S17_WritePin(MCP23S17SPI *device, uint8_t PortReg, uint8_t PinMask, GPIO_PinState State);

//Bemenet
uint8_t MCP23S17_ReadPort(MCP23S17SPI *device, uint8_t PortReg);
GPIO_PinState MCP23S17_ReadPin(MCP23S17SPI *device, uint8_t PortReg, uint8_t PinMask);
uint8_t MCP23S17_GetInterruptFlag(MCP23S17SPI *device, uint8_t PortIntfReg);
uint8_t MCP23S17_ClearInterrupt(MCP23S17SPI *device, uint8_t PortIntcapReg);
#endif /* INC_MCP23S17_H_ */
