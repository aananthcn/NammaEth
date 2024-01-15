#ifndef NAMMA_AUTOSAR_ETH_CFG_H
#define NAMMA_AUTOSAR_ETH_CFG_H

// This file is autogenerated, any hand modifications will be lost!

#include <Platform_Types.h>

#include <Spi_cfg.h>



typedef struct {
    uint16  mainfn_period_ms;
    uint8   index;
    boolean dev_error_detect;
    boolean get_cntr_val_api;
    boolean get_rx_stats_api;
    boolean get_tx_stats_api;
    boolean get_tx_erctv_api; /* EthGetTxErrorCounterValuesApi */
    boolean get_gbl_time_api;
    uint8   max_ctrl_suportd;
    boolean version_info_api;
} EthGeneralCfgType;


typedef struct {
    boolean en_cksum_ipv4;
    boolean en_cksum_icmp;
    boolean en_cksum_tcp;
    boolean en_cksum_udp;
} EthCtrlOffloadingType;


typedef enum {
    ETH_MAC_LAYER_SPEED_10M,
    ETH_MAC_LAYER_SPEED_100M,
    ETH_MAC_LAYER_SPEED_1G,
    ETH_MAC_LAYER_SPEED_2500M,
    ETH_MAC_LAYER_SPEED_10G
} EthCtrlMacLayerSpeed;


typedef enum {
    ETH_MAC_LAYER_TYPE_XMII,
    ETH_MAC_LAYER_TYPE_XGMII,
    ETH_MAC_LAYER_TYPE_XXGMII
} EthCtrlMacLayerType;


typedef enum {
    REDUCED,
    REVERSED,
    SERIAL,
    STANDARD,
    UNIVERSAL_SERIAL
} EthCtrlMacLayerSubType;


typedef enum {
	ETH_DEV_NONE,
	ETH_DEV_ENC28J60,
	MAX_ETH_DEV
} EthControllerDevType;


typedef struct {
    boolean                 buf_handlg;
    boolean                 enable_mii;
    boolean                 enable_spi;
    boolean                 en_rx_intr;
    boolean                 en_tx_intr;
    uint8                   ctrl_index;
    EthCtrlMacLayerSpeed    mac_lr_spd;
    EthCtrlMacLayerType     mac_lr_typ;
    EthCtrlMacLayerSubType  mac_sb_typ;
    uint8                   mac_addres[6];
    EthControllerDevType    spi_device;
} EthCtrlConfigType;


typedef struct {
    const uint16    buff_len;
    const uint16    buf_totl;
    const uint16    fifo_idx;
    const uint8     fifoprio;
} Eth_ConfigFifoType;


typedef struct {
    const uint32 predes_order;
} Eth_ConfigSchedulerType;


typedef struct {
    const uint32 idle_slope;
    const uint32 max_credit;
    const uint32 min_credit;
} Eth_ConfigShaperType;


typedef struct {
    const uint8                 pay_ld_size;
    const uint8                 com_retries;
    const uint32                ctimeout_ms; /* Comm. Timeout */
    const boolean               ctrldatprot;
    const boolean               rx_cs_align;
    const boolean               rx_cut_thru;
    const boolean               rx_zero_aln;
    const boolean               txd_hdr_seq;
    const boolean               tx_en_cksum;
    const boolean               tx_cut_thru;
    const boolean               spi_timstmp;
    const uint8                 tx_crdthrsh; /* Credit Threshold */
    const boolean               spi_syncacc; /* Accesss Synchronous */
    const Spi_SequenceEnumType  spisequence;
} Eth_ConfigSpiCfgType;


typedef struct {
    const EthGeneralCfgType         general;
    const EthCtrlOffloadingType     offload;
    const EthCtrlConfigType         ctrlcfg;
    const Eth_ConfigFifoType        fifo_ig;
    const Eth_ConfigFifoType        fifo_eg;
    const Eth_ConfigSchedulerType   sched_c;
    const Eth_ConfigShaperType      shape_c;
    const Eth_ConfigSpiCfgType      spi_cfg;
} Eth_ConfigType;


#define ETH_DRIVER_MAX_CHANNEL    (1)


extern const Eth_ConfigType EthConfigs[ETH_DRIVER_MAX_CHANNEL];


#endif
