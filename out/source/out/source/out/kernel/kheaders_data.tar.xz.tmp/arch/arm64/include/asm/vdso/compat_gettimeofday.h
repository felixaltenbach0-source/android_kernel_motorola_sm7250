/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __ASM_VDSO_GETTIMEOFDAY_H
#define __ASM_VDSO_GETTIMEOFDAY_H

#ifndef __ASSEMBLY__

#include <asm/unistd.h>
#include <asm/errno.h>

#include <asm/vdso/compat_barrier.h>

#define __VDSO_USE_SYSCALL		ULLONG_MAX

#define VDSO_HAS_CLOCK_GETRES		1

#define VDSO_HAS_TIME			1

#define BUILD_VDSO32			1

static __always_inline
int gettimeofday_fallback(struct __kernel_old_timeval *_tv,
			  struct timezone *_tz)
{
	register struct timezone *tz asm("r1") = _tz;
	register struct __kernel_old_timeval *tv asm("r0") = _tv;
	register long ret asm ("r0");
	register long nr asm("r7") = __NR_compat_gettimeofday;

	asm volatile(
	"	swi #0\n"
	: "=r" (ret)
	: "r" (tv), "r" (tz), "r" (nr)
	: "memory");

	return ret;
}

static __always_inline
long clock_gettime_fallback(clockid_t _clkid, struct __kernel_timespec *_ts)
{
	register struct __kernel_timespec *ts asm("r1") = _ts;
	register clockid_t clkid asm("r0") = _clkid;
	register long ret asm ("r0");
	register long nr asm("r7") = __NR_compat_clock_gettime;

	asm volatile(
	"	swi #0\n"
	: "=r" (ret)
	: "r" (clkid), "r" (ts), "r" (nr)
	: "memory");

	return ret;
}

static __always_inline
int clock_getres_fallback(clockid_t _clkid, struct __kernel_timespec *_ts)
{
	register struct __kernel_timespec *ts asm("r1") = _ts;
	register clockid_t clkid asm("r0") = _clkid;
	register long ret asm ("r0");
	register long nr asm("r7") = __NR_compat_clock_getres;

	asm volatile(
	"       swi #0\n"
	: "=r" (ret)
	: "r" (clkid), "r" (ts), "r" (nr)
	: "memory");

	return ret;
}

static __always_inline u64 __arch_get_hw_counter(s32 clock_mode)
{
	u64 res;

	
	if (clock_mode)
		return __VDSO_USE_SYSCALL;

	
	isb();
	asm volatile("mrrc p15, 1, %Q0, %R0, c14" : "=r" (res));
	
	isb();

	return res;
}

static __always_inline const struct vdso_data *__arch_get_vdso_data(void)
{
	const struct vdso_data *ret;

	
	asm volatile("mov %0, %1" : "=r"(ret) : "r"(_vdso_data));

	return ret;
}

#endif 

#endif 
