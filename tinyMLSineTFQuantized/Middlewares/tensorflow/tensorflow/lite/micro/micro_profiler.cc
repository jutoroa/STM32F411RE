/* Copyright 2020 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#include "tensorflow/lite/micro/micro_profiler.h"

#include <cstdint>

#include "tensorflow/lite/kernels/internal/compatibility.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_time.h"

namespace tflite {

uint32_t MicroProfiler::BeginEvent(const char* tag) {
#if defined(TF_LITE_PROFILER_WITH_BUFFER)
  if (num_events_ == kMaxEvents) {
    num_events_ = 0;
  }

  tags_[num_events_] = tag;
  start_ticks_[num_events_] = GetCurrentTimeTicks();
  end_ticks_[num_events_] = start_ticks_[num_events_] - 1;
#endif
  return num_events_++;
}

void MicroProfiler::EndEvent(uint32_t event_handle) {
#if defined(TF_LITE_PROFILER_WITH_BUFFER)
  TFLITE_DCHECK(event_handle < kMaxEvents);
  end_ticks_[event_handle] = GetCurrentTimeTicks();
#endif
}

int32_t MicroProfiler::GetTotalTicks() const {
#if defined(TF_LITE_PROFILER_WITH_BUFFER)
  int32_t ticks = 0;
  for (int i = 0; i < num_events_; ++i) {
    ticks += end_ticks_[i] - start_ticks_[i];
  }
  return ticks;
#endif
  return 0;
}

void MicroProfiler::Log() const {
#if defined(TF_LITE_PROFILER_WITH_BUFFER)
#if !defined(TF_LITE_STRIP_ERROR_STRINGS)
  for (int i = 0; i < num_events_; ++i) {
    int32_t ticks = end_ticks_[i] - start_ticks_[i];
    MicroPrintf("%s took %d ticks (%d ms).", tags_[i], ticks, TicksToMs(ticks));
  }
#endif
#endif
}

void MicroProfiler::LogCsv() const {
#if defined(TF_LITE_PROFILER_WITH_BUFFER)
#if !defined(TF_LITE_STRIP_ERROR_STRINGS)
  MicroPrintf("\"Event\",\"Tag\",\"Ticks\"");
  for (int i = 0; i < num_events_; ++i) {
    int32_t ticks = end_ticks_[i] - start_ticks_[i];
    MicroPrintf("%d,%s,%d", i, tags_[i], ticks);
  }
#endif
#endif
}

}  // namespace tflite
