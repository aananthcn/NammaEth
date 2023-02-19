/*
 * Created on Sun Jan 01 2023 9:00:08 AM
 *
 * The MIT License (MIT)
 * Copyright (c) 2023 Aananth C N
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
#ifndef ETH_ENC28J60_REGS_H
#define ETH_ENC28J60_REGS_H

/////////////////////////////////////////
///    ENC28J60 CONTROL REGISTERS      //
/////////////////////////////////////////
// Encoding                            //
// ~~~~~~~~                            //
// byte 0, bits[0:7] - reg. address    //
// byte 1, bits[0:5] - bank number     //
// byte 1, bit[6]    - Common register //
// byte 1, bit[7]    - MAC or MII reg. //
/////////////////////////////////////////

// ENC28J60 Common Registers
#define EIE             (0x4000 | 0x1B)
#define EIR             (0x4000 | 0x1C)
#define ESTAT           (0x4000 | 0x1D)
#define ECON2           (0x4000 | 0x1E)
#define ECON1           (0x4000 | 0x1F)

// ENC28J60 EIR Register Bit Definitions
#define EIR_PKTIF 	(0x40)
#define EIR_DMAIF 	(0x20)
#define EIR_LINKIF 	(0x10)
#define EIR_TXIF 	(0x08)
#define EIR_WOLIF 	(0x04)
#define EIR_TXERIF 	(0x02)
#define EIR_RXERIF 	(0x01)

// ENC28J60 ESTAT Register Bit Definitions
#define ESTAT_INT 	(0x80)
#define ESTAT_LATECOL  	(0x10)
#define ESTAT_RXBUSY    (0x04)
#define ESTAT_TXABRT    (0x02)
#define ESTAT_CLKRDY    (0x01)


// ENC28J60 Bank-0 Registers
#define ERDPTL          (0x0000 | 0x00)
#define ERDPTH          (0x0000 | 0x01)
#define EWRPTL          (0x0000 | 0x02)
#define EWRPTH          (0x0000 | 0x03)
#define ETXSTL          (0x0000 | 0x04)
#define ETXSTH          (0x0000 | 0x05)
#define ETXNDL          (0x0000 | 0x06)
#define ETXNDH          (0x0000 | 0x07)
#define ERXSTL          (0x0000 | 0x08)
#define ERXSTH          (0x0000 | 0x09)
#define ERXNDL          (0x0000 | 0x0A)
#define ERXNDH          (0x0000 | 0x0B)
#define ERXRDPTL        (0x0000 | 0x0C)
#define ERXRDPTH        (0x0000 | 0x0D)
#define ERXWRPTL        (0x0000 | 0x0E)
#define ERXWRPTH        (0x0000 | 0x0F)
#define EDMASTL         (0x0000 | 0x10)
#define EDMASTH         (0x0000 | 0x11)
#define EDMANDL         (0x0000 | 0x12)
#define EDMANDH         (0x0000 | 0x13)
#define EDMADSTL        (0x0000 | 0x14)
#define EDMADSTH        (0x0000 | 0x15)
#define EDMACSL         (0x0000 | 0x16)
#define EDMACSH         (0x0000 | 0x17)

// ENC28J60 Bank-1 Registers
#define EHT0            (0x0100 | 0x00)
#define EHT1            (0x0100 | 0x01)
#define EHT2            (0x0100 | 0x02)
#define EHT3            (0x0100 | 0x03)
#define EHT4            (0x0100 | 0x04)
#define EHT5            (0x0100 | 0x05)
#define EHT6            (0x0100 | 0x06)
#define EHT7            (0x0100 | 0x07)
#define EPMM0           (0x0100 | 0x08)
#define EPMM1           (0x0100 | 0x09)
#define EPMM2           (0x0100 | 0x0A)
#define EPMM3           (0x0100 | 0x0B)
#define EPMM4           (0x0100 | 0x0C)
#define EPMM5           (0x0100 | 0x0D)
#define EPMM6           (0x0100 | 0x0E)
#define EPMM7           (0x0100 | 0x0F)
#define EPMCSL          (0x0100 | 0x10)
#define EPMCSH          (0x0100 | 0x11)
#define EPMOL           (0x0100 | 0x14)
#define EPMOH           (0x0100 | 0x15)
#define EWOLIE          (0x0100 | 0x16)
#define EWOLIR          (0x0100 | 0x17)
#define ERXFCON         (0x0100 | 0x18)
#define EPKTCNT         (0x0100 | 0x19)

// ENC28J60 Bank-2 Registers
#define MACON1          (0x8200 | 0x00)
#define MACON2          (0x8200 | 0x01)
#define MACON3          (0x8200 | 0x02)
#define MACON4          (0x8200 | 0x03)
#define MABBIPG         (0x8200 | 0x04)
#define MAIPGL          (0x8200 | 0x06)
#define MAIPGH          (0x8200 | 0x07)
#define MACLCON1        (0x8200 | 0x08)
#define MACLCON2        (0x8200 | 0x09)
#define MAMXFLL         (0x8200 | 0x0A)
#define MAMXFLH         (0x8200 | 0x0B)
#define MAPHSUP         (0x8200 | 0x0D)
#define MICON           (0x8200 | 0x11)
#define MICMD           (0x8200 | 0x12)
#define MIREGADR        (0x8200 | 0x14)
#define MIWRL           (0x8200 | 0x16)
#define MIWRH           (0x8200 | 0x17)
#define MIRDL           (0x8200 | 0x18)
#define MIRDH           (0x8200 | 0x19)

// ENC28J60 Bank-3 Registers
#define MAADR1          (0x8300 | 0x00)
#define MAADR0          (0x8300 | 0x01)
#define MAADR3          (0x8300 | 0x02)
#define MAADR2          (0x8300 | 0x03)
#define MAADR5          (0x8300 | 0x04)
#define MAADR4          (0x8300 | 0x05)
#define EBSTSD          (0x0300 | 0x06)
#define EBSTCON         (0x0300 | 0x07)
#define EBSTCSL         (0x0300 | 0x08)
#define EBSTCSH         (0x0300 | 0x09)
#define MISTAT          (0x8300 | 0x0A)
#define EREVID          (0x0300 | 0x12)
#define ECOCON          (0x0300 | 0x15)
#define EFLOCON         (0x0300 | 0x17)
#define EPAUSL          (0x0300 | 0x18)
#define EPAUSH          (0x0300 | 0x19)



/////////////////////////////////////////
///      ENC28J60 PHY REGISTERS        //
/////////////////////////////////////////
#define PHCON1          (0x00)
#define PHSTAT1         (0x01)
#define PHID1           (0x02)
#define PHID2           (0x03)
#define PHCON2          (0x10)
#define PHSTAT2         (0x11)
#define PHIE            (0x12)
#define PHIR            (0x13)
#define PHLCON          (0x14)



/////////////////////////////////////////
///      ENC28J60 REGISTER BITS        //
/////////////////////////////////////////
// ENC28J60 ERXFCON Register Bit Definitions
#define ERXFCON_UCEN    (0x80)
#define ERXFCON_ANDOR   (0x40)
#define ERXFCON_CRCEN   (0x20)
#define ERXFCON_PMEN    (0x10)
#define ERXFCON_MPEN    (0x08)
#define ERXFCON_HTEN    (0x04)
#define ERXFCON_MCEN    (0x02)
#define ERXFCON_BCEN    (0x01)

// ENC28J60 EIE Register Bit Definitions
#define EIE_INTIE       (0x80)
#define EIE_PKTIE       (0x40)
#define EIE_DMAIE       (0x20)
#define EIE_LINKIE      (0x10)
#define EIE_TXIE        (0x08)
#define EIE_WOLIE       (0x04)
#define EIE_TXERIE      (0x02)
#define EIE_RXERIE      (0x01)

// ENC28J60 ECON2 Register Bit Definitions
#define ECON2_AUTOINC   (0x80)
#define ECON2_PKTDEC    (0x40)
#define ECON2_PWRSV     (0x20)
#define ECON2_VRPS      (0x08)

// ENC28J60 ECON1 Register Bit Definitions
#define ECON1_TXRST     (0x80)
#define ECON1_RXRST     (0x40)
#define ECON1_DMAST     (0x20)
#define ECON1_CSUMEN    (0x10)
#define ECON1_TXRTS     (0x08)
#define ECON1_RXEN      (0x04)
#define ECON1_BSEL1     (0x02)
#define ECON1_BSEL0     (0x01)

// ENC28J60 MACON1 Register Bit Definitions
#define MACON1_LOOPBK   (0x10)
#define MACON1_TXPAUS   (0x08)
#define MACON1_RXPAUS   (0x04)
#define MACON1_PASSALL  (0x02)
#define MACON1_MARXEN   (0x01)

// ENC28J60 MACON2 Register Bit Definitions
#define MACON2_MARST    (0x80)
#define MACON2_RNDRST   (0x40)
#define MACON2_MARXRST  (0x08)
#define MACON2_RFUNRST  (0x04)
#define MACON2_MATXRST  (0x02)
#define MACON2_TFUNRST  (0x01)

// ENC28J60 MACON3 Register Bit Definitions
#define MACON3_PADCFG2  (0x80)
#define MACON3_PADCFG1  (0x40)
#define MACON3_PADCFG0  (0x20)
#define MACON3_TXCRCEN  (0x10)
#define MACON3_PHDRLEN  (0x08)
#define MACON3_HFRMLEN  (0x04)
#define MACON3_FRMLNEN  (0x02)
#define MACON3_FULDPX   (0x01)


// ENC28J60 MISTAT Register Bit Definitions
#define MISTAT_NVALID   (0x04)
#define MISTAT_SCAN     (0x02)
#define MISTAT_BUSY     (0x01)

// ENC28J60 MICMD Register Bit Definitions
#define MICMD_MIISCAN   (0x02)
#define MICMD_MIIRD     (0x01)

// ENC28J60 PHY PHCON1 Register Bit Definitions
#define PHCON1_PRST     (0x8000)
#define PHCON1_PLOOPBK  (0x4000)
#define PHCON1_PPWRSV   (0x0800)
#define PHCON1_PDPXMD   (0x0100)

// ENC28J60 PHY PHSTAT1 Register Bit Definitions
#define PHSTAT1_PFDPX   (0x1000)
#define PHSTAT1_PHDPX   (0x0800)
#define PHSTAT1_LLSTAT  (0x0004)
#define PHSTAT1_JBSTAT  (0x0002)

// ENC28J60 PHY PHCON2 Register Bit Definitions
#define PHCON2_FRCLNK   (0x4000)
#define PHCON2_TXDIS    (0x2000)
#define PHCON2_JABBER   (0x0400)
#define PHCON2_HDLDIS   (0x0100)


#endif
