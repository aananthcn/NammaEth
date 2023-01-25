/*
 * Created on Tue Dec 20 2022 6:59:35 AM
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
#include <Eth.h>

#include <macphy.h>


void Eth_Init(const Eth_ConfigType* CfgPtr) {
	// body of the function
	int i;
	for (i = 0; i < ETH_DRIVER_MAX_CHANNEL; i++) {
		if (CfgPtr[i].ctrlcfg.enable_mii == TRUE) {
			// call function to initialize the MACPHY via SPI
			macphy_init(CfgPtr[i].ctrlcfg.mac_addres);
		}
	}
}



// Provides access to a transmit buffer of the specified Ethernet controller
BufReq_ReturnType Eth_ProvideTxBuffer(uint8 CtrlIdx, uint8 Priority, Eth_BufIdxType* BufIdxPtr,
	uint8** BufPtr, uint16* LenBytePtr) {

	return BUFREQ_E_NOT_OK;
}


// Triggers frame transmission confirmation
void Eth_TxConfirmation(uint8 CtrlIdx) {

}


// Receive a frame from the related fifo
void Eth_Receive(uint8 CtrlIdx, uint8 FifoIdx, Eth_RxStatusType* RxStatusPtr) {

}


Std_ReturnType Eth_Transmit(uint8 CtrlIdx, Eth_BufIdxType BufIdx, Eth_FrameType FrameType,
	boolean TxConfirmation, uint16 LenByte, const uint8* PhysAddrPtr) {
	Std_ReturnType retc = E_NOT_OK;

	return retc;
}