/*
 * Created on Thu Feb 02 2023 2:11:51 AM
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

#include "macphy_mpool.h"


//////////////////////////////////////////////
// BASIC ETHERNET Tx & Rx Buffers
static MacSpiMemType SpiMemPool[MEM_POOL_SIZE];


// Memory Pool Functions
int get_m_pool_index(void) {
        int i, pool_idx;

        pool_idx = -1;
        for (i = 0; i < MEM_POOL_SIZE; i++) {
                if (SpiMemPool[i].in_use == FALSE) {
                        SpiMemPool[i].in_use = TRUE;
                        pool_idx = i;
                        break;
                }
        }

        return pool_idx;
}


MacSpiMemType* get_pool_mem(int idx) {
        if (idx >= MEM_POOL_SIZE) {
                return NULL;
        }

        return &SpiMemPool[idx];
}


void free_mem_pool(int idx) {
        if (idx >= MEM_POOL_SIZE) {
                return;
        }

        SpiMemPool[idx].in_use = FALSE;
}
