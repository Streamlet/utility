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
#include <thread>
#include <xl/thread>

namespace xl {

thread::thread(std::function<void()> &&thread_routine) : thread_routine_(std::move(thread_routine)) {
  int r = pthread_create(&handle_, nullptr, run, this);
  assert(r == 0);
}

thread::~thread() {
  assert(!joinable());
  detach();
}

pthread_t thread::native_handle() const {
  return handle_;
}

bool thread::joinable() const {
  return handle_ != 0;
}

void thread::join() {
  void *thread_return = nullptr;
  pthread_join(handle_, &thread_return);
  handle_ = 0;
}

void thread::detach() {
  pthread_detach(handle_);
  handle_ = 0;
}

void *thread::run(void *context) {
  ((thread *)context)->thread_routine_();
  return nullptr;
}

} // namespace xl
