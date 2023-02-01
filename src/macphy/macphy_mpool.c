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
static int active_pool_idx = -1;
static boolean data_in_pool = FALSE;


// Memory Pool Functions
int get_m_pool_index(void) {
        int i, pool_idx;

        pool_idx = -1;
        for (i = 0; i < MEM_POOL_SIZE; i++) {
                if (SpiMemPool[i].in_use == FALSE) {
                        SpiMemPool[i].in_use = TRUE;
                        data_in_pool = TRUE;
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



int get_active_pool_idx(void) {
        return active_pool_idx;
}

void set_active_pool_idx(int idx) {
        active_pool_idx = idx;
}

void clr_active_pool_idx() {
        active_pool_idx = -1;
}

boolean if_m_pool_has_data(void) {
        return data_in_pool;
}

void m_pool_scan_complete(void) {
        data_in_pool = FALSE;
}

boolean if_m_pool_mem_in_use(int idx) {
        if (idx >= MEM_POOL_SIZE) {
                return FALSE;
        }

        return SpiMemPool[idx].in_use;
}