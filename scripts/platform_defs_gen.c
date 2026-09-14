/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) Bao Project and Contributors. All rights reserved
 */

#include <stdio.h>
#include <platform.h>
#include <bao.h>

__attribute__((weak)) void arch_platform_defs(void){
    return;
}

/**
 * Whether the architecture's boot code places each cpu's private block through the generated
 * per-cpu base table (see PLAT_CPU_PRIVATE_BASES). Ports that do override this in their own
 * platform_defs_gen.c.
 */
__attribute__((weak)) bool arch_cpu_private_placement(void){
    return false;
}

int main() {

    size_t bitmap_array_size = 0;

    printf("#define PLAT_CPU_NUM (%ld)\n", platform.cpu_num);
    printf("#define PLAT_BASE_ADDR (0x%lx)\n", platform.regions[0].base);

    for(size_t i = 1; i < platform.region_num; i++)
    {
        /*
         * Selects the first memory region with RWX (read, write, execute) permissions, and defines
         * it as PLAT_DATA_ADDR. This region is considered the main data memory that Bao will use
         * for its own purposes.
         */
        if(platform.regions[i].perms == MEM_RWX)
        {
            printf("#define PLAT_DATA_ADDR (0x%lx)\n", platform.regions[i].base);
            break;
        }
    }

    for(size_t i = 0; i < platform.region_num; i++)
    {
        size_t reg_size;

        /* Cpu-affine regions are never page pools */
        if (platform.regions[i].cpu_affinity != 0) {
            continue;
        }

        reg_size = platform.regions[i].size;

        bitmap_array_size += BITMAP_SIZE_IN_BYTES(NUM_PAGES(reg_size));
    }

    printf("#define PLAT_BITMAP_POOL_SIZE (0x%lx)\n", bitmap_array_size);

    /*
     * Cpu private block placement: the first region affine to exactly one cpu hosts that cpu's
     * private block. Cpus without such a region get 0 and fall back to the image-adjacent layout.
     */
    size_t private_min_size = 0;
    bool private_any = false;
    printf("#define PLAT_CPU_PRIVATE_BASES {");
    for (size_t cpu = 0; cpu < platform.cpu_num; cpu++) {
        paddr_t base = 0;
        for (size_t i = 0; i < platform.region_num; i++) {
            if (platform.regions[i].cpu_affinity == (1UL << cpu)) {
                base = platform.regions[i].base;
                if (!private_any || platform.regions[i].size < private_min_size) {
                    private_min_size = platform.regions[i].size;
                }
                private_any = true;
                break;
            }
        }
        printf("%s0x%lx", cpu == 0 ? " " : ", ", base);
    }
    printf(" }\n");
    if (private_any) {
        if (!arch_cpu_private_placement()) {
            fprintf(stderr, "cpu-affine memory regions are not supported by this architecture's "
                "boot code yet\n");
            return 1;
        }
        printf("#define PLAT_CPU_PRIVATE_MIN_SIZE (0x%lx)\n", private_min_size);
    }

    if (platform.cpu_master_fixed) {
        printf("#define CPU_MASTER_FIXED (%ld)\n", platform.cpu_master);
    }
    // Call arch specific platform defines generator
    arch_platform_defs();
    return 0;
}
