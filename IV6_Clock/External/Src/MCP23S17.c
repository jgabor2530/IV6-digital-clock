/*
 * MCP23S17.c
 *
 *  Created on: 2026. márc. 29.
 *      Author: Juhász Gábor
 */

#include "MCP23S17.h"

static void MCP23S17_SPI_Write(MCP23S17SPI *device, uint8_t reg, uint8_t val);
static uint8_t MCP23S17_SPI_Read(MCP23S17SPI *device, uint8_t reg);

static void MCP23S17_SPI_Write(MCP23S17SPI *device, uint8_t reg, uint8_t val){

	uint8_t TX_Buff[3];

	TX_Buff[0] = 0x40 | (device->Address << 1);
	TX_Buff[1] = reg;
	TX_Buff[2] = val;

	if(HAL_SPI_GetState(device->hspi) == HAL_SPI_STATE_READY){
		HAL_GPIO_WritePin(device->CS_Port, device->CSPin, GPIO_PIN_RESET);
		HAL_SPI_Transmit(device->hspi, TX_Buff, 3, 100);
		HAL_GPIO_WritePin(device->CS_Port, device->CSPin, GPIO_PIN_SET);
	}
}

static uint8_t MCP23S17_SPI_Read(MCP23S17SPI *device, uint8_t reg){

	uint8_t RX_Buff = 0;
	uint8_t TX_Buff[2];

	TX_Buff[0] = 0x41 | (device->Address << 1);
	TX_Buff[1] = reg;

	if(HAL_SPI_GetState(device->hspi) == HAL_SPI_STATE_READY){
		HAL_GPIO_WritePin(device->CS_Port, device->CSPin, GPIO_PIN_RESET);
		HAL_SPI_Transmit(device->hspi, TX_Buff, 2, 100);
		HAL_SPI_Receive(device->hspi, &RX_Buff, 1, 100);
		HAL_GPIO_WritePin(device->CS_Port, device->CSPin, GPIO_PIN_SET);
	}
	return RX_Buff;
}

void MCP23S17_Init(MCP23S17SPI *device){

	device->DirA = 0xFF;
	device->DirB = 0xFF;
	device->OutLatchA = 0x00;
	device->OutLatchB = 0x00;
	device->PullUpA = 0x00;
	device->PullUpA = 0x00;
	device->IntEnA = 0X00;
	device->IntEnB = 0X00;

	if(device->EnableAddressing == 1){

		uint8_t RealAddress = device->Address;
		device->Address = 0x00;

		MCP23S17_SPI_Write(device, ADDR_IOCONA, VAL_IOCON);

		device->Address = RealAddress;
	}
	MCP23S17_SPI_Write(device, ADDR_INTCONB, 0x00);

	//Setting PORTA direction register
	MCP23S17_SPI_Write(device, ADDR_IODIRA, device->DirA);
	MCP23S17_SPI_Write(device, ADDR_IODIRB, device->DirA);

	//Setting Output latch register 0 state
	MCP23S17_SPI_Write(device, ADDR_OLATA, device->OutLatchA);
	MCP23S17_SPI_Write(device, ADDR_OLATB, device->OutLatchB);

	MCP23S17_SPI_Write(device, ADDR_GPPUA, device->PullUpA);
	MCP23S17_SPI_Write(device, ADDR_GPPUB, device->PullUpB);

	MCP23S17_SPI_Write(device, ADDR_GPINTENA, device->IntEnA);
	MCP23S17_SPI_Write(device, ADDR_GPINTENA, device->IntEnB);

}

void MCP23S17_PortMode(MCP23S17SPI *device, uint8_t PortDirReg, uint8_t PortValue){

	if(PortDirReg == ADDR_IODIRA) device->DirA = PortValue;
	else if(PortDirReg == ADDR_IODIRB) device->DirB = PortValue;
	MCP23S17_SPI_Write(device, PortDirReg, PortValue);
}

void MCP23S17_PinMode(MCP23S17SPI *device, uint8_t PortDirReg, uint8_t PinMask, MCP23S17_IO mode){

	uint8_t current_dir = 0;

	if(PortDirReg == ADDR_IODIRA) current_dir = device->DirA;
	else if(PortDirReg == ADDR_IODIRB) current_dir = device->DirB;
	else return;

	if(mode == INPUT) current_dir |= PinMask;
	else current_dir &= ~PinMask;

	if(PortDirReg == ADDR_IODIRA) device->DirA = current_dir;
	else device->DirB = current_dir;

	MCP23S17_SPI_Write(device, PortDirReg, current_dir);
}

void MCP23S17_SetPullUp(MCP23S17SPI *device, uint8_t PortPuReg, uint8_t PinMask, MCP23S17_PullUp State){

	uint8_t current_pu = 0;

	if (PortPuReg == ADDR_GPPUA) current_pu = device->PullUpA;
	else if (PortPuReg == ADDR_GPPUB) current_pu = device->PullUpB;
	else return;

	if (State == PULLUP_ENABLE) current_pu |= PinMask;
	else current_pu &= ~PinMask;

	if (PortPuReg == ADDR_GPPUA) device->PullUpA = current_pu;
	else device->PullUpB = current_pu;

	MCP23S17_SPI_Write(device, PortPuReg, current_pu);
}

void MCP23S17_ConfigInterrupt(MCP23S17SPI*device, uint8_t PortReg, uint8_t PinMask, MCP23S17_INT_Mode Mode){

	uint8_t *inten, *intcon, *defval;
	uint8_t reg_inten, reg_intcon, reg_defval;

	if (PortReg == ADDR_GPINTENA || PortReg == ADDR_GPIOA) {
	        inten = &device->IntEnA;   intcon = &device->IntConA;   defval = &device->DefValA;
	        reg_inten = ADDR_GPINTENA; reg_intcon = ADDR_INTCONA;   reg_defval = ADDR_DEFVALA;
	    } else {
	        inten = &device->IntEnB;   intcon = &device->IntConB;   defval = &device->DefValB;
	        reg_inten = ADDR_GPINTENB; reg_intcon = ADDR_INTCONB;   reg_defval = ADDR_DEFVALB;
	    }

	    switch (Mode) {
	        case MCP_INT_DISABLED:
	            *inten &= ~PinMask;
	            break;

	        case MCP_INT_CHANGE:
	            *inten |= PinMask;
	            *intcon &= ~PinMask;
	            break;

	        case MCP_INT_FALLING:
	            *inten |= PinMask;
	            *intcon |= PinMask;
	            *defval |= PinMask;
	            break;

	        case MCP_INT_RISING:
	            *inten |= PinMask;   // Bekapcsoljuk a megszakítást
	            *intcon |= PinMask;  // INTCON = 1 (DEFVAL-hoz hasonlít)
	            *defval &= ~PinMask; // DEFVAL = 0 (A nyugalmi állapot 0, így 1-re riaszt)
	            break;
	    }

	    MCP23S17_SPI_Write(device, reg_inten, *inten);
	    MCP23S17_SPI_Write(device, reg_intcon, *intcon);
	    MCP23S17_SPI_Write(device, reg_defval, *defval);
}

void MCP23S17_WritePort(MCP23S17SPI *device, uint8_t PortReg , uint8_t PortValue){

	if(PortReg == ADDR_OLATA || PortReg == ADDR_GPIOA) device->OutLatchA = PortValue;
	else if(PortReg == ADDR_OLATB || PortReg == ADDR_GPIOB) device->OutLatchA = PortValue;
	else return;
	MCP23S17_SPI_Write(device, PortReg, PortValue);
}

void MCP23S17_WritePin(MCP23S17SPI *device, uint8_t PortReg, uint8_t PinMask, GPIO_PinState State) {

	    uint8_t current_latch = 0;

	    if (PortReg == ADDR_OLATA || PortReg == ADDR_GPIOA) current_latch = device->OutLatchA;
	    else if (PortReg == ADDR_OLATB || PortReg == ADDR_GPIOB) current_latch = device->OutLatchB;
	    else return;

	    if (State == GPIO_PIN_SET) current_latch |= PinMask;
	    else current_latch &= ~PinMask;

	    if (PortReg == ADDR_OLATA || PortReg == ADDR_GPIOA) device->OutLatchA = current_latch;
	    else device->OutLatchB = current_latch;

	    MCP23S17_SPI_Write(device, PortReg, current_latch);

}

uint8_t MCP23S17_ReadPort(MCP23S17SPI *device, uint8_t PortReg){

	uint8_t rxdata = MCP23S17_SPI_Read(device, PortReg);
	return rxdata;
}

GPIO_PinState MCP23S17_ReadPin(MCP23S17SPI *device, uint8_t PortReg, uint8_t PinMask){

	uint8_t rxdata = MCP23S17_SPI_Read(device, PortReg);
	if ((rxdata & PinMask) != 0) return GPIO_PIN_SET;
	else return GPIO_PIN_RESET;
}

uint8_t MCP23S17_GetInterruptFlag(MCP23S17SPI *device, uint8_t PortIntfReg) {

    return MCP23S17_SPI_Read(device, PortIntfReg);
}

uint8_t MCP23S17_ClearInterrupt(MCP23S17SPI *device, uint8_t PortIntcapReg) {

    return MCP23S17_SPI_Read(device, PortIntcapReg);
}

