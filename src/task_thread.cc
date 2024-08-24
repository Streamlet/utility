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

#include <xl/task_thread>

namespace xl {

task_thread::task_thread() : thread_(&task_thread::run, this) {
}

task_thread::~task_thread() {
  quit();
  join();
}

bool task_thread::post_task(std::function<void()> &&task) {
  std::unique_lock<std::mutex> lock(mutex_);
  if (quit_) {
    return false;
  }
  tasks_.push(std::move(task));
  cv_.notify_all();
  return true;
}

void task_thread::quit() {
  std::unique_lock<std::mutex> lock(mutex_);
  quit_ = true;
  cv_.notify_all();
}

void task_thread::join() {
  if (thread_.joinable()) {
    thread_.join();
  }
}

void task_thread::run() {
  bool run = true;
  while (run) {
    std::queue<std::function<void()>> tasks;
    {
      std::unique_lock<std::mutex> lock(mutex_);
      cv_.wait(lock, [this] {
        return quit_ || !tasks_.empty();
      });
      if (quit_) {
        break;
      }
      std::swap(tasks, tasks_);
    }

    while (!tasks.empty()) {
      {
        std::unique_lock<std::mutex> lock(mutex_);
        if (quit_) {
          run = false;
          break;
        }
      }
      tasks.front()();
      tasks.pop();
    }
  }
}

} // namespace xl