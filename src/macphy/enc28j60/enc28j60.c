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


void MacPhy_Init(void) {
	// read location 0x00 == ERDPTL (reset value = 0xfa)
	spi_data[0] = 0x00;
	Spi_WriteIB(0, (const u8*)spi_data);
	pr_log("Spi Tx data: 0x%02x\n", spi_data[0]);
	if (E_NOT_OK == Spi_SyncTransmit(SEQ_ETH_2_BYTE)) {
		pr_log("Spi Sync Transmit Failure!\n");
	}
	Spi_ReadIB(1, spi_data);
	pr_log("Spi Rx data: 0x%02x\n", spi_data[0]);

	// read location 0x01 == ERDPTH (reset value = 0x05)
	spi_data[0] = 0x01;
	Spi_WriteIB(0, (const u8*)spi_data);
	pr_log("Spi Tx data: 0x%02x\n", spi_data[0]);
	if (E_NOT_OK == Spi_SyncTransmit(SEQ_ETH_2_BYTE)) {
		pr_log("Spi Sync Transmit Failure!\n");
	}
	Spi_ReadIB(1, spi_data);
	pr_log("Spi Rx data: 0x%02x\n", spi_data[0]);
}