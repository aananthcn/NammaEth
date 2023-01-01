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
// static uint8 Regs_Cmn[5] = {0x00, 0x00, 0x00, 0x80 /* AUTOINC = 0x1*/, 0x00};
static MacPhyStateType ENC28J60_State;


// BASIC ETHERNET Tx & Rx Buffers
#define SPI_ETH_BASIC_CHAN_LEN	(2048)
uint8 SpiEthBasicTx[SPI_ETH_BASIC_CHAN_LEN];
uint8 SpiEthBasicRx[SPI_ETH_BASIC_CHAN_LEN];



//////////////////////////////////////////////
// Local Functions

/* This function switches bank based on bits[0:5] - bank number bits
** from passed argument (i.e., reg). */
static inline boolean enc28j60_switch_bank(uint16 reg) {
	uint8 data;
	uint8 bank;

	// First, return if the target reg is a common register
	if (0x4000 & reg) {
		return TRUE; // bank switch is not required for common register
	}

	/* For the up-comming transmission, we just need to send 1+1 byte */
	Spi_SetupEB(0, SpiEthBasicTx, SpiEthBasicRx, 2);

	/* Read ECON1 register */
	SpiEthBasicTx[0] = (uint8) ((RD_REG_OPCODE) | (ECON1));
	SpiEthBasicTx[1] = 0; // dummy
	if (E_NOT_OK == Spi_SyncTransmit(SEQ_ETHERNET_BASIC_TX_RX)) {
		pr_log("%s: Spi Sync Tx failure[1]!\n", __func__);
		return FALSE;
	}
	data = SpiEthBasicRx[1] & 0xFC; // remove last 2 bits & read


	/* Write ECON1 register with target bank bits */
	SpiEthBasicTx[0] = (uint8) ((WR_REG_OPCODE) | (ECON1));
	bank = ((reg & 0x3F00) >> 8);
	SpiEthBasicTx[1] = data | (bank & 0x03);
	if (E_NOT_OK == Spi_SyncTransmit(SEQ_ETHERNET_BASIC_TX_RX)) {
		pr_log("%s: Spi Sync Tx failure[2]!\n", __func__);
		return FALSE;
	}

	return TRUE;
}



//////////////////////////////////////////////
// Basic ENC28J60 Primitive - Register R/W
uint8 enc28j60_read_reg(uint16 reg) {
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

	SpiEthBasicTx[0] = (uint8) ((RD_REG_OPCODE) | (reg & 0xff));
	SpiEthBasicTx[dlen-1] = 0x00; // dummy 2nd byte for MII or MAC registers
	if (E_NOT_OK == Spi_SyncTransmit(SEQ_ETHERNET_BASIC_TX_RX)) {
		pr_log("%s: Spi Sync Tx failure!\n", __func__);
		return 0xFF;
	}

	/* Though ENC28J60 supports MOTOROLA SPI format, but in RPi Pico / ARM 
	implementation, data received in response to the dummy 2nd byte  */
	return SpiEthBasicRx[dlen-1];
}



boolean enc28j60_write_reg(uint16 reg, uint8 data) {
	uint8 dlen = 2;

	// switch bank based on register
	enc28j60_switch_bank(reg);

	/* For the up-comming transmission, we just need to send/recv 1+1 byte */
	Spi_SetupEB(0, SpiEthBasicTx, SpiEthBasicRx, dlen);

	SpiEthBasicTx[0] = (uint8) ((WR_REG_OPCODE) | (reg & 0xff));
	SpiEthBasicTx[dlen-1] = data;
	if (E_NOT_OK == Spi_SyncTransmit(SEQ_ETHERNET_BASIC_TX_RX)) {
		pr_log("%s: Spi Sync Tx failure!\n", __func__);
		return FALSE;
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
// Basic ENC28J60 Primitive - Bit Set/Clear
static inline boolean enc28j60_bit_ops_reg(uint8 opcode, uint16 reg, uint8 data) {
	uint8 dlen = 2;

	/* MAC, MII register check */
	if (reg & 0x8000) {
		// bit operations can be done only for Ethernet control registers
		return FALSE;
	}

	// switch bank based on register
	enc28j60_switch_bank(reg);

	/* For the up-comming transmission, we just need to send/recv 1+1 byte */
	Spi_SetupEB(0, SpiEthBasicTx, SpiEthBasicRx, dlen);

	SpiEthBasicTx[0] = (uint8) ((opcode) | (reg & 0xff));
	SpiEthBasicTx[dlen-1] = data;
	if (E_NOT_OK == Spi_SyncTransmit(SEQ_ETHERNET_BASIC_TX_RX)) {
		pr_log("%s: Spi Sync Tx failure!\n", __func__);
		return FALSE;
	}

	return TRUE;
}


boolean enc28j60_bitset_reg(uint16 reg, uint8 data) {
	return enc28j60_bit_ops_reg(BT_SET_OPCODE, reg, data);
}


boolean enc28j60_bitclr_reg(uint16 reg, uint8 data) {
	return enc28j60_bit_ops_reg(BT_CLR_OPCODE, reg, data);
}



//////////////////////////////////////////////
// Basic ENC28J60 Primitive - PHY R/W
boolean enc28j60_check_phy_busy(void) {
	boolean phy_busy = FALSE; // lets assume that PHY is idle
	uint8 mstat;
	uint8 cnt = 10; // let us not loop for ever

	while(1) {
		mstat = enc28j60_read_reg(MISTAT);
		if (!(mstat & MISTAT_BUSY)) {
			break;
		}
		cnt--;
		if (cnt == 0) {
			phy_busy = TRUE;
			break;
		}
	}

	return phy_busy;
}



uint16 enc28j60_read_phy(uint8 phyaddr) {
	uint16 phyreg = 0xffff;
	boolean phybusy;
	uint8 hig, low;

	// return if PHY is not free even after few retries
	if (enc28j60_check_phy_busy()) {
		pr_log("%s(): wait period exceeded [1]\n");
		return phyreg;
	}

	// write the address of the PHY register to read
	enc28j60_write_reg(MIREGADR, phyaddr);

	// set the MICMD.MIIRD bit
	enc28j60_write_reg(MICMD, MICMD_MIIRD);

	// wait 10.24us and poll MSTAT.BUSY bit
	phybusy = enc28j60_check_phy_busy();

	// clear the MICMD.MIIRD bit
	enc28j60_write_reg(MICMD, 0x00);
	if (phybusy) {
		pr_log("%s(): wait period exceeded [2]\n");
		return phyreg;
	}

	// read desired data from MIRDL and MIRDH (order is important)
	low = enc28j60_read_reg(MIRDL);
	hig = enc28j60_read_reg(MIRDH);
	phyreg = (hig << 8 | low);

	return phyreg;
}



boolean enc28j60_write_phy(uint8 phyaddr, uint16 data) {
	// write the address of the PHY register to read
	enc28j60_write_reg(MIREGADR, phyaddr);

	// write data low and high bytes
	enc28j60_write_reg(MIWRL, (uint8)(data & 0xff));
	enc28j60_write_reg(MIWRH, (uint8)(data >> 8));

	// wait 10.24us and poll MSTAT.BUSY bit
	if (enc28j60_check_phy_busy()) {
		pr_log("%s(): wait period exceeded\n");
		return FALSE;
	}

	return TRUE;
}



//////////////////////////////////////////////
// Global Functions
boolean macphy_init(const uint8 *mac_addr) {
	if (mac_addr == NULL) {
		pr_log("%s: invalid MAC address!\n", __func__);
		return FALSE;
	}

	/* reset the chip first, set bank to 0 */
	enc28j60_sys_cmd(SC_RST_OPCODE);
	enc28j60_bitclr_reg(ECON1, 0x03);

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