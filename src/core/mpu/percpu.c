/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 */

#include <percpu.h>

vaddr_t percpu_base(cpuid_t cpuid)
{
    /**
     * A cpu with a coupled memory region (TCM, local RAM) has its private block placed there by
     * the boot code, the others are laid out back to back starting at the template.
     */
    paddr_t base = cpu_private_base_tbl[cpuid];
    if (base == 0U) {
        base = (vaddr_t)&_percpu_start + (cpuid * PERCPU_SIZE);
    }
    return base;
}

void percpu_init(void)
{
    /* Identity mapped: a cpu's private block is reachable through its physical address */
}
