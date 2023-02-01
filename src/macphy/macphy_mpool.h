/*
 * Created on Thu Feb 02 2023 2:12:06 AM
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
#ifndef SPI_MEM_POOL_H
#define SPI_MEM_POOL_H

#include <Platform_Types.h>
#include <Std_Types.h>
#include <stddef.h>

#define MEM_POOL_BUF_LEN        (1522)
#define MEM_POOL_SIZE           (2)


typedef struct {
        uint8 tx_buf[MEM_POOL_BUF_LEN];
        uint8 rx_buf[MEM_POOL_BUF_LEN];
        uint16 tx_len;
        boolean in_use;
} MacSpiMemType;


int get_m_pool_index(void);
MacSpiMemType* get_pool_mem(int idx);
void free_mem_pool(int idx);

int get_active_pool_idx(void);
void set_active_pool_idx(int idx);
void clr_active_pool_idx();
boolean if_m_pool_has_data(void);
void m_pool_scan_complete(void);
boolean if_m_pool_mem_in_use(int idx);

#endif
