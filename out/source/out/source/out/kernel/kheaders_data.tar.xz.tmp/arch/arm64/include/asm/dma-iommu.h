/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef ASMARM_DMA_IOMMU_H
#define ASMARM_DMA_IOMMU_H

#ifdef __KERNEL__

#include <linux/err.h>
#include <linux/mm_types.h>
#include <linux/scatterlist.h>
#include <linux/dma-debug.h>
#include <linux/kref.h>
#include <linux/dma-mapping-fast.h>

struct dma_iommu_mapping {
	
	struct iommu_domain	*domain;
	bool			init;
	struct kref		kref;
	const struct dma_map_ops *ops;

	
	spinlock_t		lock;
	void			*bitmap;
	size_t			bits;
	dma_addr_t		base;

	struct dma_fast_smmu_mapping *fast;
};

#ifdef CONFIG_ARM64_DMA_USE_IOMMU
void arm_iommu_put_dma_cookie(struct iommu_domain *domain);
#else  
static inline void arm_iommu_put_dma_cookie(struct iommu_domain *domain) {}
#endif	

#endif 
#endif
