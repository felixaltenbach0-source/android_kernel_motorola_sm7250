/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __SUBSYS_RESTART_H
#define __SUBSYS_RESTART_H

#include <linux/spinlock.h>
#include <linux/interrupt.h>

struct subsys_device;
extern struct bus_type subsys_bus_type;

enum {
	RESET_SOC = 0,
	RESET_SUBSYS_COUPLED,
	RESET_LEVEL_MAX
};

enum crash_status {
	CRASH_STATUS_NO_CRASH = 0,
	CRASH_STATUS_ERR_FATAL,
	CRASH_STATUS_WDOG_BITE,
};

struct device;
struct module;

enum ssr_comm {
	SUBSYS_TO_SUBSYS_SYSMON,
	SUBSYS_TO_HLOS,
	HLOS_TO_SUBSYS_SYSMON_SHUTDOWN,
	NUM_SSR_COMMS,
};


struct subsys_notif_timeout {
	enum ssr_comm comm_type;
	const char *dest_name;
	const char *source_name;
	struct timer_list timer;
};


struct subsys_desc {
	const char *name;
	char fw_name[256];
	const char *pon_depends_on;
	const char *poff_depends_on;
	struct device *dev;
	struct module *owner;

	int (*shutdown)(const struct subsys_desc *desc, bool force_stop);
	int (*powerup)(const struct subsys_desc *desc);
	void (*crash_shutdown)(const struct subsys_desc *desc);
	int (*ramdump)(int, const struct subsys_desc *desc);
	void (*free_memory)(const struct subsys_desc *desc);
	irqreturn_t (*err_fatal_handler)(int irq, void *dev_id);
	irqreturn_t (*stop_ack_handler)(int irq, void *dev_id);
	irqreturn_t (*shutdown_ack_handler)(int irq, void *dev_id);
	irqreturn_t (*ramdump_disable_handler)(int irq, void *dev_id);
	irqreturn_t (*wdog_bite_handler)(int irq, void *dev_id);
	irqreturn_t (*generic_handler)(int irq, void *dev_id);
	int is_not_loadable;
	int err_fatal_gpio;
	unsigned int err_fatal_irq;
	unsigned int err_ready_irq;
	unsigned int stop_ack_irq;
	unsigned int wdog_bite_irq;
	unsigned int generic_irq;
	int force_stop_bit;
	int ramdump_disable_irq;
	int shutdown_ack_irq;
	int ramdump_disable;
	bool no_auth;
	bool pil_mss_memsetup;
	int ssctl_instance_id;
	u32 sysmon_pid;
	int sysmon_shutdown_ret;
	bool system_debug;
	bool ignore_ssr_failure;
	const char *edge;
	struct qcom_smem_state *state;
#ifdef CONFIG_SETUP_SSR_NOTIF_TIMEOUTS
	struct subsys_notif_timeout timeout_data;
#endif 
};


struct notif_data {
	enum crash_status crashed;
	int enable_ramdump;
	int enable_mini_ramdumps;
	bool no_auth;
	struct platform_device *pdev;
};

#if defined(CONFIG_MSM_SUBSYSTEM_RESTART)

extern int subsys_get_restart_level(struct subsys_device *dev);
extern int subsystem_restart_dev(struct subsys_device *dev);
extern int subsystem_restart(const char *name);
extern int subsystem_crashed(const char *name);

extern void *subsystem_get(const char *name);
extern void *subsystem_get_with_fwname(const char *name, const char *fw_name);
extern int subsystem_set_fwname(const char *name, const char *fw_name);
extern void subsystem_put(void *subsystem);

extern struct subsys_device *subsys_register(struct subsys_desc *desc);
extern void subsys_unregister(struct subsys_device *dev);

extern void subsys_default_online(struct subsys_device *dev);
extern void subsys_set_crash_status(struct subsys_device *dev,
					enum crash_status crashed);
extern enum crash_status subsys_get_crash_status(struct subsys_device *dev);
void notify_proxy_vote(struct device *device);
void notify_proxy_unvote(struct device *device);
void notify_before_auth_and_reset(struct device *device);
void complete_err_ready(struct subsys_device *subsys);
void complete_shutdown_ack(struct subsys_device *subsys);
struct subsys_device *find_subsys_device(const char *str);
extern int wait_for_shutdown_ack(struct subsys_desc *desc);
#else

static inline int subsys_get_restart_level(struct subsys_device *dev)
{
	return 0;
}

static inline int subsystem_restart_dev(struct subsys_device *dev)
{
	return 0;
}

static inline int subsystem_restart(const char *name)
{
	return 0;
}

static inline int subsystem_crashed(const char *name)
{
	return 0;
}

static inline void *subsystem_get(const char *name)
{
	return NULL;
}

static inline void *subsystem_get_with_fwname(const char *name,
				const char *fw_name) {
	return NULL;
}

static inline int subsystem_set_fwname(const char *name,
				const char *fw_name) {
	return 0;
}

static inline void subsystem_put(void *subsystem) { }

static inline
struct subsys_device *subsys_register(struct subsys_desc *desc)
{
	return NULL;
}

static inline void subsys_unregister(struct subsys_device *dev) { }

static inline void subsys_default_online(struct subsys_device *dev) { }
static inline void subsys_set_crash_status(struct subsys_device *dev,
						enum crash_status crashed) { }
static inline
enum crash_status subsys_get_crash_status(struct subsys_device *dev)
{
	return false;
}
static inline void notify_proxy_vote(struct device *device) { }
static inline void notify_proxy_unvote(struct device *device) { }
static inline void notify_before_auth_and_reset(struct device *device) { }
static inline int wait_for_shutdown_ack(struct subsys_desc *desc)
{
	return -EOPNOTSUPP;
}
#endif 

#endif
