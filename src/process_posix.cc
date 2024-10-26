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

#include <ctime>
#include <signal.h>
#include <spawn.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <xl/process>
#ifdef __APPLE__
#include <AvailabilityMacros.h>
#include <mach-o/dyld.h>
#include <pthread.h>
#include <sys/errno.h>
#include <sys/param.h>
#include <sys/syscall.h>
#endif

namespace xl {

namespace process {

#ifdef __APPLE__

std::string executable_path() {
  char path[MAXPATHLEN];
  uint32_t size = sizeof(path);
  if (::_NSGetExecutablePath(path, &size) == 0) {
    return path;
  }

  std::string long_path(size - 1, '\0');
  if (::_NSGetExecutablePath(&long_path[0], &size) != 0) {
    return "";
  }

  return long_path;
}

#else

std::string executable_path() {
  const unsigned int SHORT_PATH_LENGTH = 1024, LONG_PATH_LENGTH = 32768;
  char path[SHORT_PATH_LENGTH] = {0};
  const char *PROC_SELF_EXE = "/proc/self/exe";
  ssize_t len = ::readlink(PROC_SELF_EXE, path, SHORT_PATH_LENGTH);
  if (len < SHORT_PATH_LENGTH) {
    path[len] = '\0';
    return path;
  }
  std::string long_path(LONG_PATH_LENGTH, '\0');
  len = ::readlink(PROC_SELF_EXE, &long_path[0], LONG_PATH_LENGTH);
  if (len < LONG_PATH_LENGTH) {
    long_path.resize(len);
    return long_path;
  }
  return "";
}

#endif

std::string current_workdir() {
  char *buffer = ::getcwd(NULL, 0);
  std::string cwd = buffer;
  ::free(buffer);
  return cwd;
}

long pid() {
  return ::getpid();
}

long tid() {
#ifdef __APPLE__
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_12
  uint64_t tid64;
  pthread_threadid_np(nullptr, &tid64);
  return (pid_t)tid64;
#else
  return ::syscall(SYS_thread_selfid);
#endif
#else
  return ::gettid();
#endif
}

long start(const std::string &executable,
           const std::vector<std::string> &arguments,
           const std::string &start_dir,
           unsigned int milliseconds) {
  std::string executable_copied = executable;
  std::vector<std::string> arguments_copied = arguments;
  std::vector<char *> argv;
  argv.push_back(&executable_copied[0]);
  for (auto &argument : arguments_copied) {
    argv.push_back(&argument[0]);
  }
  argv.push_back(NULL);
  std::string cwd = current_workdir();
  if (!start_dir.empty() && start_dir != cwd) {
    chdir(start_dir.c_str());
  }
  pid_t pid = ::vfork();
  if (pid == -1) {
    chdir(cwd.c_str());
    return 0;
  }

  if (pid == 0) {
    int exit_code = ::execvp(executable.c_str(), &argv[0]);
    ::exit(exit_code);
  }

  chdir(cwd.c_str());
  if (milliseconds > 0) {
    wait(pid, milliseconds);
  }

  return pid;
}

bool wait(long pid, unsigned int milliseconds) {
  timespec begin = {};
  ::clock_gettime(CLOCK_MONOTONIC, &begin);
  while (true) {
    errno = 0;
    int status = 0;
    pid_t ret = ::waitpid(pid, &status, WNOHANG);
    if (ret == -1) {
      // if error, return false
      if (errno != EINTR) { // continue waiting for EINTR
        return false;
      }
    } else if (ret != 0) {
      // if exited or signaled, return true
      if (WIFEXITED(status) || WIFSIGNALED(status)) {
        return true;
      }
    }
    // otherwise, waiting if not timeout
    timespec now = {};
    ::clock_gettime(CLOCK_MONOTONIC, &now);
    unsigned int time_elapsed = (now.tv_sec - begin.tv_sec) * 1000 + (now.tv_nsec - begin.tv_nsec) / 1000000;
    if (time_elapsed >= milliseconds) {
      break;
    }
    unsigned int time_left = milliseconds - time_elapsed;
    if (time_left < 1000) {
      timespec ts{0, (time_left % 1000) * 1000000};
      ::nanosleep(&ts, NULL);
    } else {
      ::sleep(1);
    }
  }
  return false;
}

bool kill(long pid) {
  if (::kill(pid, SIGKILL) != 0) {
    return false;
  }
  return true;
}

void sleep(unsigned int milliseconds) {
  timespec ts{milliseconds / 1000, (milliseconds % 1000) * 1000000};
  ::nanosleep(&ts, NULL);
}

} // namespace process

} // namespace xl
