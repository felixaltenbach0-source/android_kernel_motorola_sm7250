/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_GENERIC_DMA_MAPPING_H
#define _ASM_GENERIC_DMA_MAPPING_H

static inline const struct dma_map_ops *get_arch_dma_ops(struct bus_type *bus)
{
	
#ifdef CONFIG_DMA_NONCOHERENT_OPS
	return &dma_noncoherent_ops;
#else
	return &dma_direct_ops;
#endif
}

#endif 
