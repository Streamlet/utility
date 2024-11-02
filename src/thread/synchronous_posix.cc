// MIT License
//
// Copyright (c) 2022 Streamlet (streamlet@outlook.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <cassert>
#include <errno.h>
#include <pthread.h>
#include <xl/synchronous>

// mainly rewrite from msquic

namespace xl {

namespace {

typedef struct event_context {

  //
  // Mutex and condition. The alignas is important, as the perf tanks
  // if the event is not aligned.
  //
  alignas(16) pthread_mutex_t mutex = {};
  pthread_cond_t cond = {};

  //
  // Denotes if the event object is in signaled state.
  //

  bool signaled = false;

  //
  // Denotes if the event object should be auto reset after it's signaled.
  //

  bool auto_reset = true;

} event_context;

} // namespace

event::event(bool init_signaled, bool auto_reset) {
  event_context *context = new event_context;
  platform_context_ = context;
  context->signaled = init_signaled;
  context->auto_reset = auto_reset;
  int r = pthread_mutex_init(&context->mutex, NULL);
  assert(r == 0);
  pthread_condattr_t attr = {};
  r = pthread_condattr_init(&attr);
  assert(r == 0);
#ifdef __linux__
  r = pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
  assert(r == 0);
#endif // CX_PLATFORM_LINUX
  r = pthread_cond_init(&context->cond, &attr);
  assert(r == 0);
  r = pthread_condattr_destroy(&attr);
  assert(r == 0);
}

event::~event() {
  event_context *context = (event_context *)platform_context_;
  int r = pthread_cond_destroy(&context->cond);
  assert(r == 0);
  r = pthread_mutex_destroy(&context->mutex);
  assert(r == 0);
  delete context;
  platform_context_ = nullptr;
}

void event::wait() const {
  event_context *context = (event_context *)platform_context_;
  int r = pthread_mutex_lock(&context->mutex);
  assert(r == 0);
  //
  // Spurious wake ups from pthread_cond_wait can occur. So the function needs
  // to be called in a loop until the predicate 'Signalled' is satisfied.
  //
  while (!context->signaled) {
    r = pthread_cond_wait(&context->cond, &context->mutex);
    assert(r == 0);
  }
  if (context->auto_reset) {
    context->signaled = false;
  }
  r = pthread_mutex_unlock(&context->mutex);
  assert(r == 0);
}

namespace {

void time_to_abs(unsigned long milliseconds, struct timespec *ts) {
#ifdef __APPLE__
  //
  // timespec_get is used on darwin, as CLOCK_MONOTONIC isn't actually
  // monotonic according to our tests.
  //
  int r = timespec_get(ts, TIME_UTC);
#else
  int r = clock_gettime(CLOCK_MONOTONIC, ts);
#endif // CX_PLATFORM_DARWIN
  assert(r == 0);
  ts->tv_sec += (milliseconds / 1000);
  ts->tv_nsec += ((milliseconds % 1000) * 1000000);
  if (ts->tv_nsec >= 1000000000) {
    ts->tv_sec += 1;
    ts->tv_nsec -= 1000000000;
  }
  assert(ts->tv_sec >= 0);
  assert(ts->tv_nsec >= 0);
  assert(ts->tv_nsec < 1000000000);
}

} // namespace

bool event::timed_wait(unsigned long milliseconds) const {
  event_context *context = (event_context *)platform_context_;
  assert(milliseconds != -1);
  //
  // Get absolute time.
  //
  struct timespec ts = {0, 0};
  time_to_abs(milliseconds, &ts);

  int r = pthread_mutex_lock(&context->mutex);
  assert(r == 0);

  bool timeout = false;
  while (!context->signaled) {
    r = pthread_cond_timedwait(&context->cond, &context->mutex, &ts);
    if (r == ETIMEDOUT) {
      timeout = true;
      break;
    }
    assert(r == 0);
  }
  if (!timeout && context->auto_reset) {
    context->signaled = false;
  }
  r = pthread_mutex_unlock(&context->mutex);
  assert(r == 0);
  return !timeout;
}

void event::set() const {
  event_context *context = (event_context *)platform_context_;
  int r = pthread_mutex_lock(&context->mutex);
  assert(r == 0);
  context->signaled = true;
  //
  // Signal the condition while holding the lock for predictable scheduling,
  // better performance and removing possibility of use after free for the
  // condition.
  //
  r = pthread_cond_broadcast(&context->cond);
  assert(r == 0);
  r = pthread_mutex_unlock(&context->mutex);
  assert(r == 0);
}

void event::reset() const {
  event_context *context = (event_context *)platform_context_;
  int r = pthread_mutex_lock(&context->mutex);
  assert(r == 0);
  context->signaled = false;
  r = pthread_mutex_unlock(&context->mutex);
  assert(r == 0);
}

} // namespace xl
