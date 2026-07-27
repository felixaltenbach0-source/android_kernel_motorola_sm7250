/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _MSM_DBA_H
#define _MSM_DBA_H

#include <linux/types.h>
#include <linux/bitops.h>

#define MSM_DBA_CHIP_NAME_MAX_LEN 20
#define MSM_DBA_CLIENT_NAME_LEN   20

#define MSM_DBA_DEFER_PROPERTY_FLAG 0x1
#define MSM_DBA_ASYNC_FLAG          0x2


enum msm_dba_callback_event {
	MSM_DBA_CB_REMOTE_INT = BIT(0),
	MSM_DBA_CB_HDCP_LINK_AUTHENTICATED = BIT(1),
	MSM_DBA_CB_HDCP_LINK_UNAUTHENTICATED = BIT(2),
	MSM_DBA_CB_HPD_CONNECT = BIT(3),
	MSM_DBA_CB_HPD_DISCONNECT = BIT(4),
	MSM_DBA_CB_VIDEO_FAILURE = BIT(5),
	MSM_DBA_CB_AUDIO_FAILURE = BIT(6),
	MSM_DBA_CB_CEC_WRITE_SUCCESS = BIT(7),
	MSM_DBA_CB_CEC_WRITE_FAIL = BIT(8),
	MSM_DBA_CB_CEC_READ_PENDING = BIT(9),
	MSM_DBA_CB_PRE_RESET = BIT(10),
	MSM_DBA_CB_POST_RESET = BIT(11),
};


enum msm_dba_audio_interface_type {
	MSM_DBA_AUDIO_I2S_INTERFACE = BIT(0),
	MSM_DBA_AUDIO_SPDIF_INTERFACE = BIT(1),
};


enum msm_dba_audio_format_type {
	MSM_DBA_AUDIO_FMT_UNCOMPRESSED_LPCM = BIT(0),
	MSM_DBA_AUDIO_FMT_COMPRESSED = BIT(1),
};


enum msm_dba_audio_copyright_type {
	MSM_DBA_AUDIO_COPYRIGHT_PROTECTED = BIT(0),
	MSM_DBA_AUDIO_COPYRIGHT_NOT_PROTECTED = BIT(1),
};


enum msm_dba_audio_pre_emphasis_type {
	MSM_DBA_AUDIO_NO_PRE_EMPHASIS = BIT(0),
	MSM_DBA_AUDIO_PRE_EMPHASIS_50_15us = BIT(1),
};


enum msm_dba_audio_clock_accuracy {
	MSM_DBA_AUDIO_CLOCK_ACCURACY_LVL1 = BIT(1),
	MSM_DBA_AUDIO_CLOCK_ACCURACY_LVL2 = BIT(0),
	MSM_DBA_AUDIO_CLOCK_ACCURACY_LVL3 = BIT(2),
};


enum msm_dba_channel_status_source {
	MSM_DBA_AUDIO_CS_SOURCE_I2S_STREAM,
	MSM_DBA_AUDIO_CS_SOURCE_REGISTERS
};


enum msm_dba_audio_sampling_rates_type {
	MSM_DBA_AUDIO_32KHZ = BIT(0),
	MSM_DBA_AUDIO_44P1KHZ = BIT(1),
	MSM_DBA_AUDIO_48KHZ = BIT(2),
	MSM_DBA_AUDIO_88P2KHZ = BIT(1),
	MSM_DBA_AUDIO_96KHZ = BIT(3),
	MSM_DBA_AUDIO_176P4KHZ = BIT(1),
	MSM_DBA_AUDIO_192KHZ = BIT(4),
};


enum msm_dba_audio_word_bit_depth {
	MSM_DBA_AUDIO_WORD_16BIT = BIT(1),
	MSM_DBA_AUDIO_WORD_24BIT = BIT(2),
	MSM_DBA_AUDIO_WORD_32BIT = BIT(3),
};


enum msm_dba_audio_channel_count {
	MSM_DBA_AUDIO_CHANNEL_2 = BIT(0),
	MSM_DBA_AUDIO_CHANNEL_4 = BIT(1),
	MSM_DBA_AUDIO_CHANNEL_8 = BIT(2),
};


enum msm_dba_audio_i2s_format {
	MSM_DBA_AUDIO_I2S_FMT_STANDARD = 0,
	MSM_DBA_AUDIO_I2S_FMT_RIGHT_JUSTIFIED,
	MSM_DBA_AUDIO_I2S_FMT_LEFT_JUSTIFIED,
	MSM_DBA_AUDIO_I2S_FMT_AES3_DIRECT,
	MSM_DBA_AUDIO_I2S_FMT_MAX,
};

enum msm_dba_video_aspect_ratio {
	MSM_DBA_AR_UNKNOWN = 0,
	MSM_DBA_AR_4_3,
	MSM_DBA_AR_5_4,
	MSM_DBA_AR_16_9,
	MSM_DBA_AR_16_10,
	MSM_DBA_AR_64_27,
	MSM_DBA_AR_256_135,
	MSM_DBA_AR_MAX
};

enum msm_dba_audio_word_endian_type {
	MSM_DBA_AUDIO_WORD_LITTLE_ENDIAN = 0,
	MSM_DBA_AUDIO_WORD_BIG_ENDIAN,
	MSM_DBA_AUDIO_WORD_ENDIAN_MAX
};


enum msm_dba_audio_op_mode {
	MSM_DBA_AUDIO_MODE_MANUAL,
	MSM_DBA_AUDIO_MODE_AUTOMATIC,
};


typedef void (*msm_dba_cb)(void *data, enum msm_dba_callback_event event);


struct msm_dba_reg_info {
	char client_name[MSM_DBA_CLIENT_NAME_LEN];
	char chip_name[MSM_DBA_CHIP_NAME_MAX_LEN];
	u32 instance_id;
	msm_dba_cb cb;
	void *cb_data;
};


struct msm_dba_video_caps_info {
	bool hdcp_support;
	bool edid_support;
	bool data_lanes_lp_support;
	bool clock_lanes_lp_support;
	u32 max_pclk_khz;
	u32 num_of_input_lanes;
};


struct msm_dba_audio_caps_info {
	u32 audio_support;
	u32 audio_rates;
	u32 audio_fmts;
};


struct msm_dba_capabilities {
	struct msm_dba_video_caps_info vid_caps;
	struct msm_dba_audio_caps_info aud_caps;
	bool av_mute_support;
	bool deferred_commit_support;
};


struct msm_dba_audio_cfg {
	enum msm_dba_audio_interface_type interface;
	enum msm_dba_audio_format_type format;
	enum msm_dba_audio_channel_count channels;
	enum msm_dba_audio_i2s_format i2s_fmt;
	enum msm_dba_audio_sampling_rates_type sampling_rate;
	enum msm_dba_audio_word_bit_depth word_size;
	enum msm_dba_audio_word_endian_type word_endianness;
	enum msm_dba_audio_copyright_type copyright;
	enum msm_dba_audio_pre_emphasis_type pre_emphasis;
	enum msm_dba_audio_clock_accuracy clock_accuracy;
	enum msm_dba_channel_status_source channel_status_source;
	enum msm_dba_audio_op_mode mode;

	u32 channel_status_category_code;
	u32 channel_status_source_number;
	u32 channel_status_v_bit;
	u32 channel_allocation;
	u32 channel_status_word_length;

	u32 n;
	u32 cts;
};


struct msm_dba_video_cfg {
	u32  h_active;
	u32  h_front_porch;
	u32  h_pulse_width;
	u32  h_back_porch;
	bool h_polarity;
	u32  v_active;
	u32  v_front_porch;
	u32  v_pulse_width;
	u32  v_back_porch;
	bool v_polarity;
	u32  pclk_khz;
	bool interlaced;
	u32  vic;
	bool hdmi_mode;
	enum msm_dba_video_aspect_ratio ar;
	u32  num_of_input_lanes;
	u8 scaninfo;
};

struct mdss_dba_timing_info {
	u16 xres;
	u16 yres;
	u8 bpp;
	u8 fps;
	u8 lanes;
};


struct msm_dba_ops {
	int (*get_caps)(void *client,
			struct msm_dba_capabilities *caps);

	int (*power_on)(void *client,
			bool on,
			u32 flags);

	int (*video_on)(void *client,
			bool on,
			struct msm_dba_video_cfg *cfg,
			u32 flags);

	int (*audio_on)(void *client,
			bool on,
			u32 flags);

	int (*configure_audio)(void *client,
			       struct msm_dba_audio_cfg *cfg,
			       u32 flags);

	int (*av_mute)(void *client,
		       bool video_mute,
		       bool audio_mute,
		       u32 flags);

	int (*interrupts_enable)(void *client,
				bool on,
				u32 event_mask,
				u32 flags);

	int (*hdcp_enable)(void *client,
			   bool hdcp_on,
			   bool enc_on,
			   u32 flags);

	int (*hdcp_get_ksv_list_size)(void *client,
				      u32 *count,
				      u32 flags);

	int (*hdcp_get_ksv_list)(void *client,
				 u32 count,
				 char *buf,
				 u32 flags);

	int (*hdmi_cec_on)(void *client,
			      bool enable,
			      u32 flags);

	int (*hdmi_cec_write)(void *client,
			      u32 size,
			      char *buf,
			      u32 flags);

	int (*hdmi_cec_read)(void *client,
			     u32 *size,
			     char *buf,
			     u32 flags);

	int (*get_edid_size)(void *client,
			     u32 *size,
			     u32 flags);

	int (*get_raw_edid)(void *client,
			    u32 size,
			    char *buf,
			    u32 flags);

	int (*enable_remote_comm)(void *client,
				  bool on,
				  u32 flags);

	int (*add_remote_device)(void *client,
				 u32 *slave_ids,
				 u32 count,
				 u32 flags);

	int (*commit_deferred_props)(void *client,
				    u32 flags);

	int (*force_reset)(void *client, u32 flags);
	int (*dump_debug_info)(void *client, u32 flags);
	int (*check_hpd)(void *client, u32 flags);
	void (*set_audio_block)(void *client, u32 size, void *buf);
	void (*get_audio_block)(void *client, u32 size, void *buf);
	void* (*get_supp_timing_info)(void);
};


void *msm_dba_register_client(struct msm_dba_reg_info *info,
			      struct msm_dba_ops *ops);


int msm_dba_deregister_client(void *client);

#endif 
