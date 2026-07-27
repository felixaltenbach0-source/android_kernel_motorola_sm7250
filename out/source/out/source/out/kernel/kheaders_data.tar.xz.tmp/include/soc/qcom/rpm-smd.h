/* SPDX-License-Identifier: GPL-2.0-only */


#include <linux/platform_device.h>

#ifndef __ARCH_ARM_MACH_MSM_RPM_SMD_H
#define __ARCH_ARM_MACH_MSM_RPM_SMD_H


enum msm_rpm_set {
	MSM_RPM_CTX_ACTIVE_SET,
	MSM_RPM_CTX_SLEEP_SET,
};

struct msm_rpm_request;

struct msm_rpm_kvp {
	uint32_t key;
	uint32_t length;
	uint8_t *data;
};
#ifdef CONFIG_MSM_RPM_SMD

struct msm_rpm_request *msm_rpm_create_request(
		enum msm_rpm_set set, uint32_t rsc_type,
		uint32_t rsc_id, int num_elements);


struct msm_rpm_request *msm_rpm_create_request_noirq(
		enum msm_rpm_set set, uint32_t rsc_type,
		uint32_t rsc_id, int num_elements);


int msm_rpm_add_kvp_data(struct msm_rpm_request *handle,
		uint32_t key, const uint8_t *data, int size);


int msm_rpm_add_kvp_data_noirq(struct msm_rpm_request *handle,
		uint32_t key, const uint8_t *data, int size);



void msm_rpm_free_request(struct msm_rpm_request *handle);


int msm_rpm_send_request(struct msm_rpm_request *handle);


void *msm_rpm_send_request_noack(struct msm_rpm_request *handle);


int msm_rpm_send_request_noirq(struct msm_rpm_request *handle);


int msm_rpm_wait_for_ack(uint32_t msg_id);


int msm_rpm_wait_for_ack_noirq(uint32_t msg_id);


int msm_rpm_send_message(enum msm_rpm_set set, uint32_t rsc_type,
		uint32_t rsc_id, struct msm_rpm_kvp *kvp, int nelems);


void *msm_rpm_send_message_noack(enum msm_rpm_set set, uint32_t rsc_type,
		uint32_t rsc_id, struct msm_rpm_kvp *kvp, int nelems);


int msm_rpm_send_message_noirq(enum msm_rpm_set set, uint32_t rsc_type,
		uint32_t rsc_id, struct msm_rpm_kvp *kvp, int nelems);


int __init msm_rpm_driver_init(void);

#else

static inline struct msm_rpm_request *msm_rpm_create_request(
		enum msm_rpm_set set, uint32_t rsc_type,
		uint32_t rsc_id, int num_elements)
{
	return NULL;
}

static inline struct msm_rpm_request *msm_rpm_create_request_noirq(
		enum msm_rpm_set set, uint32_t rsc_type,
		uint32_t rsc_id, int num_elements)
{
	return NULL;

}
static inline uint32_t msm_rpm_add_kvp_data(struct msm_rpm_request *handle,
		uint32_t key, const uint8_t *data, int count)
{
	return 0;
}
static inline uint32_t msm_rpm_add_kvp_data_noirq(
		struct msm_rpm_request *handle, uint32_t key,
		const uint8_t *data, int count)
{
	return 0;
}

static inline void msm_rpm_free_request(struct msm_rpm_request *handle)
{
}

static inline int msm_rpm_send_request(struct msm_rpm_request *handle)
{
	return 0;
}

static inline int msm_rpm_send_request_noirq(struct msm_rpm_request *handle)
{
	return 0;

}

static inline void *msm_rpm_send_request_noack(struct msm_rpm_request *handle)
{
	return NULL;
}

static inline int msm_rpm_send_message(enum msm_rpm_set set, uint32_t rsc_type,
		uint32_t rsc_id, struct msm_rpm_kvp *kvp, int nelems)
{
	return 0;
}

static inline int msm_rpm_send_message_noirq(enum msm_rpm_set set,
		uint32_t rsc_type, uint32_t rsc_id, struct msm_rpm_kvp *kvp,
		int nelems)
{
	return 0;
}

static inline void *msm_rpm_send_message_noack(enum msm_rpm_set set,
		uint32_t rsc_type, uint32_t rsc_id, struct msm_rpm_kvp *kvp,
		int nelems)
{
	return NULL;
}

static inline int msm_rpm_wait_for_ack(uint32_t msg_id)
{
	return 0;

}
static inline int msm_rpm_wait_for_ack_noirq(uint32_t msg_id)
{
	return 0;
}

static inline int __init msm_rpm_driver_init(void)
{
	return 0;
}
#endif

#endif 
