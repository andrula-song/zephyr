/* SPDX-License-Identifier: Apache-2.0 */
/*
 * Copyright (c) 2023 Intel Corporation
 *
 * Author: Adrian Warecki <adrian.warecki@intel.com>
 */

#ifndef __INTEL_DAI_DRIVER_HDA_REGS_H__
#define __INTEL_DAI_DRIVER_HDA_REGS_H__

/** \brief Number of 'base' SSP ports available */
#define DAI_NUM_SSP_BASE	6

/** \brief Number of 'extended' SSP ports available */
#define DAI_NUM_SSP_EXT		0

/** \brief Number of SSP MCLKs available */
#define DAI_NUM_SSP_MCLK	2

#define NUM_SSP_FREQ	3

/* SSP timestamping registers */
#define TS_I2S_LOCAL_TSCTRL(x)	(0x100 + 0x20 * (x) + 0x00)
#define TS_I2S_LOCAL_OFFS(x)	(0x100 + 0x20 * (x) + 0x04)
#define TS_I2S_LOCAL_SAMPLE(x)	(0x100 + 0x20 * (x) + 0x08)
#define TS_I2S_LOCAL_WALCLK(x)	(0x100 + 0x20 * (x) + 0x10)

/* New Timestamp Taken */
#define TS_LOCAL_TSCTRL_NTK		BIT(31)

/* Capture DMA Select */
#define TS_LOCAL_TSCTRL_CDMAS		GENMASK(4, 0)

/* On Demand Time Stamp */
#define TS_LOCAL_TSCTRL_ODTS		BIT(5)

#define TS_DMIC_LOCAL_TSCTRL_OFFSET	0x000
#define TS_DMIC_LOCAL_OFFS_OFFSET	0x004
#define TS_DMIC_LOCAL_SAMPLE_OFFSET	0x008
#define TS_DMIC_LOCAL_WALCLK_OFFSET	0x010
#define TS_DMIC_TSCC_OFFSET		0x018

/* Timestamping */
#define TIMESTAMP_BASE			0x00071800



#endif /* !__INTEL_DAI_DRIVER_HDA_REGS_H__ */
