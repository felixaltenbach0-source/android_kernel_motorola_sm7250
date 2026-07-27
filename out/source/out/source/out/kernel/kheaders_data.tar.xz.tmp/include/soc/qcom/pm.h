/* SPDX-License-Identifier: GPL-2.0-only */



#ifndef __ARCH_ARM_MACH_MSM_PM_H
#define __ARCH_ARM_MACH_MSM_PM_H

#include <linux/types.h>
#include <linux/cpuidle.h>
#include <dt-bindings/msm/pm.h>

struct latency_level {
	int affinity_level;
	int reset_level;
	const char *level_name;
};

#ifdef CONFIG_MSM_PM

s32 msm_cpuidle_get_deep_idle_latency(void);


int lpm_get_latency(struct latency_level *level, uint32_t *latency);

#else

static inline s32 msm_cpuidle_get_deep_idle_latency(void) { return 0; }

static inline int lpm_get_latency(struct latency_level *level,
						uint32_t *latency)
{
	return 0;
}
#endif

#endif  
