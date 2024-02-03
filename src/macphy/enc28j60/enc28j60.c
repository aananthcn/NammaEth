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
#include <string.h>

#include "enc28j60.h"
#include <macphy_mpool.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(enc28j60, LOG_LEVEL_DBG);

#define ADDL_ENC28J60_DEBUG_PRINTS      1


// Memory Buffer Layout (8k)
#define BUFFER_BEG	(0x0000)
#define BUFFER_END	(0x1FFF)
#define TX_BUF_BEG	(BUFFER_BEG)
#define TX_BUF_END	(0x0FFF)
#define RX_BUF_BEG	(0x1000)
#define RX_BUF_END	(BUFFER_END)

#define TX_VECT_SZ      (8)
#define RX_VECT_SZ      (4)



// Main Control & Status Registers
static uint32 PHY_Id;
static uint8  PHY_Rev;
static uint8  MAC_RevId;


// State Management Bits
typedef enum {
        MACPHY_RESET = 0x00,
        MACPHY_LINK_UP = 0x01,
        MAX_MACPHY_STATE
}MacPhyState_t;

static MacPhyState_t MacPhy_state;


// Frame configs
#define MAX_ETH_FRAME_LEN	(MEM_POOL_BUF_LEN)
#define ENC28J60_BASIC_MSG_LEN	(32)

uint8 SpiEthBasicTx[ENC28J60_BASIC_MSG_LEN];
uint8 SpiEthBasicRx[ENC28J60_BASIC_MSG_LEN];


// Local function prototypes
boolean enc28j60_write_mem(uint8 *dptr, uint16 dlen, MacSpiMemType *spi_mem);
boolean enc28j60_read_mem(uint8 *dptr, uint16 dlen, MacSpiMemType *spi_mem);



//////////////////////////////////////////////
// Local Functions

/* This function switches bank based on bits[0:5] - bank number bits
** from passed argument (i.e., reg). */
static inline boolean enc28j60_switch_bank(uint16 reg) {
        uint8 data;
        uint8 bank;
        static uint8 bank_old;

        // First, return if the target reg is a common register
        if (0x4000 & reg) {
                return TRUE; // bank switch is not required for common register
        }

        // check if it is required to switch the bank
        bank = ((reg & 0x3F00) >> 8);
        if (bank == bank_old) {
                return TRUE; // already switched
        }

        /* For the up-comming transmission, we just need to send 1+1 byte */
        Spi_SetupEB(0, SpiEthBasicTx, SpiEthBasicRx, 2);

        /* Read ECON1 register */
        SpiEthBasicTx[0] = (uint8) ((RD_REG_OPCODE) | (ECON1));
        SpiEthBasicTx[1] = 0; // dummy
        if (E_NOT_OK == Spi_SyncTransmit(SEQ_ETHERNET_BASIC_TX_RX)) {
                LOG_ERR("%s: Spi Sync Tx failure[1]!", __func__);
                return FALSE;
        }
        data = SpiEthBasicRx[1] & 0xFC; // remove last 2 bits & read


        /* Write ECON1 register with target bank bits */
        SpiEthBasicTx[0] = (uint8) ((WR_REG_OPCODE) | (ECON1));
        SpiEthBasicTx[1] = data | (bank & 0x03);
        if (E_NOT_OK == Spi_SyncTransmit(SEQ_ETHERNET_BASIC_TX_RX)) {
                LOG_ERR("%s: Spi Sync Tx failure[2]!", __func__);
                return FALSE;
        }

        /* Store old bank value to prevent bank switching if it is already switched */
        bank_old = bank;

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
                LOG_ERR("%s: Spi Sync Tx failure!", __func__);
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
                LOG_ERR("%s: Spi Sync Tx failure!", __func__);
                return FALSE;
        }

        return TRUE;	
}


boolean enc28j60_sys_cmd(uint8 cmd) {
        /* For the up-comming transmission, we just need to send 1 byte */
        Spi_SetupEB(0, SpiEthBasicTx, SpiEthBasicRx, 1);
        SpiEthBasicTx[0] = (uint8) (cmd);
        if (E_NOT_OK == Spi_SyncTransmit(SEQ_ETHERNET_BASIC_TX_RX)) {
                LOG_ERR("%s: Spi Sync Tx failure!", __func__);
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
                LOG_ERR("%s: Spi Sync Tx failure!", __func__);
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

        mstat = enc28j60_read_reg(MISTAT);
        if (mstat & MISTAT_BUSY) {
                LOG_WRN("A PHY register is currently being read or written to!");
                phy_busy = TRUE;
        }

        if (mstat & MISTAT_NVALID) {
                LOG_WRN("The contents of MIRD are not valid yet!");
                phy_busy = TRUE;
        }

        if (mstat & MISTAT_SCAN) {
                LOG_WRN("MII management scan operation is in progress!");
                phy_busy = TRUE;
        }

        return phy_busy;
}



uint16 enc28j60_read_phy(uint8 phyaddr) {
        uint8 hig, low;

        // return if PHY is not free even after few retries
        if (enc28j60_check_phy_busy()) {
                LOG_ERR("Eth. PHY is busy - 1");
                return 0xffff;
        }

        // write the address of the PHY register to read
        enc28j60_write_reg(MIREGADR, phyaddr);

        // set the MICMD.MIIRD bit
        enc28j60_write_reg(MICMD, MICMD_MIIRD);

        // wait 10.24us and poll MSTAT.BUSY bit
        k_sleep(K_USEC(11));
        if (enc28j60_check_phy_busy()) {
                LOG_ERR("Eth. PHY is busy - 2");
                return 0xffff;
        }

        // clear the MICMD.MIIRD bit
        enc28j60_write_reg(MICMD, 0x00);

        // read desired data from MIRDL and MIRDH (order is important)
        low = enc28j60_read_reg(MIRDL);
        hig = enc28j60_read_reg(MIRDH);

        return (hig << 8 | low);
}



boolean enc28j60_write_phy(uint8 phyaddr, uint16 data) {
        // write the address of the PHY register to read
        enc28j60_write_reg(MIREGADR, phyaddr);

        // write data low and high bytes
        enc28j60_write_reg(MIWRL, (uint8)(data & 0xff));
        enc28j60_write_reg(MIWRH, (uint8)(data >> 8));

        // wait 10.24us and poll MSTAT.BUSY bit
        k_sleep(K_USEC(11));
        if (enc28j60_check_phy_busy()) {
                LOG_ERR("%s(): wait period exceeded", __func__);
                return FALSE;
        }

        return TRUE;
}



//////////////////////////////////////////////
// Basic ENC28J60 Primitive - Memory R/W
boolean enc28j60_read_mem(uint8 *dptr, uint16 dlen, MacSpiMemType *spi_mem) {
        int i;

        /* check if data+1-byte_read opcode can fit into Rx Buffer */
        if (dlen+1 > MAX_ETH_FRAME_LEN) {
                LOG_ERR("ERROR: %s(): dlen = %d greater than max = %d bytes",
                        __func__, dlen, MAX_ETH_FRAME_LEN);
                        return FALSE;
        }

        /* For the up-comming transaction Tx line will be in high impedence state
        hence we are not clearing the Tx buffer. Also Rx buffer will be filled by
        the Spi, so it is not cleared either */
        Spi_SetupEB(0, spi_mem->tx_buf, spi_mem->rx_buf, dlen+1);

        /* Do the SPI reception */
        spi_mem->tx_buf[0] = (uint8) (RD_MEM_OPCODE);
        if (E_NOT_OK == Spi_SyncTransmit(SEQ_ETHERNET_BASIC_TX_RX)) {
                LOG_ERR("%s: Spi Sync Rx failure!", __func__);
                return FALSE;
        }

        /* copy data bytes to client buffer */
        for (i = 0; i < dlen; i++) {
                dptr[i] = spi_mem->rx_buf[i+1];
        }

        return TRUE;
}



boolean enc28j60_write_mem(uint8 *dptr, uint16 dlen, MacSpiMemType *spi_mem) {
        int i;

        /* check if data+2-byte_read opcode, per-pkt ctrl-byte can fit into Tx Buffer */
        if (dlen+2 > MAX_ETH_FRAME_LEN) {
                LOG_ERR("%s(): dlen = %d greater than max = %d bytes",
                        __func__, dlen, MAX_ETH_FRAME_LEN);
                        return FALSE;
        }

        /* always move the Tx write pointer to start of the Tx memory */
        enc28j60_write_reg(ETXSTL, LO_BYTE(TX_BUF_BEG));
        enc28j60_write_reg(ETXSTH, HI_BYTE(TX_BUF_BEG));
        enc28j60_write_reg(ETXNDL, LO_BYTE(TX_BUF_BEG+dlen+1));
        enc28j60_write_reg(ETXNDH, HI_BYTE(TX_BUF_BEG+dlen+1));
        enc28j60_write_reg(EWRPTL, LO_BYTE(TX_BUF_BEG));
        enc28j60_write_reg(EWRPTH, HI_BYTE(TX_BUF_BEG));

        /* For the up-comming transmission, we just need to send/recv 1+1 byte */
        Spi_SetupEB(0, spi_mem->tx_buf, spi_mem->rx_buf, dlen+2);

        /* Setup Tx Buffer for Transmission */
        spi_mem->tx_buf[0] = (uint8)(WR_MEM_OPCODE);
        spi_mem->tx_buf[1] = (uint8)(0x00); // per-packet control byte, refer section 7.1 of ENC28J60 manual
        for (i = 0; i < dlen; i++) {
                spi_mem->tx_buf[i+2] = dptr[i];
        }

        /* Do the SPI transfer */
        if (E_NOT_OK == Spi_SyncTransmit(SEQ_ETHERNET_BASIC_TX_RX)) {
                LOG_ERR("%s: Spi Sync Tx failure!", __func__);
                return FALSE;
        }

        return TRUE;
}



static MacPhyState_t read_enc28j60_phstat_regs(void) {
	uint16 phy_reg;

	phy_reg = enc28j60_read_phy(PHSTAT1);
	LOG_DBG("\tPHSTAT1: 0x%04x", phy_reg);
        if (phy_reg & 0x04) {
                MacPhy_state |= MACPHY_LINK_UP;
        }
        else {
                MacPhy_state &= ~MACPHY_LINK_UP;
        }

        // TODO: read PHSTAT2 register and update MacPhy_state bits

        return MacPhy_state;
}



void dump_enc28j60_status_registers(void) {
	uint8 reg_data;

	// Status register read
	reg_data = enc28j60_read_reg(ESTAT);
	LOG_DBG("\tESTAT: 0x%02x", reg_data);
	reg_data = enc28j60_read_reg(EIR);
	LOG_DBG("\tEIR: 0x%02x", reg_data);

	// other register tests
	reg_data = enc28j60_read_reg(ERDPTL);
	LOG_DBG("\tERDPTL: 0x%02x", reg_data);
	reg_data = enc28j60_read_reg(ERDPTH);
	LOG_DBG("\tERDPTH: 0x%02x", reg_data);
	reg_data = enc28j60_read_reg(ECOCON);
	LOG_DBG("\tECOCON: 0x%02x", reg_data);

	// phy register status read
	read_enc28j60_phstat_regs();
}


void send_pkt_from_mpool(int pidx, uint8 *pktptr, uint16 pktlen) {
        /* the current pool index will be used for transmission */
        set_active_pool_idx(pidx);

        /* copy the packet to MAC's Tx memory via SPI buffer pool */
        enc28j60_write_mem(pktptr, pktlen, get_pool_mem(pidx));
        /* trigger the MAC to send the copied pkg */
        enc28j60_bitset_reg(ECON1, ECON1_TXRTS);


        /* Errata 12 - Transmit abort may stall transmit logic, revID <= 4 */
        if (MAC_RevId <= 4) {
                enc28j60_bitclr_reg(ECON1, ECON1_TXRTS);
        }

        /* by this time the packet should have gone into the macphy's memory */
        clr_active_pool_idx();

#ifdef ADDL_ENC28J60_DEBUG_PRINTS
        dump_enc28j60_status_registers();
#endif
}



//////////////////////////////////////////////
// Global Functions
boolean macphy_pkt_send(uint8 *pktptr, uint16 pktlen) {
        int pidx;
        uint8 regbits;
        boolean tx_abort;

        if (pktptr == NULL) {
                return FALSE;
        }

        /* get memory pool for ethernet frame transfer */
        pidx = get_m_pool_index();
        if (pidx < 0) {
                LOG_ERR("ERROR: %s() couldn't get a mem pool", __func__);
                return FALSE;
        }

        /* check if any transmit errors from previous attempt */
        regbits = enc28j60_read_reg(ESTAT);
        tx_abort = regbits & ESTAT_TXABRT;
        if (tx_abort) {
                enc28j60_bitclr_reg(ECON1, ECON1_TXRTS);
        }

        /* check if the MACPHY is busy as well as see if link is up */
        regbits = enc28j60_read_reg(ECON1);
        if ((regbits & ECON1_TXRTS) || ((MacPhy_state & MACPHY_LINK_UP) == 0)) {
                /* Not ready - so copy the packet to memory pool buffer */
                memcpy(get_pool_mem(pidx)->tx_buf, pktptr, pktlen);
                get_pool_mem(pidx)->tx_len = pktlen;

                /* In case link is down, please read it now for future use */
                read_enc28j60_phstat_regs();
        }
        else {
                /* NOT BUSY - send data to MACPHY via mem-pool */
                send_pkt_from_mpool(pidx, pktptr, pktlen);
        }

#if ADDL_ENC28J60_DEBUG_PRINTS
        /* check for errors while sending */
        if (enc28j60_read_reg(ESTAT) & ESTAT_TXABRT)
        {
                // a seven-byte transmit status vector will be
                // written to the location pointed to by ETXND + 1,
                LOG_ERR("ERR - transmit aborted");
        }

        if (enc28j60_read_reg(EIR) & EIR_TXERIF)
        {
                LOG_ERR("ERR - transmit interrupt flag set");
        }
#endif

        return TRUE;
}



#define RX_PKT_HDR_SZ (6) /* 2 byte next pkt pointer + rx status vector */
uint16 macphy_pkt_recv(uint8 *pktptr, uint16 maxlen) {
        int pidx;
        uint16 pktlen;
        static uint16 nxtpktptr = RX_BUF_BEG;
        static uint16 rx_status;
        uint8 rx_pkt_hdr[RX_PKT_HDR_SZ];
        uint8 regbits;

        regbits = enc28j60_read_reg(EPKTCNT);
        if (!regbits) {
                return 0;
        }

        /* get memory pool for ethernet frame transfer */
        pidx = get_m_pool_index();
        if (pidx < 0) {
                LOG_ERR("ERROR: %s() couldn't get a mem pool", __func__);
                return 0;
        }

        /* set the read pointer to the start of the next packet */
        enc28j60_write_reg(ERDPTL, LO_BYTE(nxtpktptr));
        enc28j60_write_reg(ERDPTH, HI_BYTE(nxtpktptr));

        /* read next pkt pointer and rx status vector */
        enc28j60_read_mem(rx_pkt_hdr, RX_PKT_HDR_SZ, get_pool_mem(pidx));

        /* as per figure 7-3 of datasheet (page - 45), read next pkt pointer */
        nxtpktptr = rx_pkt_hdr[0]; // low byte
        nxtpktptr |= (rx_pkt_hdr[1] << 8); // high byte

        /* read length (incl. crc + padding) as per table 7-3 (page - 46) */
        pktlen = rx_pkt_hdr[2];
        pktlen |= rx_pkt_hdr[3] << 8;
        pktlen -= 4; // crc len

        /* limit the upcoming read size based on client memory size */
        if (pktlen > maxlen) {
                pktlen = maxlen;
        }

        /* read status, errors bits */
        rx_status = rx_pkt_hdr[4];
        rx_status |= (rx_pkt_hdr[5] << 8);

        /* copy the new message from ENCJ60 hardware to client buffer, if ok */
        if (rx_status & 0x80) {
                enc28j60_read_mem(pktptr, pktlen, get_pool_mem(pidx));
        }
        else {
                pktlen = 0; // Rx error present, hence ignore the packet
        }

        /* move Rx read pointer to nxtpktptr to free up buffer space in HW */
        enc28j60_write_reg(ERXRDPTL, LO_BYTE(nxtpktptr));
        enc28j60_write_reg(ERXRDPTH, HI_BYTE(nxtpktptr));

        /* inform HW that we are done with the reading of current packet */
        enc28j60_bitset_reg(ECON2, ECON2_PKTDEC);

        /* free the memory pool */
        free_mem_pool(pidx);

        LOG_DBG("rx_status = 0x%04x, next_pkt_ptr = 0x%04x", rx_status, nxtpktptr);

        return pktlen;
}



void macphy_periodic_fn(void) {
        int i;
        uint8 *pktptr;
        uint16 pktlen;

        /* if data already queued into the mpool */
        if (if_m_pool_has_data() == FALSE) {
                return;
        }

        /* but, if enc28j60 is in transmission state */
        if (get_active_pool_idx() != -1) {
                return;
        }

        /* if code reaches here, then some data is in the pool & enc28j60 is idle */
        for (i = 0; i < MEM_POOL_BUF_LEN; i++) {
                if(if_m_pool_mem_in_use(i)) {
                        pktptr = get_pool_mem(i)->tx_buf;
                        pktlen = get_pool_mem(i)->tx_len;
                        send_pkt_from_mpool(i, pktptr, pktlen);

                        /* ENC28J60 can send one msg at a time, hence return */
                        return;
                }
        }

        /* this function should be called only if there is no mem pool is in_use state */
        m_pool_scan_complete();
}



boolean macphy_init(const uint8 *mac_addr) {
        uint16 reg_bits;

        if (mac_addr == NULL) {
                LOG_ERR("%s: invalid MAC address!", __func__);
                return FALSE;
        }

        /* reset the chip first, set bank to 0 */
        enc28j60_sys_cmd(SC_RST_OPCODE);

        enc28j60_bitclr_reg(ECON1, 0x03);
        LOG_DBG("This build uses MACPHY: ENC28J60");

        /* read the chip revision IDs */
        MAC_RevId = enc28j60_read_reg(EREVID);
        LOG_DBG("MAC RevID: 0x%02x", MAC_RevId);

        /* set buffer memory layout - Rx */
        enc28j60_write_reg(ERXSTL, LO_BYTE(RX_BUF_BEG));
        enc28j60_write_reg(ERXSTH, HI_BYTE(RX_BUF_BEG));
        enc28j60_write_reg(ERXNDL, LO_BYTE(RX_BUF_END));
        enc28j60_write_reg(ERXNDH, HI_BYTE(RX_BUF_END));
        enc28j60_write_reg(ERXRDPTL, LO_BYTE(RX_BUF_END));
        enc28j60_write_reg(ERXRDPTH, HI_BYTE(RX_BUF_END));

        /* set buffer memory layout - Tx */
        enc28j60_write_reg(ETXSTL, LO_BYTE(TX_BUF_BEG));
        enc28j60_write_reg(ETXSTH, HI_BYTE(TX_BUF_BEG));
        enc28j60_write_reg(ETXNDL, LO_BYTE(TX_BUF_END));
        enc28j60_write_reg(ETXNDH, HI_BYTE(TX_BUF_END));
        enc28j60_write_reg(EWRPTL, LO_BYTE(TX_BUF_BEG));
        enc28j60_write_reg(EWRPTH, HI_BYTE(TX_BUF_BEG));

        /* set packet filter for reception */
        enc28j60_write_reg(ERXFCON, ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_BCEN);

        /* MAC configurations */
        enc28j60_write_reg(MACON1, MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS);
        enc28j60_write_reg(MACON2, 0x00);
        reg_bits = MACON3_PADCFG1 | MACON3_PADCFG0; // pad to 64 bytes + CRC
#if defined(MACPHY_HALF_DUPLEX)
        enc28j60_write_reg(MACON3, (uint8)(reg_bits | MACON3_TXCRCEN | MACON3_FRMLNEN));
        enc28j60_write_reg(MABBIPG, 0x12);
#else
        enc28j60_write_reg(MACON3, (uint8)(reg_bits | MACON3_TXCRCEN | MACON3_FRMLNEN | MACON3_FULDPX));
        enc28j60_write_reg(MABBIPG, 0x15);
#endif

        /* other interframe gap configurations */
        enc28j60_write_reg(MAIPGL, 0x12);
        enc28j60_write_reg(MAIPGH, 0x0C);

        /* max frame length configurations */
        enc28j60_write_reg(MAMXFLL, HI_BYTE(MAX_ETH_FRAME_LEN));
        enc28j60_write_reg(MAMXFLH, LO_BYTE(MAX_ETH_FRAME_LEN));

        /* write MAC address - bit 48 on byte 0, hence reversed */
        enc28j60_write_reg(MAADR5, mac_addr[0]);
        enc28j60_write_reg(MAADR4, mac_addr[1]);
        enc28j60_write_reg(MAADR3, mac_addr[2]);
        enc28j60_write_reg(MAADR2, mac_addr[3]);
        enc28j60_write_reg(MAADR1, mac_addr[4]);
        enc28j60_write_reg(MAADR0, mac_addr[5]);

        /* Configure PHY */
        //----------------
        /*   Note: all PHY registers should not be read or written to until
             at least 50 μs have passed since the Reset has ended. */
        reg_bits = enc28j60_read_phy(PHID1);
        PHY_Id = reg_bits << 3; // bits[18:3]
        reg_bits = enc28j60_read_phy(PHID2);
        // phy_id = ((phid2 & 0xFC00) >> 10) << 19 /*bits[15:10] --> bits[24:19]*/ |  phid1 << 3 /*bits[18:3]*/;
        PHY_Id |= (reg_bits & 0xFC00) << (19-10); //bits[24:19]
        PHY_Rev = (uint8) reg_bits & 0x0F;
        LOG_DBG("PHY ID: 0x%x", PHY_Id);
        LOG_DBG("PHY RevID: 0x%02x", PHY_Rev);

#if defined(MACPHY_HALF_DUPLEX)
        enc28j60_write_phy(PHCON1, 0x0000);
#else
        enc28j60_write_phy(PHCON1, PHCON1_PDPXMD); // PHY in full-duplex
#endif
        // LED-A (green): Link status; LED-B: Tx/Rx activity, LED Pulse Stretched = 139 ms
        enc28j60_write_phy(PHLCON, 0x047A);
        enc28j60_write_phy(PHCON2, 0); // normal operation. To debug set PHCON2_FRCLNK and check

        /* Enable packet receiption */
        enc28j60_bitset_reg(EIE, EIE_INTIE | EIE_PKTIE);
        enc28j60_bitset_reg(ECON1, ECON1_RXEN | ECON1_CSUMEN);

        LOG_DBG("ENC28J60 init complete!");

        return TRUE;
}
