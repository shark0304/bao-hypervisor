/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 */

#include <vm.h>
#include <percpu.h>
#include <platform.h>

void vm_mem_prot_cpu_init(struct vm* vm)
{
    /**
     * The vcpus of the vm live in the private blocks of their physical cpus. Make the blocks of
     * the vm's other cpus reachable from this one (one locked entry per sibling).
     */
    for (cpuid_t cpuid = 0; cpuid < platform.cpu_num; cpuid++) {
        if ((cpuid == cpu()->id) || !bit_get(vm->cpus, cpuid)) {
            continue;
        }
        struct mp_region mpr = {
            .base = percpu_base(cpuid),
            .size = PERCPU_SIZE,
            .mem_flags = PTE_HYP_FLAGS,
            .as_sec = SEC_HYP_VM,
        };
        if (!mem_map(&cpu()->as, &mpr, MEM_DONT_BROADCAST, MEM_LOCKED)) {
            ERROR("failed to map cpu %d private block\n", cpuid);
        }
    }
}

void vm_mem_prot_init(struct vm* vm, const struct vm_config* config)
{
    UNUSED_ARG(config);

    as_init(&vm->as, AS_VM, 0);

    if (DEFINED(MMIO_SLAVE_SIDE_PROT) && (vm->master == cpu()->id)) {
        mem_mmio_init_regions(&vm->as);
    }
}
