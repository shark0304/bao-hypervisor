/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 */

#include <platform.h>
#include <percpu.h>
#include <arch/csa.h>

/* Each cpu's context save areas live in its private block (DSPR when the platform couples it) */
DEFINE_PERCPU(union csa, csa_pool[CSA_ENTRIES]);
