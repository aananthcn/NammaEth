/*
 * Created on Mon Dec 19 2022 5:28:31 PM
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
#ifndef NAMMA_ETH_H
#define NAMMA_ETH_H

#include <Platform_Types.h>
#include <Std_Types.h>
#include <Eth_cfg.h>

/* SpiStatusRegister bit fields */
#define TRANSMIT_PROTOCOL_ERROR_BIT             (0x00)
#define TRANSMIT_BUFFER_OVERFLOW_ERROR          (0x01)
#define TRANSMIT_BUFFER_UNDERFLOW_ERROR         (0x02)
#define RECEIVE_BUFFER_OVERFLOW_ERROR           (0x03)
#define LOSS_FRAMING_ERROR                      (0x04)
#define HEADER_ERROR                            (0x05)
#define RESET_COMPLETE                          (0x06)
#define PHY_INTERRUPT                           (0x07)
#define TRANSMIT_TIMESTAMP CAPTURE_AVAILABLE_A  (0x08)
#define TRANSMIT_TIMESTAMP CAPTURE_AVAILABLE_B  (0x09)
#define TRANSMIT_TIMESTAMP CAPTURE_AVAILABLE_C  (0x0A)
#define TRANSMIT_FRAME_CHECK_SEQUENCE_ERROR     (0x0C)
#define CONTROL_DATA_PROTECTION_ERROR           (0x0D)

typedef struct {
	uint32 SpiStatusRegister;
	boolean Sync; /*  TRUE: MACPHY is not configured. FALSE: MACPHY is configured */
	uint8 BufferStatusTxCredit; /* Frames that can be written to MAC without overflow */
	uint8 BufferStatusRxCredit; /* Frames that is avaialble for the host to read it */
} Eth_SpiStatusType;


void Eth_Init(const Eth_ConfigType* CfgPtr);


#endif
