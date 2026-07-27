

#ifndef __ASM_ARM_KVM_ARCH_TIMER_H
#define __ASM_ARM_KVM_ARCH_TIMER_H

#include <linux/clocksource.h>
#include <linux/hrtimer.h>
#include <linux/workqueue.h>

struct arch_timer_context {
	
	u32				cnt_ctl;
	u64				cnt_cval;

	
	struct kvm_irq_level		irq;

	
	bool			loaded;

	
	u64			cntvoff;
};

struct arch_timer_cpu {
	struct arch_timer_context	vtimer;
	struct arch_timer_context	ptimer;

	
	struct hrtimer			bg_timer;

	
	struct work_struct		expired;

	
	struct hrtimer			phys_timer;

	
	bool			enabled;
};

int kvm_timer_hyp_init(bool);
int kvm_timer_enable(struct kvm_vcpu *vcpu);
int kvm_timer_vcpu_reset(struct kvm_vcpu *vcpu);
void kvm_timer_vcpu_init(struct kvm_vcpu *vcpu);
void kvm_timer_sync_hwstate(struct kvm_vcpu *vcpu);
bool kvm_timer_should_notify_user(struct kvm_vcpu *vcpu);
void kvm_timer_update_run(struct kvm_vcpu *vcpu);
void kvm_timer_vcpu_terminate(struct kvm_vcpu *vcpu);

u64 kvm_arm_timer_get_reg(struct kvm_vcpu *, u64 regid);
int kvm_arm_timer_set_reg(struct kvm_vcpu *, u64 regid, u64 value);

int kvm_arm_timer_set_attr(struct kvm_vcpu *vcpu, struct kvm_device_attr *attr);
int kvm_arm_timer_get_attr(struct kvm_vcpu *vcpu, struct kvm_device_attr *attr);
int kvm_arm_timer_has_attr(struct kvm_vcpu *vcpu, struct kvm_device_attr *attr);

bool kvm_timer_is_pending(struct kvm_vcpu *vcpu);

void kvm_timer_schedule(struct kvm_vcpu *vcpu);
void kvm_timer_unschedule(struct kvm_vcpu *vcpu);

u64 kvm_phys_timer_read(void);

void kvm_timer_vcpu_load(struct kvm_vcpu *vcpu);
void kvm_timer_vcpu_put(struct kvm_vcpu *vcpu);

void kvm_timer_init_vhe(void);

bool kvm_arch_timer_get_input_level(int vintid);

#define vcpu_vtimer(v)	(&(v)->arch.timer_cpu.vtimer)
#define vcpu_ptimer(v)	(&(v)->arch.timer_cpu.ptimer)

#endif
