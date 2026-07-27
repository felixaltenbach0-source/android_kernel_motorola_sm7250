/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef ASM_EDAC_H
#define ASM_EDAC_H

#if defined(CONFIG_EDAC_CORTEX_ARM64) && \
	!defined(CONFIG_EDAC_CORTEX_ARM64_DBE_IRQ_ONLY)
void arm64_check_cache_ecc(void *info);
#else
static inline void arm64_check_cache_ecc(void *info) { }
#endif

static inline void atomic_scrub(void *addr, int size) { }

#endif
