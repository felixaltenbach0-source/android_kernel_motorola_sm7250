/* SPDX-License-Identifier: GPL-2.0-only */



#ifndef __ARCH_ARM_MACH_MSM_EVENT_TIMER_H
#define __ARCH_ARM_MACH_MSM_EVENT_TIMER_H

#include <linux/hrtimer.h>

struct event_timer_info;

#ifdef CONFIG_MSM_EVENT_TIMER

struct event_timer_info *add_event_timer(uint32_t irq,
				void (*function)(void *), void *data);


void activate_event_timer(struct event_timer_info *event, ktime_t event_time);


void deactivate_event_timer(struct event_timer_info *event);


void destroy_event_timer(struct event_timer_info *event);


ktime_t get_next_event_time(int cpu);
#else
static inline void *add_event_timer(uint32_t irq, void (*function)(void *),
						void *data)
{
	return NULL;
}

static inline void activate_event_timer(void *event, ktime_t event_time) {}

static inline void  deactivate_event_timer(void *event) {}

static inline void destroy_event_timer(void *event) {}

static inline ktime_t get_next_event_time(int cpu)
{
	return ns_to_ktime(0);
}

#endif 
#endif 
