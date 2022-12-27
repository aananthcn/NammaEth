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

#include <stddef.h>

#include "enc28j60.h"


// Memory Buffer Layout (8k)
#define BUFFER_BEG	0x0000
#define BUFFER_END	0x1FFF
#define TX_BUF_BEG	BUFFER_BEG
#define TX_BUF_END	0x0FFF
#define RX_BUF_BEG	0x1000
#define RX_BUF_END	BUFFER_END

// Frame configs
#define MAX_FRMLEN	(1522)


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
	uint8 dlen = 2;

	// switch bank based on register
	enc28j60_switch_bank(reg);

	/* MAC, MII register check */
	if (reg & 0x8000) {
		dlen = 3;
		SpiEthBasicTx[dlen-2] = 0x00; // dummy byte
	}

	/* For the up-comming transmission, we just need to send/recv 1+1 byte */
	Spi_SetupEB(0, SpiEthBasicTx, SpiEthBasicRx, dlen);

	SpiEthBasicTx[0] = (uint8) ((WR_REG_OPCODE) | (reg & 0xff));
	SpiEthBasicTx[dlen-1] = data;
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
boolean macphy_init(uint8 *mac_addr) {
	if (mac_addr == NULL) {
		pr_log("%s: invalid MAC address!\n", __func__);
		return FALSE;
	}

	/* reset the chip first */
	enc28j60_sys_cmd(SC_RST_OPCODE);

	/* set buffer memory layout - Rx */
	enc28j60_write_reg(ERXSTL, (uint8)(RX_BUF_BEG & 0xFF));
	enc28j60_write_reg(ERXSTH, (uint8)(RX_BUF_BEG >> 8));
	enc28j60_write_reg(ERXNDL, (uint8)(RX_BUF_END & 0xFF));
	enc28j60_write_reg(ERXNDH, (uint8)(RX_BUF_END >> 8));

	/* set buffer memory layout - Tx */
	enc28j60_write_reg(ETXSTL, (uint8)(TX_BUF_BEG & 0xFF));
	enc28j60_write_reg(ETXSTH, (uint8)(TX_BUF_BEG >> 8));
	enc28j60_write_reg(ETXNDL, (uint8)(TX_BUF_END & 0xFF));
	enc28j60_write_reg(ETXNDH, (uint8)(TX_BUF_END >> 8));

	/* set packet filter for reception */
	enc28j60_write_reg(ERXFCON, ERXFCON_UCEN|ERXFCON_CRCEN|ERXFCON_BCEN);

	/* MAC configurations */
	enc28j60_write_reg(MACON1, MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS);
	enc28j60_write_reg(MACON2, 0x00);
	enc28j60_write_reg(MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN);

	/* interframe gap configurations */
	enc28j60_write_reg(MAIPGL, 0x12);
	enc28j60_write_reg(MAIPGH, 0x0C);
	enc28j60_write_reg(MABBIPG, 0x12);

	/* max frame length configfigurations */
	enc28j60_write_reg(MAMXFLL, (uint8)(MAX_FRMLEN & 0xFF));
	enc28j60_write_reg(MAMXFLH, (uint8)(MAX_FRMLEN >> 8));

	/* write MAC address - bit 48 on byte 0, hence reversed */
	enc28j60_write_reg(MAADR5, mac_addr[0]);
	enc28j60_write_reg(MAADR4, mac_addr[1]);
	enc28j60_write_reg(MAADR3, mac_addr[2]);
	enc28j60_write_reg(MAADR2, mac_addr[3]);
	enc28j60_write_reg(MAADR1, mac_addr[4]);
	enc28j60_write_reg(MAADR0, mac_addr[5]);

	return TRUE;
}


void macphy_periodic_fn(void) {

}