/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __QTEE_SHMBRIDGE_H__
#define __QTEE_SHMBRIDGE_H__


#include <soc/qcom/secure_buffer.h>


struct qtee_shm {
	phys_addr_t paddr;
	void *vaddr;
	size_t size;
};


bool qtee_shmbridge_is_enabled(void);


int32_t qtee_shmbridge_query(phys_addr_t paddr);


int32_t qtee_shmbridge_register(
		phys_addr_t paddr,
		size_t size,
		uint32_t *ns_vmid_list,
		uint32_t *ns_vm_perm_list,
		uint32_t ns_vmid_num,
		uint32_t tz_perm,
		uint64_t *handle);


int32_t qtee_shmbridge_deregister(uint64_t handle);


int32_t qtee_shmbridge_allocate_shm(size_t size, struct qtee_shm *shm);


void qtee_shmbridge_free_shm(struct qtee_shm *shm);

#endif 
