/*
 * Copyright 2023 The Android Open Source Project
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

#include "renderer_gles.h"
#include "renderer_debug.h"
#include "renderer_index_buffer_gles.h"
#include "renderer_render_pass_gles.h"
#include "renderer_render_state_gles.h"
#include "renderer_shader_program_gles.h"
#include "renderer_texture_gles.h"
#include "renderer_uniform_buffer_gles.h"
#include "renderer_vertex_buffer_gles.h"
#include "display_manager.h"
#include "gles/graphics_api_gles_resources.h"

#include <chrono>
#include <inttypes.h>

#include <android/log.h>

// #include <EGL/egl.h>
// #include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include "adpf_gpu.hpp"

#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__);
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__);
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__);
#ifdef NDEBUG
#define ALOGV(...)
#else
#define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__);
#endif

using namespace base_game_framework;

namespace simple_renderer {

static const char *kAstcExtensionString = "GL_OES_texture_compression_astc";
static const char *kDisjointTimerQueryExtensionString = "GL_EXT_disjoint_timer_query";

RendererGLES::RendererGLES() {
  GraphicsAPIResourcesGLES graphics_api_resources_gles;
  SwapchainFrameResourcesGLES swapchain_frame_resources_gles;
  DisplayManager& display_manager = DisplayManager::GetInstance();
  display_manager.GetGraphicsAPIResourcesGLES(graphics_api_resources_gles);
  const base_game_framework::DisplayManager::SwapchainFrameHandle frame_handle =
      display_manager.GetCurrentSwapchainFrame(Renderer::GetSwapchainHandle());
  display_manager.GetSwapchainFrameResourcesGLES(frame_handle, swapchain_frame_resources_gles);
  egl_context_ = graphics_api_resources_gles.egl_context;
  egl_display_ = swapchain_frame_resources_gles.egl_display;
  egl_surface_ = swapchain_frame_resources_gles.egl_surface;

  listFeaturesAvailable();

  // Call BeginFrame to make sure the context is set in case the user starts creating resources
  // immediately after initialization
  first_call_ = true;
  timestamp_query_available_ = GetFeatureAvailable(RendererFeature::kFeature_DisjointTimerQuery);
  BeginFrame(Renderer::GetSwapchainHandle());
}

RendererGLES::~RendererGLES() {
}

void RendererGLES::PrepareShutdown() {
  render_pass_ = nullptr;
  render_state_ = nullptr;
  resources_.ProcessDeleteQueue();
}

void RendererGLES::listFeaturesAvailable() {
  GLint extensionCount = 0;

  glGetIntegerv(GL_NUM_EXTENSIONS, &extensionCount);
  ALOGI("RendererGLES::listFeaturesAvailable extensionCount: %d", extensionCount);
  for (GLint i = 0; i < extensionCount; i++) {
    const GLubyte *extensionString = glGetStringi(GL_EXTENSIONS, i);
    const char* extStr = reinterpret_cast<const char*>(extensionString);
    ALOGI("RendererGLES extension found: %s", extStr);
    // if (strcmp(reinterpret_cast<const char *>(extensionString), kAstcExtensionString) == 0) {
    //   supported = true;
    //   break;
    // }
  }
}

  void RendererGLES::testQueryTimer()
  {
    GLsizei N = 1;
    GLuint queries[1]; // [N]
    GLuint available = 0;
    GLint disjointOccurred = 0;

    ALOGI("RendererGLES::testQueryTimer");

    /* Timer queries can contain more than 32 bits of data, so always
        query them using the 64 bit types to avoid overflow */
    GLuint timeElapsed = 0;

    /* Create a query object. */
    // glGenQueries (GLsizei n, GLuint *ids);
    glGenQueries(N, queries);
    
    /* Clear disjoint error */
    glGetIntegerv(GL_GPU_DISJOINT_EXT, &disjointOccurred);

    /* Start query 1 */
    glBeginQuery(GL_TIME_ELAPSED_EXT, queries[0]);

    /* Draw object 1 */
    //....

    /* End query 1 */
    glEndQuery(GL_TIME_ELAPSED_EXT);

    //...

    /* Start query N */
    glBeginQuery(GL_TIME_ELAPSED_EXT, queries[N-1]);

    /* Draw object N */
    //....

    /* End query N */
    glEndQuery(GL_TIME_ELAPSED_EXT);

    /* Wait for all results to become available */
    while (!available) {
        glGetQueryObjectuiv(queries[N-1], GL_QUERY_RESULT_AVAILABLE, &available);
    }
    
    /* Check for disjoint operation for all queries within the last
        disjoint check. This way we can only check disjoint once for all
        queries between, and once the last is filled we know all previous
        will have been filled as well */
    glGetIntegerv(GL_GPU_DISJOINT_EXT, &disjointOccurred);
    
    /* If a disjoint operation occurred, all timer queries in between
        the last two disjoint checks that were filled are invalid, continue
        without reading the the values */
    ALOGI("RendererGLES::testQueryTimer disjointOccured: %d", disjointOccurred);
    if (!disjointOccurred) {
        for (int i = 0; i < N; i++) {
            /* See how much time the rendering of object i took in nanoseconds. */
            //glGetQueryObjectui64vEXT(queries[i], GL_QUERY_RESULT, &timeElapsed);
            glGetQueryObjectuiv(queries[i], GL_QUERY_RESULT, &timeElapsed);
            
            /* Do something useful with the time if a disjoint operation did
                not occur.  Note that care should be taken to use all
                significant bits of the result, not just the least significant
                32 bits. */
            //AdjustObjectLODBasedOnDrawTime(i, timeElapsed);
            ALOGI("RendererGLES::testQueryTimer timeElapsed %d => %d", i, timeElapsed);
        }
    }

    // https://registry.khronos.org/OpenGL/extensions/EXT/EXT_disjoint_timer_query.txt
    // This example is sub-optimal in that it stalls at the end of every
    // frame to wait for query results.  Ideally, the collection of results
    // would be delayed one frame to minimize the amount of time spent
    // waiting for the GPU to finish rendering.
  }

  GLuint queries;
  GLuint available = 0;
  GLint disjointOccurred = 0;
  GLuint timeElapsed = 0;
  void RendererGLES::StartQueryTimer()
  {
    //GLsizei N = 1;
    // GLuint queries[N];
    // GLuint available = 0;
    // GLint disjointOccurred = 0;

    ALOGI("RendererGLES::StartQueryTimer START");

    // CPU_PERF_HINT
    cpu_clock_start_ = std::chrono::high_resolution_clock::now();
    auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(cpu_clock_start_.time_since_epoch()).count();
    AdpfGpu::getInstance().setWorkPeriodStartTimestampNanos(nanos);

    /* Timer queries can contain more than 32 bits of data, so always
        query them using the 64 bit types to avoid overflow */
    // GLuint timeElapsed = 0;

    /* Create a query object. */
    // glGenQueries (GLsizei n, GLuint *ids);
    glGenQueries(1, &queries);
    
    /* Clear disjoint error */
    glGetIntegerv(GL_GPU_DISJOINT_EXT, &disjointOccurred);

    /* Start query 1 */
    glBeginQuery(GL_TIME_ELAPSED_EXT, queries);

    ALOGI("RendererGLES::StartQueryTimer END");
  }

  void RendererGLES::EndQueryTimer()
  {
    ALOGI("RendererGLES::EndQueryTimer START");

    // CPU_PERF_HINT
    auto cpu_clock_end = std::chrono::high_resolution_clock::now();
    auto cpu_clock_past = cpu_clock_end - cpu_clock_start_;
    auto cpu_clock_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(cpu_clock_past).count();
    int64_t duration_ns = static_cast<int64_t>(cpu_clock_duration);
    AdpfGpu::getInstance().setActualCpuDurationNanos(duration_ns);
    AdpfGpu::getInstance().setActualTotalDurationNanos(duration_ns);

    /* End query N */
    glEndQuery(GL_TIME_ELAPSED_EXT);

    /* Wait for all results to become available */
    while (!available) {
        glGetQueryObjectuiv(queries, GL_QUERY_RESULT_AVAILABLE, &available);
    }
    
    /* Check for disjoint operation for all queries within the last
        disjoint check. This way we can only check disjoint once for all
        queries between, and once the last is filled we know all previous
        will have been filled as well */
    glGetIntegerv(GL_GPU_DISJOINT_EXT, &disjointOccurred);
    
    /* If a disjoint operation occurred, all timer queries in between
        the last two disjoint checks that were filled are invalid, continue
        without reading the the values */
    // ALOGI("RendererGLES::EndQueryTimer disjointOccured: %d", disjointOccurred);

    glGetQueryObjectuiv(queries, GL_QUERY_RESULT, &timeElapsed);
    ALOGI("RendererGLES::EndQueryTimer disjointOccured: %d, timeElapsed %d", disjointOccurred, timeElapsed);

//     if (!disjointOccurred) {
//         glGetQueryObjectuiv(queries, GL_QUERY_RESULT, &timeElapsed);
//         ALOGI("RendererGLES::EndQueryTimer timeElapsed %d", timeElapsed);

// //        for (int i = 0; i < N; i++) {
// //            /* See how much time the rendering of object i took in nanoseconds. */
// //            //glGetQueryObjectui64vEXT(queries[i], GL_QUERY_RESULT, &timeElapsed);
// //            glGetQueryObjectuiv(queries[i], GL_QUERY_RESULT, &timeElapsed);
// //
// //            /* Do something useful with the time if a disjoint operation did
// //                not occur.  Note that care should be taken to use all
// //                significant bits of the result, not just the least significant
// //                32 bits. */
// //            //AdjustObjectLODBasedOnDrawTime(i, timeElapsed);
// //            ALOGI("RendererGLES::EndQueryTimer timeElapsed %d => %d", i, timeElapsed);
// //        }

//     }

    // https://registry.khronos.org/OpenGL/extensions/EXT/EXT_disjoint_timer_query.txt
    // This example is sub-optimal in that it stalls at the end of every
    // frame to wait for query results.  Ideally, the collection of results
    // would be delayed one frame to minimize the amount of time spent
    // waiting for the GPU to finish rendering.

    int64_t workDuration = (int64_t) timeElapsed;
    // AdpfGpu::getInstance().reportGpuWorkDuration(workDuration);
    AdpfGpu::getInstance().setActualGpuDurationNanos(workDuration);

    ALOGI("RendererGLES::EndQueryTimer END %" PRIu64 "", workDuration);
  }

bool RendererGLES::GetFeatureAvailable(const RendererFeature feature) {
  bool supported = false;
  switch (feature) {
    case Renderer::kFeature_ASTC:
    {
      GLint extensionCount = 0;

      glGetIntegerv(GL_NUM_EXTENSIONS, &extensionCount);
      for (GLint i = 0; i < extensionCount; i++) {
        const GLubyte *extensionString = glGetStringi(GL_EXTENSIONS, i);
        if (strcmp(reinterpret_cast<const char *>(extensionString), kAstcExtensionString) == 0) {
          supported = true;
          break;
        }
      }
    }
    case Renderer::kFeature_DisjointTimerQuery:
    {
      GLint extensionCount = 0;

      glGetIntegerv(GL_NUM_EXTENSIONS, &extensionCount);
      for (GLint i = 0; i < extensionCount; i++) {
        const GLubyte *extensionString = glGetStringi(GL_EXTENSIONS, i);
        if (strcmp(reinterpret_cast<const char *>(extensionString), kDisjointTimerQueryExtensionString) == 0) {
          supported = true;
          break;
        }
      }
    }
      break;
    default:
      break;
  }
  return supported;
}

void RendererGLES::BeginFrame(
    const base_game_framework::DisplayManager::SwapchainHandle /*swapchain_handle*/) {

  if ( timestamp_query_available_ ) {
      if ( first_call_ ) {
        first_call_ = false;
      } else {
        StartQueryTimer();
      }
  }
  
  resources_.ProcessDeleteQueue();
  EGLBoolean result = eglMakeCurrent(egl_display_, egl_surface_, egl_surface_, egl_context_);
  if (result == EGL_FALSE) {
    RENDERER_ERROR("eglMakeCurrent failed: %d", eglGetError())
  }

  // Make sure errors are cleared at top of frame
  GLenum gl_error = glGetError();
  while (gl_error != GL_NO_ERROR) {
    gl_error = glGetError();
  }
}

void RendererGLES::EndFrame() {
  if ( timestamp_query_available_ ) {
    EndQueryTimer();
  }
  
  EndRenderPass();

  // Clear current render pass
  render_pass_ = nullptr;

}

void RendererGLES::SwapchainRecreated() {

}

void RendererGLES::EndRenderPass() {
  // Unbind any current render state
  if (render_state_ != nullptr) {
    RenderStateGLES& previous_state = *(static_cast<RenderStateGLES*>(render_state_.get()));
    previous_state.UnbindRenderState();
  }
  // Clear current render state
  render_state_ = nullptr;

  // Tell the current render pass to end
  if (render_pass_ != nullptr) {
    render_pass_->EndRenderPass();
  }
}

void RendererGLES::Draw(const uint32_t vertex_count, const uint32_t first_vertex) {
  // Update any uniform data that might have changed between draw calls
  RenderStateGLES& state = *(static_cast<RenderStateGLES*>(render_state_.get()));
  state.UpdateUniformData(true);

  glDrawArrays(state.GetPrimitiveType(), first_vertex, vertex_count);
  RENDERER_CHECK_GLES("glDrawArrays");
}

void RendererGLES::DrawIndexed(const uint32_t index_count, const uint32_t first_index) {
  // Update any uniform data that might have changed between draw calls
  RenderStateGLES& state = *(static_cast<RenderStateGLES*>(render_state_.get()));
  state.UpdateUniformData(true);

  // Currently fixed to 16-bit index values
  const void* first_index_offset = reinterpret_cast<const void*>((first_index * sizeof(uint16_t)));
  glDrawElements(state.GetPrimitiveType(),
                 index_count, GL_UNSIGNED_SHORT, first_index_offset);
  RENDERER_CHECK_GLES("glDrawElements");
}

void RendererGLES::SetRenderPass(std::shared_ptr<RenderPass> render_pass) {
  // End any currently active render pass
  EndRenderPass();

  render_pass_ = render_pass;
  // Call BeginRenderPass on the new one
  if (render_pass != nullptr) {
    render_pass->BeginRenderPass();
  }
}

void RendererGLES::SetRenderState(std::shared_ptr<RenderState> render_state) {
  // Exit early if we are setting a render state that is already the current one
  if (render_state_.get() == render_state.get()) {
    return;
  }

  // Unbind resources from any currently active render state
  if (render_state_ != nullptr) {
    RenderStateGLES& previous_state = *(static_cast<RenderStateGLES*>(render_state_.get()));
    previous_state.UnbindRenderState();
  }

  // Set the new state and bind its resources
  render_state_ = render_state;
  RenderStateGLES& state = *(static_cast<RenderStateGLES*>(render_state.get()));
  state.BindRenderState();
}

void RendererGLES::BindIndexBuffer(std::shared_ptr<IndexBuffer> index_buffer) {
  if (index_buffer == nullptr) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    RENDERER_CHECK_GLES("glBindBuffer GL_ELEMENT_ARRAY_BUFFER reset");
  } else {
    const IndexBufferGLES& index = *static_cast<IndexBufferGLES *>(index_buffer.get());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index.GetIndexBufferObject());
    RENDERER_CHECK_GLES("glBindBuffer GL_ELEMENT_ARRAY_BUFFER");
  }
}

void RendererGLES::BindVertexBuffer(std::shared_ptr<VertexBuffer> vertex_buffer) {
  if (vertex_buffer == nullptr) {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    RENDERER_CHECK_GLES("glBindBuffer GL_ARRAY_BUFFER reset");
  } else {
    const VertexBufferGLES& vertex = *static_cast<VertexBufferGLES *>(vertex_buffer.get());
    glBindBuffer(GL_ARRAY_BUFFER, vertex.GetVertexBufferObject());
    RENDERER_CHECK_GLES("glBindBuffer GL_ARRAY_BUFFER");
  }
}

void RendererGLES::BindTexture(std::shared_ptr<Texture> texture) {
  if (texture == nullptr) {
    glBindTexture(GL_TEXTURE_2D, 0);
    RENDERER_CHECK_GLES("glBindTexture reset");
  } else {
    const TextureGLES& tex = *static_cast<TextureGLES *>(texture.get());
    glBindTexture(GL_TEXTURE_2D, tex.GetTextureObject());
    RENDERER_CHECK_GLES("glBindTexture");
  }
}

std::shared_ptr<IndexBuffer> RendererGLES::CreateIndexBuffer(
    const IndexBuffer::IndexBufferCreationParams& params) {
  return resources_.AddIndexBuffer(new IndexBufferGLES(params));
}

void RendererGLES::DestroyIndexBuffer(std::shared_ptr<IndexBuffer> index_buffer) {
  resources_.QueueDeleteIndexBuffer(index_buffer);
}

std::shared_ptr<RenderPass> RendererGLES::CreateRenderPass(
    const RenderPass::RenderPassCreationParams& params) {
  return resources_.AddRenderPass(new RenderPassGLES(params));
}

void RendererGLES::DestroyRenderPass(std::shared_ptr<RenderPass> render_pass) {
  resources_.QueueDeleteRenderPass(render_pass);
}

std::shared_ptr<RenderState> RendererGLES::CreateRenderState(
    const RenderState::RenderStateCreationParams& params) {
  return resources_.AddRenderState(new RenderStateGLES(params));
}

void RendererGLES::DestroyRenderState(std::shared_ptr<RenderState> render_state) {
  resources_.QueueDeleteRenderState(render_state);
}

std::shared_ptr<ShaderProgram> RendererGLES::CreateShaderProgram(
    const ShaderProgram::ShaderProgramCreationParams& params) {
  return resources_.AddShaderProgram(new ShaderProgramGLES(params));
}

void RendererGLES::DestroyShaderProgram(std::shared_ptr<ShaderProgram> shader_program) {
  resources_.QueueDeleteShaderProgram(shader_program);
}

std::shared_ptr<Texture> RendererGLES::CreateTexture(const Texture::TextureCreationParams& params) {
  return resources_.AddTexture(new TextureGLES(params));
}

void RendererGLES::DestroyTexture(std::shared_ptr<Texture> texture) {
  resources_.QueueDeleteTexture(texture);
}

std::shared_ptr<UniformBuffer> RendererGLES::CreateUniformBuffer(
    const UniformBuffer::UniformBufferCreationParams& params) {
  return resources_.AddUniformBuffer(new UniformBufferGLES(params));
}

void RendererGLES::DestroyUniformBuffer(std::shared_ptr<UniformBuffer> uniform_buffer) {
  resources_.QueueDeleteUniformBuffer(uniform_buffer);
}

std::shared_ptr<VertexBuffer> RendererGLES::CreateVertexBuffer(
    const VertexBuffer::VertexBufferCreationParams& params) {
  return resources_.AddVertexBuffer(new VertexBufferGLES(params));
}

void RendererGLES::DestroyVertexBuffer(std::shared_ptr<VertexBuffer> vertex_buffer) {
  resources_.QueueDeleteVertexBuffer(vertex_buffer);
}

}
