
#ifndef __ASM_DEVICE_H
#define __ASM_DEVICE_H

struct dev_archdata {
#ifdef CONFIG_IOMMU_API
	void *iommu;			
#endif
#ifdef CONFIG_XEN
	const struct dma_map_ops *dev_dma_ops;
#endif
	bool dma_coherent;
#ifdef CONFIG_ARM64_DMA_USE_IOMMU
	struct dma_iommu_mapping       *mapping;
#endif
};

struct pdev_archdata {
};

#ifdef CONFIG_ARM64_DMA_USE_IOMMU
#define to_dma_iommu_mapping(dev) ((dev)->archdata.mapping)
#else
#define to_dma_iommu_mapping(dev) NULL
#endif

#endif
