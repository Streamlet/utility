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
#include <process.h>
#include <xl/thread>

namespace xl {

thread::thread(std::function<void()> &&thread_routine) : thread_routine_(std::move(thread_routine)) {
  handle_ = (HANDLE)_beginthreadex(nullptr, 0, run, this, 0, nullptr);
  assert(handle_ != NULL);
}

thread::~thread() {
  assert(!joinable());
  detach();
}

HANDLE thread::native_handle() const {
  return handle_;
}

bool thread::joinable() const {
  return handle_ != NULL && ::WaitForSingleObject(handle_, 0) == WAIT_TIMEOUT;
}

void thread::join() {
  ::WaitForSingleObject(handle_, INFINITE);
  ::CloseHandle(handle_);
  handle_ = nullptr;
}

void thread::detach() {
  if (handle_ != NULL) {
    ::CloseHandle(handle_);
    handle_ = nullptr;
  }
}

unsigned thread::run(void *context) {
  ((thread *)context)->thread_routine_();
  return 0;
}

} // namespace xl
