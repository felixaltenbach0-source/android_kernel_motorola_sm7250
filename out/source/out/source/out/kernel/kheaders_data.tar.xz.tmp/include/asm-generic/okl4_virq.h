

#ifndef __OKL4_VIRQ_H__
#define __OKL4_VIRQ_H__

#include <linux/irq.h>
#include <microvisor/microvisor.h>

static inline okl4_virq_flags_t okl4_get_virq_payload(unsigned int irq)
{
	struct irq_data *irqd = irq_get_irq_data(irq);

	if (WARN_ON_ONCE(!irqd))
		return 0;

	return _okl4_sys_interrupt_get_payload(irqd_to_hwirq(irqd)).payload;
}

#endif
