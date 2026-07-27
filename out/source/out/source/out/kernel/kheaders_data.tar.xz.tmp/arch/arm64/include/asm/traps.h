
#ifndef __ASM_TRAP_H
#define __ASM_TRAP_H

#include <linux/list.h>
#include <asm/esr.h>
#include <asm/sections.h>

struct pt_regs;

struct undef_hook {
	struct list_head node;
	u32 instr_mask;
	u32 instr_val;
	u64 pstate_mask;
	u64 pstate_val;
	int (*fn)(struct pt_regs *regs, u32 instr);
};

void register_undef_hook(struct undef_hook *hook);
void unregister_undef_hook(struct undef_hook *hook);
void force_signal_inject(int signal, int code, unsigned long address);
void arm64_notify_segfault(unsigned long addr);
void arm64_force_sig_info(struct siginfo *info, const char *str,
			  struct task_struct *tsk);


void arm64_skip_faulting_instruction(struct pt_regs *regs, unsigned long size);

static inline int __in_irqentry_text(unsigned long ptr)
{
	return ptr >= (unsigned long)&__irqentry_text_start &&
	       ptr < (unsigned long)&__irqentry_text_end;
}

static inline int in_exception_text(unsigned long ptr)
{
	int in;

	in = ptr >= (unsigned long)&__exception_text_start &&
	     ptr < (unsigned long)&__exception_text_end;

	return in ? : __in_irqentry_text(ptr);
}

static inline int in_entry_text(unsigned long ptr)
{
	return ptr >= (unsigned long)&__entry_text_start &&
	       ptr < (unsigned long)&__entry_text_end;
}


static inline bool arm64_is_ras_serror(u32 esr)
{
	WARN_ON(preemptible());

	if (esr & ESR_ELx_IDS)
		return false;

	if (this_cpu_has_cap(ARM64_HAS_RAS_EXTN))
		return true;
	else
		return false;
}


static inline u32 arm64_ras_serror_get_severity(u32 esr)
{
	u32 aet = esr & ESR_ELx_AET;

	if (!arm64_is_ras_serror(esr)) {
		
		return ESR_ELx_AET_UC;
	}

	
	if ((esr & ESR_ELx_FSC) != ESR_ELx_FSC_SERROR) {
		
		return ESR_ELx_AET_UC;
	}

	return aet;
}

bool arm64_is_fatal_ras_serror(struct pt_regs *regs, unsigned int esr);
void __noreturn arm64_serror_panic(struct pt_regs *regs, u32 esr);
static inline void get_timer_count_hook_init(void) {}
static inline void get_timer_freq_hook_init(void) {}
#endif
