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

#include <sched.h>
#include <unistd.h>

using namespace base_game_framework;

// verbose debug logs on?
#define VERBOSE_LOGGING 1

#if VERBOSE_LOGGING
#define VLOGD ALOGI
#else
#define VLOGD
#endif

AdpfGpu::AdpfGpu() :
    performance_hint_manager_(nullptr),
    performance_hint_session_(nullptr),
    work_duration_(nullptr)
{
    // int32_t thread_ids, size_t thread_size, int64_t target_work_duration
    int32_t tids[1];
    tids[0] = gettid();
    initializePerformanceHintManager(tids, 1);
}

AdpfGpu::~AdpfGpu()
{
    uninitializePerformanceHintManager();
}

void AdpfGpu::initializePerformanceHintManager(int32_t *thread_ids, size_t thread_size, int64_t target_work_duration)
{
    ALOGI("AdpfGpu::initializePerformanceHintManager %d", __ANDROID_API__);
#if __ANDROID_API__ >= 35
    performance_hint_manager_ = APerformanceHint_getManager();
    performance_hint_session_ = APerformanceHint_createSession(performance_hint_manager_, thread_ids, thread_size, target_work_duration);
    work_duration_ = AWorkDuration_create();
#endif
}

void AdpfGpu::uninitializePerformanceHintManager()
{
#if __ANDROID_API__ >= 35
    if ( work_duration_ != nullptr ) {
        AWorkDuration_release(work_duration_);
        work_duration_ = nullptr;
    }
    if ( performance_hint_session_ != nullptr ) {
        APerformanceHint_closeSession(performance_hint_session_);
        performance_hint_session_ = nullptr;
    }
    performance_hint_manager_ = nullptr;
#endif
}

void AdpfGpu::reportGpuWorkDuration(int64_t work_duration)
{
    if ( performance_hint_manager_ != nullptr && 
        performance_hint_session_ != nullptr &&  work_duration_ != nullptr ) {
#if __ANDROID_API__ >= 35
        ALOGI("AdpfGpu::reportGpuWorkDuration %" PRIu64 "", work_duration);
        AWorkDuration_setActualGpuDurationNanos(work_duration_, work_duration);
        APerformanceHint_reportActualWorkDuration2(performance_hint_session_, work_duration_);
#endif
    } else {
        ALOGI("AdpfGpu::reportGpuWorkDuration performance_hint_manager_ is null : %p", performance_hint_manager_);
    }
}
