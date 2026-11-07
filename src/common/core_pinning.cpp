// SPDX-FileCopyrightText: Copyright 2026 Eden Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <algorithm>

#include "common/core_pinning.h"

namespace Common {

GpuThreadPinning PlanGpuThreadPinning(std::span<const LogicalProcessor> processors) {
    GpuThreadPinning plan;
    if (processors.empty()) {
        return plan;
    }
    const u8 fastest =
        std::ranges::max(processors, {}, &LogicalProcessor::efficiency_class).efficiency_class;
    bool has_efficiency_cores = false;
    std::vector<u32> cores;
    for (const LogicalProcessor& processor : processors) {
        if (processor.efficiency_class == fastest) {
            cores.push_back(processor.core);
        } else {
            has_efficiency_cores = true;
        }
    }
    std::ranges::sort(cores);
    cores.erase(std::unique(cores.begin(), cores.end()), cores.end());
    plan.performance_cores = cores.size();
    if (cores.size() < MinCoresForGpuThreadPinning) {
        return plan;
    }
    const u32 front_core = cores[1];
    const u32 back_core = cores[2];
    // On >=8-core uniform CPUs (e.g. Ryzen 7 7800X3D), isolate the 4 Switch 2 guest cores onto
    // physical cores 3..6 and place background workers (shader builders, unswizzlers) on cores
    // 0 and 7+ so background bursts never contend with Dynarmic guest threads.
    const bool isolate_uniform_background =
        !has_efficiency_cores && cores.size() >= MinCoresForDedicatedBackground;

    std::array<u32, GuestCoreCount> dedicated_guest_cores{};
    if (isolate_uniform_background) {
        for (std::size_t i = 0; i < GuestCoreCount; ++i) {
            dedicated_guest_cores[i] = cores[3 + i];
        }
    } else {
        std::size_t slot = 0;
        for (const u32 core_id : cores) {
            if (core_id == front_core || core_id == back_core) {
                continue;
            }
            if (slot < GuestCoreCount) {
                dedicated_guest_cores[slot++] = core_id;
            }
        }
    }

    for (const LogicalProcessor& processor : processors) {
        if (processor.efficiency_class != fastest) {
            plan.background.push_back(processor.id);
            continue;
        }
        if (processor.core == front_core) {
            plan.front.push_back(processor.id);
            continue;
        }
        if (processor.core == back_core) {
            plan.back.push_back(processor.id);
            continue;
        }
        for (std::size_t i = 0; i < GuestCoreCount; ++i) {
            if (processor.core == dedicated_guest_cores[i]) {
                plan.guest_cores[i].push_back(processor.id);
                break;
            }
        }
        plan.guest.push_back(processor.id);
        if (isolate_uniform_background) {
            // Keep Core 3 (CPUCore_0) and Core 4 (CPUCore_1) 100% isolated from background shader
            // and unswizzle workers alongside Core 1 (GPUFront) and Core 2 (GPUBack), while giving
            // BackgroundWorkers 4 physical cores (Cores 0, 5, 6, 7 = 8 logical threads).
            const bool is_primary_guest_core =
                processor.core == dedicated_guest_cores[0] ||
                processor.core == dedicated_guest_cores[1];
            if (!is_primary_guest_core) {
                plan.background.push_back(processor.id);
            }
        }
    }
    if (plan.background.empty()) {
        plan.background = plan.guest;
    }
    return plan;
}

} // namespace Common
