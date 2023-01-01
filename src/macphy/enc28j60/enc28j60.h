/*
 * Created on Sun Dec 25 2022 1:34:15 PM
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
#ifndef ETH_ENC28J60_H
#define ETH_ENC28J60_H

#include "enc28j60_regs.h"


/////////////////////////////////////////
///     ENC28J60 INSTRUCTION SETS      //
/////////////////////////////////////////
#define RD_REG_OPCODE   (0x00) /* here bit[4:0] would by ORed by reg addr */
#define RD_MEM_OPCODE   (0x3A)    
#define WR_REG_OPCODE   (0x40) /* here bit[4:0] would by ORed by reg addr */
#define WR_MEM_OPCODE   (0x7A)
#define BT_SET_OPCODE   (0x80) /* here bit[4:0] would by ORed by arg */
#define BT_CLR_OPCODE   (0xA0) /* here bit[4:0] would by ORed by arg */
#define SC_RST_OPCODE   (0xFF)



/////////////////////////////////////////
///   Declarations & Definitions       //
/////////////////////////////////////////
typedef enum {
        MACPHY_UNINIT,
        MACPHY_INIT,
        MACPHY_MAX_STATE
} MacPhyStateType;


// public functions
boolean macphy_init(const uint8 *mac_addr);

uint8   enc28j60_read_reg(uint16 reg);
boolean enc28j60_write_reg(uint16 reg, uint8 data);
boolean enc28j60_bitset_reg(uint16 reg, uint8 data);
boolean enc28j60_bitclr_reg(uint16 reg, uint8 data);

uint16 enc28j60_read_phy(uint8 phyaddr);


#endif
