/*
 * Created on Thu Dec 22 2022 6:47:39 AM
 *
 * The MIT License (MIT)
 * Copyright (c) 2022 Aananth C N
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
 * TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <Spi.h>
#include <os_api.h>

#include "enc28j60.h"


// Main Control & Status Registers
static uint8 Reg_EIE 	= 0x00;
static uint8 Reg_EIR 	= 0x00;
static uint8 Reg_ESTAT 	= 0x00;
static uint8 Reg_ECON2 	= 0x80; // AUTOINC = 0x1
static uint8 Reg_ECON1 	= 0x00;


// BASIC ETHERNET Tx & Rx Buffers
#define SPI_ETH_BASIC_CHAN_LEN	(2048)
uint8 SpiEthBasicTx[SPI_ETH_BASIC_CHAN_LEN];
uint8 SpiEthBasicRx[SPI_ETH_BASIC_CHAN_LEN];



// local declaration
uint8 enc28j60_read_reg(uint16 reg);
boolean enc28j60_switch_bank(uint8 bank);


//////////////////////////////////////////////
// Function Definitions
void macphy_init(void) {
	uint8 data;
	static uint8 bank;

	data = enc28j60_read_reg(ERDPTL);
	pr_log("ERDPTL: 0x%02x\n", data);

	data = enc28j60_read_reg(ERDPTH);
	pr_log("ERDPTH: 0x%02x\n", data);

	enc28j60_switch_bank(++bank);
	if (bank > 3) {
		bank = 0;
	}
	data = enc28j60_read_reg(ECON1);
	pr_log("ECON1: 0x%02x\n", data);
}



boolean enc28j60_switch_bank(uint8 bank) {
	/* For the up-comming transmission, we just need to send 1+1 byte */
	Spi_SetupEB(0, SpiEthBasicTx, SpiEthBasicRx, 2);

	SpiEthBasicTx[0] = (uint8) ((WR_OPCODE) | (ECON1));
	SpiEthBasicTx[1] = (uint8) ((Reg_ECON1 & 0xFC) | (bank & 0x03));
	if (E_NOT_OK == Spi_SyncTransmit(SEQ_ETHERNET_BASIC_TX_RX)) {
		pr_log("%s: Spi Sync Tx failure!\n", __func__);
		return FALSE;
	}

	return TRUE;
}



uint8 enc28j60_read_reg(uint16 reg) {
	/* For the up-comming transmission, we just need to send/recv 1+1 byte */
	Spi_SetupEB(0, SpiEthBasicTx, SpiEthBasicRx, 2);

	SpiEthBasicTx[0] = (uint8) ((RD_OPCODE) | (reg & 0xff));
	SpiEthBasicTx[1] = 0x00; // dummy 2nd byte
	if (E_NOT_OK == Spi_SyncTransmit(SEQ_ETHERNET_BASIC_TX_RX)) {
		pr_log("%s: Spi Sync Tx failure!\n", __func__);
		return 0xFF;
	}

	/* Though ENC28J60 supports MOTOROLA SPI format, but in RPi Pico / ARM 
	implementation, data received in response to the dummy 2nd byte  */
	return SpiEthBasicRx[1];
}