/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __ARCH_ARM_MACH_MSM_RPM_NOTIF_H
#define __ARCH_ARM_MACH_MSM_RPM_NOTIF_H

struct msm_rpm_notifier_data {
	uint32_t rsc_type;
	uint32_t rsc_id;
	uint32_t key;
	uint32_t size;
	uint8_t *value;
};

int msm_rpm_register_notifier(struct notifier_block *nb);


int msm_rpm_unregister_notifier(struct notifier_block *nb);


int msm_rpm_enter_sleep(bool print, const struct cpumask *cpumask);


void msm_rpm_exit_sleep(void);


bool msm_rpm_waiting_for_ack(void);

#endif 
