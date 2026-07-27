/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __MINIDUMP_H
#define __MINIDUMP_H

#define MAX_NAME_LENGTH		12


#define MINIDUMP_DEFAULT_ID	UINT_MAX


struct md_region {
	char	name[MAX_NAME_LENGTH];
	u32	id;
	u64	virt_addr;
	u64	phys_addr;
	u64	size;
};

#ifdef CONFIG_QCOM_MINIDUMP

extern int msm_minidump_add_region(const struct md_region *entry);
extern int msm_minidump_remove_region(const struct md_region *entry);

extern int msm_minidump_update_region(int regno, const struct md_region *entry);
extern bool msm_minidump_enabled(void);
extern void dump_stack_minidump(u64 sp);
#else
static inline int msm_minidump_add_region(const struct md_region *entry)
{
	
	return 0;
}
static inline int msm_minidump_remove_region(const struct md_region *entry)
{
	return 0;
}
static inline bool msm_minidump_enabled(void) { return false; }
static inline void dump_stack_minidump(u64 sp) {}
#endif

#ifdef CONFIG_QCOM_DYN_MINIDUMP_STACK

extern void update_md_current_stack(void *data);
#else
static inline void update_md_current_stack(void *data) {}
#endif
#endif
