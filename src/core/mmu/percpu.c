/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 */

#include <percpu.h>
#include <mem.h>
#include <platform.h>

/**
 * Every cpu sees its own private block at BAO_CPU_BASE, so that address cannot name another cpu's
 * block. Instead each cpu maps the blocks of all cpus, its own included, in an alias window at the
 * top of its private section. The alias of a given cpu is the same address on every cpu, which
 * makes it the canonical pointer for per-cpu objects shared with other cpus.
 */

static paddr_t percpu_pa[PLAT_CPU_NUM];

vaddr_t percpu_base(cpuid_t cpuid)
{
    vaddr_t alias_base = (vaddr_t)BAO_VM_BASE - ((size_t)PLAT_CPU_NUM * PERCPU_SIZE);
    return alias_base + (cpuid * PERCPU_SIZE);
}

void percpu_init(void)
{
    if (!mem_translate(&cpu()->as, (vaddr_t)cpu(), &percpu_pa[cpu()->id])) {
        ERROR("failed to translate cpu private block address\n");
    }

    cpu_sync_barrier(&cpu_glb_sync);

    size_t num_pages = NUM_PAGES(PERCPU_SIZE);
    for (cpuid_t cpuid = 0; cpuid < platform.cpu_num; cpuid++) {
        struct ppages ppages = mem_ppages_get(percpu_pa[cpuid], num_pages);
        vaddr_t at = percpu_base(cpuid);
        vaddr_t va =
            mem_alloc_map(&cpu()->as, SEC_HYP_PRIVATE, &ppages, at, num_pages, PTE_HYP_FLAGS);
        if (va != at) {
            ERROR("failed to map cpu %d private block alias\n", cpuid);
        }
    }

    cpu_sync_barrier(&cpu_glb_sync);
}
