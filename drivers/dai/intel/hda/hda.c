/*
 * Copyright (c) 2022 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <zephyr/spinlock.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>

#define DT_DRV_COMPAT intel_hda_dai
#define LOG_DOMAIN dai_intel_hda

LOG_MODULE_REGISTER(LOG_DOMAIN);

#include "hda.h"
#include <hda_regs.h>

static int dai_hda_trigger(const struct device *dev, enum dai_dir dir,
			   enum dai_trigger_cmd cmd)
{
	LOG_DBG("cmd %d", cmd);

	return 0;
}

/* Digital Audio interface formatting */
static int dai_hda_set_config_tplg(struct dai_intel_hda *dp, const void *spec_config)
{
	struct dai_intel_hda_pdata *hda = dai_get_drvdata(dp);
	const struct dai_intel_ipc_hda_params *config = spec_config;

	if (config->channels)
		hda->params.channels = config->channels;
	if (config->rate)
		hda->params.rate = config->rate;

	return 0;
}

static int dai_hda_config_get(const struct device *dev, struct dai_config *cfg, enum dai_dir dir)
{
	struct dai_config *params = (struct dai_config *)dev->config;
	struct dai_intel_hda *dp = (struct dai_intel_hda *)dev->data;
	struct dai_intel_hda_pdata *hda = dai_get_drvdata(dp);

	if (!cfg) {
		return -EINVAL;
	}

	params->rate = hda->params.rate;
	params->channels = hda->params.channels;

	params->word_size = DAI_INTEL_HDA_DEFAULT_WORD_SIZE;

	*cfg = *params;

	return 0;
}

static int dai_hda_config_set(const struct device *dev, const struct dai_config *cfg,
				  const void *bespoke_cfg)
{
	struct dai_intel_hda *dp = (struct dai_intel_hda *)dev->data;

	if (cfg->type == DAI_INTEL_HDA)
		return dai_hda_set_config_tplg(dp, bespoke_cfg);

	return 0;
}

static const struct dai_properties *dai_hda_get_properties(const struct device *dev,
							   enum dai_dir dir, int stream_id)
{
	struct dai_intel_hda *dp = (struct dai_intel_hda *)dev->data;
	struct dai_intel_hda_pdata *hda = dai_get_drvdata(dp);
	struct dai_properties *prop = &hda->props;

	prop->fifo_address = 0;
	prop->dma_hs_id = 0;
	prop->stream_id = 0;

	return prop;
}

static int dai_hda_probe(const struct device *dev)
{
	LOG_DBG("%s", __func__);

	return 0;
}

static int dai_hda_remove(const struct device *dev)
{
	LOG_DBG("%s", __func__);

	return 0;
}

#define DAI_INTEL_SSP_CLOCK_XTAL_OSCILLATOR	0x0
#define DAI_INTEL_SSP_CLOCK_AUDIO_CARDINAL	0x1
#define DAI_INTEL_SSP_CLOCK_PLL_FIXED		0x2

static uint32_t ssp_freq_sources[] = {
	DAI_INTEL_SSP_CLOCK_AUDIO_CARDINAL,
	DAI_INTEL_SSP_CLOCK_XTAL_OSCILLATOR,
	DAI_INTEL_SSP_CLOCK_PLL_FIXED,
};

struct dai_intel_ssp_freq_table {
	uint32_t freq;
	uint32_t ticks_per_msec;
};

// static struct dai_intel_ssp_freq_table ssp_freq_table[] = {
// 	{ DT_PROP(DT_NODELABEL(audioclk), clock_frequency),
// 	  DT_PROP(DT_NODELABEL(audioclk), clock_frequency) / 1000},
// 	{ DT_PROP(DT_NODELABEL(sysclk), clock_frequency),
// 	  DT_PROP(DT_NODELABEL(sysclk), clock_frequency) / 1000},
// 	{ DT_PROP(DT_NODELABEL(pllclk), clock_frequency),
// 	  DT_PROP(DT_NODELABEL(pllclk), clock_frequency) / 1000},
// };

// static int dai_hda_timestamp_config(const struct device *dev, struct dai_ts_cfg *cfg)
// {
// 	int i;

// 	cfg->walclk_rate = 0;
// 	for (i = 0; i < NUM_SSP_FREQ; i++) {
// 		if (ssp_freq_sources[i] == DAI_INTEL_SSP_CLOCK_XTAL_OSCILLATOR)
// 			cfg->walclk_rate = ssp_freq_table[i].freq;
// 	}

// 	return 0;
// }

// static int dai_timestamp_hda_start(const struct device *dev, struct dai_ts_cfg *cfg)
// {
// 	uint32_t addr = TIMESTAMP_BASE + TS_HDA_LOCAL_TSCTRL;
// 	uint32_t cdmas;

// 	/* Set HDA timestamp registers */

// 	/* First point CDMAS to GPDMA channel that is used by DMIC
// 	 * also clear NTK to be sure there is no old timestamp.
// 	 */
// 	cdmas = FIELD_PREP(TS_LOCAL_TSCTRL_CDMAS, cfg->dma_chan_index +
// 		cfg->dma_chan_count * cfg->dma_id);
// 	sys_write32(TS_LOCAL_TSCTRL_NTK | cdmas, addr);

// 	/* Request on demand timestamp */
// 	sys_write32(TS_LOCAL_TSCTRL_ODTS | cdmas, addr);

// 	return 0;
// }

// static int dai_timestamp_hda_stop(const struct device *dev, struct dai_ts_cfg *cfg)
// {
// 	/* Clear NTK and write zero to CDMAS */
// 	sys_write32(TS_LOCAL_TSCTRL_NTK, TIMESTAMP_BASE + TS_HDA_LOCAL_TSCTRL);
// 	return 0;
// }

// static int dai_timestamp_hda_get(const struct device *dev, struct dai_ts_cfg *cfg,
// 				  struct dai_ts_data *tsd)
// {
// 	/* Read HDA timestamp registers */
// 	uint32_t tsctrl = TIMESTAMP_BASE + TS_HDA_LOCAL_TSCTRL;
// 	uint32_t ntk;

// 	ntk = sys_read32(tsctrl) & TS_LOCAL_TSCTRL_NTK;
// 	if (!ntk)
// 		goto out;

// 	/* NTK was set, get wall clock */
// 	tsd->walclk = sys_read64(TIMESTAMP_BASE + TS_HDA_LOCAL_WALCLK);

// 	/* Sample */
// 	tsd->sample = sys_read64(TIMESTAMP_BASE + TS_DMIC_LOCAL_SAMPLE_OFFSET);

// 	/* Clear NTK to enable successive timestamps */
// 	sys_write32(TS_LOCAL_TSCTRL_NTK, tsctrl);

// out:
// 	tsd->walclk_rate = cfg->walclk_rate;
// 	if (!ntk)
// 		return -ENODATA;

// 	return 0;
// }

static const struct dai_driver_api dai_intel_hda_api_funcs = {
	.probe			= dai_hda_probe,
	.remove			= dai_hda_remove,
	.config_set		= dai_hda_config_set,
	.config_get		= dai_hda_config_get,
	.trigger		= dai_hda_trigger,
	.get_properties		= dai_hda_get_properties,
	// .ts_config		= dai_hda_timestamp_config,
	// .ts_start		= dai_timestamp_hda_start,
	// .ts_stop		= dai_timestamp_hda_stop,
	// .ts_get			= dai_timestamp_hda_get
};

#define DAI_INTEL_HDA_DEVICE_INIT(n)				\
	static struct dai_config dai_intel_hda_config_##n = {	\
		.type = DAI_INTEL_HDA,				\
		.dai_index = DT_INST_REG_ADDR(n),	\
	};							\
	static struct dai_intel_hda dai_intel_hda_data_##n = {	\
		.index = DT_INST_REG_ADDR(n)		\
								\
	};							\
								\
	DEVICE_DT_INST_DEFINE(n,				\
			NULL, NULL,				\
			&dai_intel_hda_data_##n,		\
			&dai_intel_hda_config_##n,		\
			POST_KERNEL, 32,			\
			&dai_intel_hda_api_funcs);

DT_INST_FOREACH_STATUS_OKAY(DAI_INTEL_HDA_DEVICE_INIT)
