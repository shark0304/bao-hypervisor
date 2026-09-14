/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) Bao Project and Contributors. All rights reserved
 */

#include <stdio.h>
#include <platform.h>

void arch_platform_defs() {
    printf("#define PLAT_INTC1_ADDR (0x%lx)\n", platform.arch.intc.intc1_addr);
}

/* The rh850 boot code places each cpu's private block through the generated table */
bool arch_cpu_private_placement(void) {
    return true;
}
