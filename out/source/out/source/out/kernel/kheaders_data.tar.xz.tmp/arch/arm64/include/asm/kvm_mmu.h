

#ifndef __ARM64_KVM_MMU_H__
#define __ARM64_KVM_MMU_H__

#include <asm/page.h>
#include <asm/memory.h>
#include <asm/mmu_context.h>
#include <asm/cpufeature.h>



#ifdef __ASSEMBLY__

#include <asm/alternative.h>


.macro kern_hyp_va	reg
alternative_cb kvm_update_va_mask
	and     \reg, \reg, #1		
	ror	\reg, \reg, #1		
	add	\reg, \reg, #0		
	add	\reg, \reg, #0, lsl 12	
	ror	\reg, \reg, #63		
alternative_cb_end
.endm

#else

#include <asm/pgalloc.h>
#include <asm/cache.h>
#include <asm/cacheflush.h>
#include <asm/mmu_context.h>
#include <asm/pgtable.h>

void kvm_update_va_mask(struct alt_instr *alt,
			__le32 *origptr, __le32 *updptr, int nr_inst);

static inline unsigned long __kern_hyp_va(unsigned long v)
{
	asm volatile(ALTERNATIVE_CB("and %0, %0, #1\n"
				    "ror %0, %0, #1\n"
				    "add %0, %0, #0\n"
				    "add %0, %0, #0, lsl 12\n"
				    "ror %0, %0, #63\n",
				    kvm_update_va_mask)
		     : "+r" (v));
	return v;
}

#define kern_hyp_va(v) 	((typeof(v))(__kern_hyp_va((unsigned long)(v))))


#define hyp_symbol_addr(s)						\
	({								\
		typeof(s) *addr;					\
		asm("adrp	%0, %1\n"				\
		    "add	%0, %0, :lo12:%1\n"			\
		    : "=r" (addr) : "S" (&s));				\
		addr;							\
	})


#define KVM_PHYS_SHIFT	(40)
#define KVM_PHYS_SIZE	(1UL << KVM_PHYS_SHIFT)
#define KVM_PHYS_MASK	(KVM_PHYS_SIZE - 1UL)

#include <asm/stage2_pgtable.h>

int create_hyp_mappings(void *from, void *to, pgprot_t prot);
int create_hyp_io_mappings(phys_addr_t phys_addr, size_t size,
			   void __iomem **kaddr,
			   void __iomem **haddr);
int create_hyp_exec_mappings(phys_addr_t phys_addr, size_t size,
			     void **haddr);
void free_hyp_pgds(void);

void stage2_unmap_vm(struct kvm *kvm);
int kvm_alloc_stage2_pgd(struct kvm *kvm);
void kvm_free_stage2_pgd(struct kvm *kvm);
int kvm_phys_addr_ioremap(struct kvm *kvm, phys_addr_t guest_ipa,
			  phys_addr_t pa, unsigned long size, bool writable);

int kvm_handle_guest_abort(struct kvm_vcpu *vcpu, struct kvm_run *run);

void kvm_mmu_free_memory_caches(struct kvm_vcpu *vcpu);

phys_addr_t kvm_mmu_get_httbr(void);
phys_addr_t kvm_get_idmap_vector(void);
int kvm_mmu_init(void);
void kvm_clear_hyp_idmap(void);

#define kvm_mk_pmd(ptep)					\
	__pmd(__phys_to_pmd_val(__pa(ptep)) | PMD_TYPE_TABLE)
#define kvm_mk_pud(pmdp)					\
	__pud(__phys_to_pud_val(__pa(pmdp)) | PMD_TYPE_TABLE)
#define kvm_mk_pgd(pudp)					\
	__pgd(__phys_to_pgd_val(__pa(pudp)) | PUD_TYPE_TABLE)

static inline pte_t kvm_s2pte_mkwrite(pte_t pte)
{
	pte_val(pte) |= PTE_S2_RDWR;
	return pte;
}

static inline pmd_t kvm_s2pmd_mkwrite(pmd_t pmd)
{
	pmd_val(pmd) |= PMD_S2_RDWR;
	return pmd;
}

static inline pte_t kvm_s2pte_mkexec(pte_t pte)
{
	pte_val(pte) &= ~PTE_S2_XN;
	return pte;
}

static inline pmd_t kvm_s2pmd_mkexec(pmd_t pmd)
{
	pmd_val(pmd) &= ~PMD_S2_XN;
	return pmd;
}

static inline void kvm_set_s2pte_readonly(pte_t *ptep)
{
	pteval_t old_pteval, pteval;

	pteval = READ_ONCE(pte_val(*ptep));
	do {
		old_pteval = pteval;
		pteval &= ~PTE_S2_RDWR;
		pteval |= PTE_S2_RDONLY;
		pteval = cmpxchg_relaxed(&pte_val(*ptep), old_pteval, pteval);
	} while (pteval != old_pteval);
}

static inline bool kvm_s2pte_readonly(pte_t *ptep)
{
	return (READ_ONCE(pte_val(*ptep)) & PTE_S2_RDWR) == PTE_S2_RDONLY;
}

static inline bool kvm_s2pte_exec(pte_t *ptep)
{
	return !(READ_ONCE(pte_val(*ptep)) & PTE_S2_XN);
}

static inline void kvm_set_s2pmd_readonly(pmd_t *pmdp)
{
	kvm_set_s2pte_readonly((pte_t *)pmdp);
}

static inline bool kvm_s2pmd_readonly(pmd_t *pmdp)
{
	return kvm_s2pte_readonly((pte_t *)pmdp);
}

static inline bool kvm_s2pmd_exec(pmd_t *pmdp)
{
	return !(READ_ONCE(pmd_val(*pmdp)) & PMD_S2_XN);
}

static inline bool kvm_page_empty(void *ptr)
{
	struct page *ptr_page = virt_to_page(ptr);
	return page_count(ptr_page) == 1;
}

#define hyp_pte_table_empty(ptep) kvm_page_empty(ptep)

#ifdef __PAGETABLE_PMD_FOLDED
#define hyp_pmd_table_empty(pmdp) (0)
#else
#define hyp_pmd_table_empty(pmdp) kvm_page_empty(pmdp)
#endif

#ifdef __PAGETABLE_PUD_FOLDED
#define hyp_pud_table_empty(pudp) (0)
#else
#define hyp_pud_table_empty(pudp) kvm_page_empty(pudp)
#endif

struct kvm;

#define kvm_flush_dcache_to_poc(a,l)	__flush_dcache_area((a), (l))

static inline bool vcpu_has_cache_enabled(struct kvm_vcpu *vcpu)
{
	return (vcpu_read_sys_reg(vcpu, SCTLR_EL1) & 0b101) == 0b101;
}

static inline void __clean_dcache_guest_page(kvm_pfn_t pfn, unsigned long size)
{
	void *va = page_address(pfn_to_page(pfn));

	
	if (cpus_have_const_cap(ARM64_HAS_STAGE2_FWB))
		return;

	kvm_flush_dcache_to_poc(va, size);
}

static inline void __invalidate_icache_guest_page(kvm_pfn_t pfn,
						  unsigned long size)
{
	if (icache_is_aliasing()) {
		
		__flush_icache_all();
	} else if (is_kernel_in_hyp_mode() || !icache_is_vpipt()) {
		
		void *va = page_address(pfn_to_page(pfn));

		invalidate_icache_range((unsigned long)va,
					(unsigned long)va + size);
	}
}

static inline void __kvm_flush_dcache_pte(pte_t pte)
{
	if (!cpus_have_const_cap(ARM64_HAS_STAGE2_FWB)) {
		struct page *page = pte_page(pte);
		kvm_flush_dcache_to_poc(page_address(page), PAGE_SIZE);
	}
}

static inline void __kvm_flush_dcache_pmd(pmd_t pmd)
{
	if (!cpus_have_const_cap(ARM64_HAS_STAGE2_FWB)) {
		struct page *page = pmd_page(pmd);
		kvm_flush_dcache_to_poc(page_address(page), PMD_SIZE);
	}
}

static inline void __kvm_flush_dcache_pud(pud_t pud)
{
	if (!cpus_have_const_cap(ARM64_HAS_STAGE2_FWB)) {
		struct page *page = pud_page(pud);
		kvm_flush_dcache_to_poc(page_address(page), PUD_SIZE);
	}
}

#define kvm_virt_to_phys(x)		__pa_symbol(x)

void kvm_set_way_flush(struct kvm_vcpu *vcpu);
void kvm_toggle_cache(struct kvm_vcpu *vcpu, bool was_enabled);

static inline bool __kvm_cpu_uses_extended_idmap(void)
{
	return __cpu_uses_extended_idmap_level();
}

static inline unsigned long __kvm_idmap_ptrs_per_pgd(void)
{
	return idmap_ptrs_per_pgd;
}


static inline void __kvm_extend_hypmap(pgd_t *boot_hyp_pgd,
				       pgd_t *hyp_pgd,
				       pgd_t *merged_hyp_pgd,
				       unsigned long hyp_idmap_start)
{
	int idmap_idx;
	u64 pgd_addr;

	
	VM_BUG_ON(pgd_val(merged_hyp_pgd[0]));
	pgd_addr = __phys_to_pgd_val(__pa(hyp_pgd));
	merged_hyp_pgd[0] = __pgd(pgd_addr | PMD_TYPE_TABLE);

	
	idmap_idx = hyp_idmap_start >> VA_BITS;
	VM_BUG_ON(pgd_val(merged_hyp_pgd[idmap_idx]));
	pgd_addr = __phys_to_pgd_val(__pa(boot_hyp_pgd));
	merged_hyp_pgd[idmap_idx] = __pgd(pgd_addr | PMD_TYPE_TABLE);
}

static inline unsigned int kvm_get_vmid_bits(void)
{
	int reg = read_sanitised_ftr_reg(SYS_ID_AA64MMFR1_EL1);

	return (cpuid_feature_extract_unsigned_field(reg, ID_AA64MMFR1_VMIDBITS_SHIFT) == 2) ? 16 : 8;
}


static inline int kvm_read_guest_lock(struct kvm *kvm,
				      gpa_t gpa, void *data, unsigned long len)
{
	int srcu_idx = srcu_read_lock(&kvm->srcu);
	int ret = kvm_read_guest(kvm, gpa, data, len);

	srcu_read_unlock(&kvm->srcu, srcu_idx);

	return ret;
}

static inline int kvm_write_guest_lock(struct kvm *kvm, gpa_t gpa,
				       const void *data, unsigned long len)
{
	int srcu_idx = srcu_read_lock(&kvm->srcu);
	int ret = kvm_write_guest(kvm, gpa, data, len);

	srcu_read_unlock(&kvm->srcu, srcu_idx);

	return ret;
}

#ifdef CONFIG_KVM_INDIRECT_VECTORS

#include <asm/mmu.h>

extern void *__kvm_bp_vect_base;
extern int __kvm_harden_el2_vector_slot;

static inline void *kvm_get_hyp_vector(void)
{
	struct bp_hardening_data *data = arm64_get_bp_hardening_data();
	void *vect = kern_hyp_va(kvm_ksym_ref(__kvm_hyp_vector));
	int slot = -1;

	if (cpus_have_const_cap(ARM64_HARDEN_BRANCH_PREDICTOR) && data->fn) {
		vect = kern_hyp_va(kvm_ksym_ref(__bp_harden_hyp_vecs_start));
		slot = data->hyp_vectors_slot;
	}

	if (this_cpu_has_cap(ARM64_HARDEN_EL2_VECTORS) && !has_vhe()) {
		vect = __kvm_bp_vect_base;
		if (slot == -1)
			slot = __kvm_harden_el2_vector_slot;
	}

	if (slot != -1)
		vect += slot * SZ_2K;

	return vect;
}


static inline int kvm_map_vectors(void)
{
	
	if (cpus_have_const_cap(ARM64_HARDEN_BRANCH_PREDICTOR)) {
		__kvm_bp_vect_base = kvm_ksym_ref(__bp_harden_hyp_vecs_start);
		__kvm_bp_vect_base = kern_hyp_va(__kvm_bp_vect_base);
	}

	if (cpus_have_const_cap(ARM64_HARDEN_EL2_VECTORS)) {
		phys_addr_t vect_pa = __pa_symbol(__bp_harden_hyp_vecs_start);
		unsigned long size = (__bp_harden_hyp_vecs_end -
				      __bp_harden_hyp_vecs_start);

		
		__kvm_harden_el2_vector_slot = atomic_inc_return(&arm64_el2_vector_last_slot);
		BUG_ON(__kvm_harden_el2_vector_slot >= BP_HARDEN_EL2_SLOTS);
		return create_hyp_exec_mappings(vect_pa, size,
						&__kvm_bp_vect_base);
	}

	return 0;
}
#else
static inline void *kvm_get_hyp_vector(void)
{
	return kern_hyp_va(kvm_ksym_ref(__kvm_hyp_vector));
}

static inline int kvm_map_vectors(void)
{
	return 0;
}
#endif

#ifdef CONFIG_ARM64_SSBD
DECLARE_PER_CPU_READ_MOSTLY(u64, arm64_ssbd_callback_required);

static inline int hyp_map_aux_data(void)
{
	int cpu, err;

	for_each_possible_cpu(cpu) {
		u64 *ptr;

		ptr = per_cpu_ptr(&arm64_ssbd_callback_required, cpu);
		err = create_hyp_mappings(ptr, ptr + 1, PAGE_HYP);
		if (err)
			return err;
	}
	return 0;
}
#else
static inline int hyp_map_aux_data(void)
{
	return 0;
}
#endif

#define kvm_phys_to_vttbr(addr)		phys_to_ttbr(addr)

static inline void kvm_workaround_1542418_vmid_rollover(void)
{
	unsigned long flags;

	if (!IS_ENABLED(CONFIG_ARM64_ERRATUM_1542418) ||
	    !cpus_have_const_cap(ARM64_WORKAROUND_1542418))
		return;

	local_irq_save(flags);
	arm64_workaround_1542418_asid_rollover();
	local_irq_restore(flags);

}

#endif 
#endif 
