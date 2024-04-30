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

#ifndef adpf_gpu_hpp
#define adpf_gpu_hpp

#include "common.hpp"
#include "display_manager.h"
#include "system_event_manager.h"
#include "user_input_manager.h"

#include <android/performance_hint.h>

using namespace base_game_framework;

#define DEFAULT_TARGET_NS 16666666

class AdpfGpu {
    private:
        AdpfGpu();

        APerformanceHintManager *performance_hint_manager_;
        APerformanceHintSession *performance_hint_session_;
        AWorkDuration *work_duration_;
    public:
        ~AdpfGpu();

        static AdpfGpu& getInstance() {
            static AdpfGpu instance;
            return instance;
        }

        void initializePerformanceHintManager(int32_t thread_ids, size_t thread_size, int64_t target_work_duration = DEFAULT_TARGET_NS);
        void uninitializePerformanceHintManager();

        void reportGpuWorkDuration(int64_t work_duration);
};

#endif
