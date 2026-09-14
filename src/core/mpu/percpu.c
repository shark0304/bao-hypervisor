/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 */

#include <percpu.h>

vaddr_t percpu_base(cpuid_t cpuid)
{
    /* Private blocks are laid out back to back by the boot code, starting at the template */
    return (vaddr_t)&_percpu_start + (cpuid * PERCPU_SIZE);
}

void percpu_init(void)
{
    /* Identity mapped: a cpu's private block is reachable through its physical address */
}
