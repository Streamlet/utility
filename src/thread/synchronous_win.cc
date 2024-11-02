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
#include <Windows.h>
#include <xl/synchronous>

namespace xl {

event::event(bool init_signaled, bool auto_reset) {
  HANDLE hEvent = ::CreateEvent(NULL, !auto_reset, init_signaled, NULL);
  assert(hEvent != NULL);
  platform_context_ = hEvent;
}

event::~event() {
  HANDLE hEvent = (HANDLE)platform_context_;
  assert(hEvent != NULL);
  ::CloseHandle(hEvent);
  platform_context_ = nullptr;
}

void event::wait() const {
  HANDLE hEvent = (HANDLE)platform_context_;
  assert(hEvent != NULL);
  DWORD dwWaitResult = ::WaitForSingleObject(hEvent, INFINITE);
  assert(dwWaitResult == WAIT_OBJECT_0);
}

bool event::timed_wait(unsigned long milliseconds) const {
  HANDLE hEvent = (HANDLE)platform_context_;
  assert(hEvent != NULL);
  DWORD dwWaitResult = ::WaitForSingleObject(hEvent, milliseconds);
  assert(dwWaitResult == WAIT_OBJECT_0 || dwWaitResult == WAIT_TIMEOUT);
  return dwWaitResult == WAIT_OBJECT_0;
}

void event::set() const {
  HANDLE hEvent = (HANDLE)platform_context_;
  assert(hEvent != NULL);
  BOOL r = ::SetEvent(hEvent);
  assert(r);
}

void event::reset() const {
  HANDLE hEvent = (HANDLE)platform_context_;
  assert(hEvent != NULL);
  BOOL r = ::ResetEvent(hEvent);
  assert(r);
}

} // namespace xl
