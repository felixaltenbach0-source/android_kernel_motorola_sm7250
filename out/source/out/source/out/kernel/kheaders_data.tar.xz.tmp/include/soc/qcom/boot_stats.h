/* SPDX-License-Identifier: GPL-2.0-only */


#ifdef CONFIG_MSM_BOOT_STATS
int boot_stats_init(void);
#else
static inline int boot_stats_init(void) { return 0; }
#endif
