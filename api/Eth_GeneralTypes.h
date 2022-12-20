/*
 * Created on Mon Dec 19 2022 5:25:54 PM
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
#ifndef ETH_GENERAL_TYPES_H
#define ETH_GENERAL_TYPES_H

#include <Platform_Types.h>
#include <Std_Types.h>


typedef enum {
	ETH_MODE_DOWN, /* sleep - for OA TC10 compatible Ethernet hardware */
	ETH_MODE_ACTIVE,
	ETH_MODE_ACTIVE_WITH_WAKEUP_REQUEST, /* for OA TC10 compatible Ethernet hardware */
	ETH_MODE_ACTIVE_TX_OFFLINE
} Eth_ModeType;


typedef enum {
	ETH_STATE_UNINIT,
	ETH_STATE_INIT
} Eth_StateType;


typedef uint16 Eth_FrameType;
typedef uint8 Eth_DataType;
// Eth_DataType depends on the CPU type.
// typedef uint16 Eth_DataType;
// typedef uint32 Eth_DataType;
typedef uint32 Eth_BufIdxType;


typedef enum {
	ETH_RECEIVED,
	ETH_NOT_RECEIVED,
	ETH_RECEIVED_MORE_DATA_AVAILABLE
} Eth_RxStatusType;


typedef enum  {
	ETH_ADD_TO_FILTER,
	ETH_REMOVE_FROM_FILTER
} Eth_FilterActionType;


typedef enum {
	ETH_VALID,
	ETH_INVALID,
	ETH_UNCERTAIN
}Eth_TimeStampQualType;



typedef struct {
	uint32 nanoseconds;
	uint32 seconds;
	uint16 secondsHi;
} Eth_TimeStampType;



typedef struct {
	Eth_TimeStampType diff;
	boolean sign;
} Eth_TimeIntDiffType;


typedef struct {
	Eth_TimeIntDiffType IngressTimeStampDelta;
	Eth_TimeIntDiffType OriginTimeStampDelta;
} Eth_RateRatioType;


typedef struct {
	uint8 MacAddr[6];
	uint16 VlanId; /* Specifies the VLAN address 0..65535 */
	Uint32 SwitchPort; /* Specifies the ports of the switch as bit mask (0x00000001->Port0, 0x80000001->Port31+Port0) */
} Eth_MacVlanType;


/* Statistic counter for diagnostics. */
typedef struct {
	uint32 DropPktBufOverrun;
	uint32 DropPktCrc;
	uint32 UndersizePkt;
	uint32 OversizePkt;
	uint32 AlgnmtErr;
	uint32 SqeTestErr; /* IETF RFC1643 dot3StatsSQETestErrors */
	uint32 DiscInbdPkt; /* discarded inbound packet */
	uint32 ErrInbdPkt;
	uint32 DiscOtbdPkt; /* discarded outbound packet */
	uint32 ErrOtbdPkt;
	uint32 SnglCollPkt; /* Single collision frames */
	uint32 MultCollPkt;
	uint32 DfrdPkt; /* deferred transmission */
	uint32 LatCollPkt; /* late collisions */
	uint32 HwDepCtr0;
	uint32 HwDepCtr1;
	uint32 HwDepCtr2;
	uint32 HwDepCtr3;
} Eth_CounterType;


typedef struct {
	uint32 RxStatsDropEvents;
	uint32 RxStatsOctets;
	uint32 RxStatsBroadcastPkts;
	uint32 RxStatsMulticastPkts;
	uint32 RxStatsCrcAlignErrors;
	uint32 RxStatsUndersizePkts;
	uint32 RxStatsOversizePkts;
	uint32 RxStatsFragments;
	uint32 RxStatsJabbers;
	uint32 RxStatsCollisions;
	uint32 RxStatsPkts64Octets;
	uint32 RxStatsPkts65to127Octets;
	uint32 RxStatsPkts128to255Octets;
	uint32 RxStatsPkts256to511Octets;
	uint32 RxStatsPkts512to1023Octets;
	uint32 RxStatsPkts1024to1518Octets;
	uint32 RxUnicastFrames;
} Eth_RxStatsType;


typedef struct {
	uint32 TxNumberOfOctets;
	uint32 TxNUcastPkts;
	uint32 TxUniCastPkts;
} Eth_TxStatsType;


typedef struct {
	uint32 TxDroppedNoErrorPkts;
	uint32 TxDroppedErrorPkts;
	uint32 TxDeferredTrans;
	uint32 TxSingleCollision;
	uint32 TxMultipleCollision;
	uint32 TxLateCollision;
	uint32 TxExcessiveCollison;
} Eth_TxErrorCounterValuesType;






#endif
