/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 */

#ifndef __PERCPU_H__
#define __PERCPU_H__

#include <bao.h>
#include <cpu.h>

/**
 * Per-cpu objects live in the per-cpu private block described by the .percpu linker section: the
 * cpu structure (always first) followed by every object declared with DEFINE_PERCPU. The section
 * is only a template. Each cpu owns one instance of the whole block, located by the boot code,
 * which is why the objects must never be accessed through their link-time address but only
 * through percpu_get().
 */

extern uint8_t _percpu_start;
extern uint8_t _percpu_end;

/**
 * Size of the block, computed from the section bounds rather than from the linker's absolute
 * _percpu_size symbol (kept for the boot code): a pc-relative reference to an absolute symbol
 * does not link with lld on RISC-V.
 */
#define PERCPU_SIZE               ((size_t)((uintptr_t) & _percpu_end - (uintptr_t) & _percpu_start))

#define DEFINE_PERCPU(type, name) type name __attribute__((section(".percpu")))

#define PERCPU_OFFSET(name)       ((uintptr_t) & (name) - (uintptr_t) & _percpu_start)

/**
 * The instance of a per-cpu object belonging to the given cpu, through an address that is valid
 * on every cpu (see percpu_base()), so the resulting pointer may be shared with other cpus. The
 * calling cpu's own instance is percpu_get(name, cpu()->id).
 */
#define percpu_get(name, cpuid)   ((__typeof__(name)*)(percpu_base(cpuid) + PERCPU_OFFSET(name)))

/**
 * Address of the private block of the given cpu, valid on every cpu of the platform. On MPU
 * targets this is the physical location of the block; on MMU targets it is an alias mapping set
 * up by percpu_init() in every cpu's private address space section.
 */
vaddr_t percpu_base(cpuid_t cpuid);

/* Must run on every cpu after mem_init(), before per-cpu objects are shared. */
void percpu_init(void);

#endif /* __PERCPU_H__ */
