/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _MSM_CVP_VIDC_H_
#define _MSM_CVP_VIDC_H_
#include <uapi/media/msm_cvp_private.h>


struct cvp_kmd_usecase_desc {
	unsigned int fullres_width;
	unsigned int fullres_height;
	unsigned int downscale_width;
	unsigned int downscale_height;
	unsigned int is_downscale;
	unsigned int fps;
	unsigned int op_rate;
	unsigned int colorfmt;
	int reserved[16];
};

#define VIDEO_NONREALTIME 1
#define VIDEO_REALTIME 5

#ifdef CONFIG_MSM_CVP_V4L2
void *msm_cvp_open(int core_id, int session_type);
int msm_cvp_close(void *instance);
int msm_cvp_private(void *cvp_inst, unsigned int cmd, struct cvp_kmd_arg *arg);
int msm_cvp_est_cycles(struct cvp_kmd_usecase_desc *cvp_desc,
	struct cvp_kmd_request_power *cvp_voting);

#else
static inline void *msm_cvp_open(int core_id, int session_type)
{
	return NULL;
}
static inline int msm_cvp_close(void *instance)
{
	return -EINVAL;
}
static inline int msm_cvp_private(void *cvp_inst, unsigned int cmd,
		struct cvp_kmd_arg *arg)
{
	return -EINVAL;
}
static inline int msm_cvp_est_cycles(struct cvp_kmd_usecase_desc *cvp_desc,
	struct cvp_kmd_request_power *cvp_voting)
{
	return -EINVAL;
}
#endif

#endif

