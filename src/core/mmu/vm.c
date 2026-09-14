/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 */

#include <vm.h>

#include <config.h>
#include <mem.h>
#include <fences.h>
#include <page_table.h>

/**
 * The hypervisor page table entry covering the vm section (SEC_HYP_VM) of each vm. The vm's
 * master allocates the stage-2 page tables and other per-vm data in that section, and the vm's
 * remaining cpus copy the entry into their own root page table to share those mappings.
 */
static pte_t vm_section_pte[CONFIG_VM_NUM];

void vm_mem_prot_init(struct vm* vm, const struct vm_config* vm_config)
{
    as_init(&vm->as, AS_VM, NULL, vm_config->colors);

    vm_section_pte[vm->id] = *pt_get_pte(&cpu()->as.pt, VM_SHARED_PT_LVL, (vaddr_t)vm->as.pt.root);
    fence_ord_write();
}

void vm_mem_prot_cpu_init(struct vm* vm)
{
    /* The private blocks of all cpus are already reachable (percpu_init) */
    if (vm->master != cpu()->id) {
        pte_t* pte = pt_get_pte(&cpu()->as.pt, VM_SHARED_PT_LVL, (vaddr_t)vm->as.pt.root);
        *pte = vm_section_pte[vm->id];
        // We don't invalidate the TLB as we know there was no previous mapping or accesses to the
        // addresses in the VM section. Just make sure the write commited before leaving.
        fence_ord_write();
    }
}
