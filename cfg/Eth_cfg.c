#include <stddef.h>
#include <Eth_cfg.h>


// This file is autogenerated, any hand modifications will be lost!



const Eth_ConfigType EthConfigs[ETH_DRIVER_MAX_CHANNEL] = {
	{
		/* Eth channel - 0 */
		.general = {
			.index = 0,
			.mainfn_period_ms = 10,
			.dev_error_detect = FALSE,
			.get_cntr_val_api = FALSE,
			.get_rx_stats_api = FALSE,
			.get_tx_stats_api = FALSE,
			.get_tx_erctv_api = FALSE,
			.get_gbl_time_api = FALSE,
			.max_ctrl_suportd = 1,
			.version_info_api = FALSE
		},
		.offload = {
			.en_cksum_ipv4 = FALSE,
			.en_cksum_icmp = FALSE,
			.en_cksum_tcp = FALSE,
			.en_cksum_udp = FALSE
		},
		.ctrlcfg = {
			.buf_handlg = FALSE,
			.enable_mii = TRUE,
			.enable_spi = TRUE,
			.spi_device = ETH_DEV_ENC28J60,
			.en_rx_intr = FALSE,
			.en_tx_intr = FALSE,
			.ctrl_index = 0,
			.mac_lr_spd = ETH_MAC_LAYER_SPEED_10M,
			.mac_lr_typ = ETH_MAC_LAYER_TYPE_XMII,
			.mac_sb_typ = STANDARD,
			.mac_addres = {0x00, 0x00, 0x5e, 0x00, 0x53, 0x00},
		},
		.fifo_ig = {
			.buff_len = 128,
			.buf_totl = 100,
			.fifo_idx = 0,
			.fifoprio = 7
		},
		.fifo_eg = {
			.buff_len = 128,
			.buf_totl = 100,
			.fifo_idx = 0,
			.fifoprio = 7
		},
		.sched_c = {
			.predes_order = 0
		},
		.shape_c = {
			.idle_slope = 1,
			.max_credit = 1,
			.min_credit = 1
		},
		.spi_cfg = {
			.pay_ld_size = 64,
			.com_retries = 1,
			.ctimeout_ms = 100,
			.ctrldatprot = FALSE,
			.rx_cs_align = FALSE,
			.rx_cut_thru = FALSE,
			.rx_zero_aln = FALSE,
			.txd_hdr_seq = FALSE,
			.tx_en_cksum = FALSE,
			.tx_cut_thru = FALSE,
			.spi_timstmp = FALSE,
			.tx_crdthrsh = 0,
			.spi_syncacc = FALSE,
			.spisequence = SEQ_ETH_2_BYTE
		}
	},
};
