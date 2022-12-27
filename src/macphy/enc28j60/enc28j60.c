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
static uint8 Regs_Cmn[5] = {0x00, 0x00, 0x00, 0x80 /* AUTOINC = 0x1*/, 0x00};
static MacPhyStateType ENC28J60_State;


// BASIC ETHERNET Tx & Rx Buffers
#define SPI_ETH_BASIC_CHAN_LEN	(2048)
uint8 SpiEthBasicTx[SPI_ETH_BASIC_CHAN_LEN];
uint8 SpiEthBasicRx[SPI_ETH_BASIC_CHAN_LEN];




//////////////////////////////////////////////
// Local Functions
static inline uint8 enc28j60_get_cmn_reg(uint16 reg) {
	// check for the common register flag in the register argument
	if (!(reg & 0x4000)) {
		pr_log("%s: invalid register!\n", __func__);
		return 0xff;
	}

	// 0x1B - common reg offset; 0x4000 to remove the flag
	return Regs_Cmn[reg - 0x401B];
}


static inline void enc28j60_set_cmn_reg(uint16 reg, uint8 data) {
	// check for the common register flag in the register argument
	if (!(reg & 0x4000)) {
		pr_log("%s: invalid register!\n", __func__);
		return;
	}

	// 0x1B - common reg offset; 0x4000 to remove the flag
	Regs_Cmn[reg - 0x401B] = data;
}


static inline uint8 enc28j60_get_bank_info(uint16 reg) {
	return ((reg & 0x3F00) >> 8);
}



static inline boolean enc28j60_switch_bank(uint16 reg) {
	uint8 data;
	uint8 bank;

	// check if the bank is already switched
	bank = enc28j60_get_bank_info(reg);
	if (bank == (enc28j60_get_cmn_reg(ECON1) & 0x3)) {
		return FALSE;
	}

	/* For the up-comming transmission, we just need to send 1+1 byte */
	Spi_SetupEB(0, SpiEthBasicTx, SpiEthBasicRx, 2);

	SpiEthBasicTx[0] = (uint8) ((WR_REG_OPCODE) | (ECON1));
	data = (uint8) ((enc28j60_get_cmn_reg(ECON1) & 0xFC) | (bank & 0x03));
	SpiEthBasicTx[1] = data;
	if (E_NOT_OK == Spi_SyncTransmit(SEQ_ETHERNET_BASIC_TX_RX)) {
		pr_log("%s: Spi Sync Tx failure!\n", __func__);
		return FALSE;
	}

	// update the copy of common register on successful transmit
	enc28j60_set_cmn_reg(ECON1, data);

	return TRUE;
}



//////////////////////////////////////////////
// Basic ENC28J60 Primitive Functions 
uint8 enc28j60_read_reg(uint16 reg) {
	// switch bank based on register
	enc28j60_switch_bank(reg);

	/* For the up-comming transmission, we just need to send/recv 1+1 byte */
	Spi_SetupEB(0, SpiEthBasicTx, SpiEthBasicRx, 2);

	SpiEthBasicTx[0] = (uint8) ((RD_REG_OPCODE) | (reg & 0xff));
	SpiEthBasicTx[1] = 0x00; // dummy 2nd byte
	if (E_NOT_OK == Spi_SyncTransmit(SEQ_ETHERNET_BASIC_TX_RX)) {
		pr_log("%s: Spi Sync Tx failure!\n", __func__);
		return 0xFF;
	}

	// if the read operation is on common registers, take a copy
	if (reg & 0x4000) {
		enc28j60_set_cmn_reg(reg, SpiEthBasicRx[1]);
	}

	/* Though ENC28J60 supports MOTOROLA SPI format, but in RPi Pico / ARM 
	implementation, data received in response to the dummy 2nd byte  */
	return SpiEthBasicRx[1];
}



boolean enc28j60_write_reg(uint16 reg, uint8 data) {
	// switch bank based on register
	enc28j60_switch_bank(reg);

	/* For the up-comming transmission, we just need to send/recv 1+1 byte */
	Spi_SetupEB(0, SpiEthBasicTx, SpiEthBasicRx, 2);

	SpiEthBasicTx[0] = (uint8) ((WR_REG_OPCODE) | (reg & 0xff));
	SpiEthBasicTx[1] = data;
	if (E_NOT_OK == Spi_SyncTransmit(SEQ_ETHERNET_BASIC_TX_RX)) {
		pr_log("%s: Spi Sync Tx failure!\n", __func__);
		return FALSE;
	}

	// if the write operation is on common registers, take a copy
	if (reg & 0x4000) {
		enc28j60_set_cmn_reg(reg, data);
	}

	return TRUE;	
}


boolean enc28j60_sys_cmd(uint8 cmd) {
	/* For the up-comming transmission, we just need to send 1 byte */
	Spi_SetupEB(0, SpiEthBasicTx, SpiEthBasicRx, 1);
	SpiEthBasicTx[0] = (uint8) (cmd);
	if (E_NOT_OK == Spi_SyncTransmit(SEQ_ETHERNET_BASIC_TX_RX)) {
		pr_log("%s: Spi Sync Tx failure!\n", __func__);
		return FALSE;
	}

	return TRUE;
}


//////////////////////////////////////////////
// Global Functions
void macphy_init(void) {
	uint8 data;

	/* reset the chip first */
	enc28j60_sys_cmd(SC_RST_OPCODE);


}


void macphy_periodic_fn(void) {

}