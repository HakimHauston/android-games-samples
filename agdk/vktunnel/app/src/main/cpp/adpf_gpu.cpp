/*
 * Copyright 2021 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "common.hpp"
#include "input_util.hpp"
#include "scene_manager.hpp"
#include "loader_scene.hpp"
#include "native_engine.hpp"
#include "welcome_scene.hpp"

#include "android/platform_util_android.h"
#include "simple_renderer/renderer_interface.h"

#include "adpf_gpu.hpp"

#include <inttypes.h>
#include <android/performance_hint.h>

using namespace base_game_framework;

// verbose debug logs on?
#define VERBOSE_LOGGING 1

#if VERBOSE_LOGGING
#define VLOGD ALOGI
#else
#define VLOGD
#endif

void AdpfGpu::reportGpuWorkDuration(int64_t work_duration)
{
    ALOGI("RendererGLES::AdpfGpu::reportGpuWorkDuration %" PRIu64 "", work_duration);
}
