// SPDX-License-Identifier: GPL-2.0+
// Copyright 2017 IBM Corp.
#ifndef _MISC_OCXL_H_
#define _MISC_OCXL_H_

#include <linux/pci.h>



#define OCXL_AFU_NAME_SZ      (24+1)  


struct ocxl_afu_config {
	u8 idx;
	int dvsec_afu_control_pos; 
	char name[OCXL_AFU_NAME_SZ];
	u8 version_major;
	u8 version_minor;
	u8 afuc_type;
	u8 afum_type;
	u8 profile;
	u8 global_mmio_bar;     
	u64 global_mmio_offset;
	u32 global_mmio_size;
	u8 pp_mmio_bar;         
	u64 pp_mmio_offset;
	u32 pp_mmio_stride;
	u8 log_mem_size;
	u8 pasid_supported_log;
	u16 actag_supported;
};

struct ocxl_fn_config {
	int dvsec_tl_pos;       
	int dvsec_function_pos; 
	int dvsec_afu_info_pos; 
	s8 max_pasid_log;
	s8 max_afu_index;
};


extern int ocxl_config_read_function(struct pci_dev *dev,
				struct ocxl_fn_config *fn);


extern int ocxl_config_check_afu_index(struct pci_dev *dev,
				struct ocxl_fn_config *fn, int afu_idx);


extern int ocxl_config_read_afu(struct pci_dev *dev,
				struct ocxl_fn_config *fn,
				struct ocxl_afu_config *afu,
				u8 afu_idx);


extern int ocxl_config_get_pasid_info(struct pci_dev *dev, int *count);


extern void ocxl_config_set_afu_pasid(struct pci_dev *dev,
				int afu_control_offset,
				int pasid_base, u32 pasid_count_log);


extern int ocxl_config_get_actag_info(struct pci_dev *dev,
				u16 *base, u16 *enabled, u16 *supported);


extern void ocxl_config_set_actag(struct pci_dev *dev, int func_offset,
				u32 actag_base, u32 actag_count);


extern void ocxl_config_set_afu_actag(struct pci_dev *dev,
				int afu_control_offset,
				int actag_base, int actag_count);


extern void ocxl_config_set_afu_state(struct pci_dev *dev,
				int afu_control_offset, int enable);


extern int ocxl_config_set_TL(struct pci_dev *dev, int tl_dvsec);


extern int ocxl_config_terminate_pasid(struct pci_dev *dev,
				int afu_control_offset, int pasid);


extern int ocxl_link_setup(struct pci_dev *dev, int PE_mask,
			void **link_handle);


extern void ocxl_link_release(struct pci_dev *dev, void *link_handle);


extern int ocxl_link_add_pe(void *link_handle, int pasid, u32 pidr, u32 tidr,
		u64 amr, struct mm_struct *mm,
		void (*xsl_err_cb)(void *data, u64 addr, u64 dsisr),
		void *xsl_err_data);


extern int ocxl_link_update_pe(void *link_handle, int pasid, __u16 tid);


extern int ocxl_link_remove_pe(void *link_handle, int pasid);


extern int ocxl_link_irq_alloc(void *link_handle, int *hw_irq,
			u64 *obj_handle);


extern void ocxl_link_free_irq(void *link_handle, int hw_irq);

#endif 
