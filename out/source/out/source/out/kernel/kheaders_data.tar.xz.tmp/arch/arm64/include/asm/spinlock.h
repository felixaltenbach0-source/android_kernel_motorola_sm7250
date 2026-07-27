
#ifndef __ASM_SPINLOCK_H
#define __ASM_SPINLOCK_H

#include <asm/qrwlock.h>
#include <asm/qspinlock.h>


#define smp_mb__after_spinlock()	smp_mb()

#endif 
