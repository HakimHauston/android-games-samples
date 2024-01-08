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


#include "basegameframework_init.h"
#include "tunnel_engine.hpp"

#include "Log.h"

extern "C" {
    void android_main(struct android_app *app);
};

/*
    android_main (not main) is our game entry function, it is called from
    the native app glue utility code as part of the onCreate handler.
*/

void android_main(struct android_app *app) {
    ALOGI("LSF android_main starting");
    PlatformInitParameters init_params{app};
    BaseGameFramework_Init(init_params);
    TunnelEngine *engine = new TunnelEngine(app);
    engine->GameLoop();
    delete engine;
    BaseGameFramework_Destroy();
    ALOGI("LSF android_main quitting");
}
