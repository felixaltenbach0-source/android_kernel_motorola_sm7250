/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MSM_FD__
#define __MSM_FD__

#include <uapi/media/msm_fd.h>
#include <linux/compat.h>

#ifdef CONFIG_COMPAT

struct msm_fd_result32 {
	__u32 frame_id;
	__u32 face_cnt;
	compat_uptr_t face_data;
};


#define VIDIOC_MSM_FD_GET_RESULT32 \
	_IOWR('V', BASE_VIDIOC_PRIVATE, struct msm_fd_result32)
#endif

#endif 
