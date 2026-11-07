// SPDX-FileCopyrightText: Copyright 2026 Eden Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <array>
#include <cstddef>
#include <span>
#include <vector>

#include "common/common_types.h"

namespace Common {

/// One logical processor as the host reports it.
struct LogicalProcessor {
    /// What the host's affinity call takes (a CPU set id on Windows).
    u32 id;
    /// The physical core: equal for SMT siblings.
    u32 core;
    /// Higher is faster. Equal on every processor of a CPU that is not hybrid.
    u8 efficiency_class;
};

/// Physical cores of the fastest class needed before the GPU threads are pinned.
constexpr std::size_t MinCoresForGpuThreadPinning = 6;

/// Physical cores of the fastest class needed on a uniform (non-hybrid) CPU before the 4 guest
/// CPU cores and background workers are partitioned onto separate physical cores.
constexpr std::size_t MinCoresForDedicatedBackground = 8;

/// Number of emulated Switch / Switch 2 physical ARMv8 CPU cores.
constexpr std::size_t GuestCoreCount = 4;

/// Logical processor ids for each pinned role. All sets are empty when the host has fewer than
/// MinCoresForGpuThreadPinning performance cores.
struct GpuThreadPinning {
    std::size_t performance_cores{};
    std::vector<u32> front;
    std::vector<u32> back;
    std::vector<u32> guest;
    /// Dedicated physical core CPU-set IDs for each of the 4 guest ARMv8 CPU cores.
    std::array<std::vector<u32>, GuestCoreCount> guest_cores{};
    /// Efficiency cores on hybrid CPUs, dedicated non-guest cores (Core 0 + Core 7+) on >=8-core
    /// uniform CPUs, or the non-GPU guest cores on 6-core uniform CPUs.
    std::vector<u32> background;
};

/// The split GPU thread's front and back each get a whole physical performance core, so no SMT
/// sibling shares it, and the guest cores get the remaining performance cores. Core 0 takes most
/// interrupts, so the GPU threads get the next two.
[[nodiscard]] GpuThreadPinning PlanGpuThreadPinning(std::span<const LogicalProcessor> processors);

} // namespace Common
