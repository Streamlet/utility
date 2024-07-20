#include <chrono>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <map>
#include <xl/encoding>
#include <xl/ini>
#include <xl/log>
#include <xl/log_init>
#include <xl/native_string>
#include <xl/process>
#include <xl/scope_exit>
#include <xl/string>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace xl {

namespace logging {

namespace {

struct GlobalLogContext {
  std::string app_name = "";
  int level = LOG_LEVEL_OFF;
  unsigned int content = LOG_CONTENT_DEFAULT;
  unsigned int target = LOG_TARGET_ALL;
  FILE *log_file = NULL;

  ~GlobalLogContext() {
    if (log_file != NULL) {
      fclose(log_file);
      log_file = NULL;
    }
  }
};

GlobalLogContext log_context_;

static const char *LOG_LEVEL_STRING[] = {"OFF", "FATAL", "ERROR", "WARN", "INFO", "DEBUG"};

void thread_setup(native_string app_name, int level, int content, int target, FILE *log_file) {
  if (log_context_.log_file != NULL) {
    fclose(log_context_.log_file);
    log_context_.log_file = NULL;
  }
#if defined(_WIN32) && defined(_UNICODE)
  log_context_.app_name = std::move(encoding::utf16_to_utf8(app_name));
#else
  log_context_.app_name = std::move(app_name);
#endif
  log_context_.level = level;
  log_context_.content = content;
  log_context_.target = target;
  log_context_.log_file = log_file;
}

template <typename FromCharType, typename ToCharType>
std::basic_string<ToCharType> transform_string(const std::basic_string<FromCharType> &s);

template <>
std::string transform_string(const std::string &s) {
  return s;
}
template <>
std::string transform_string(const std::wstring &s) {
  return encoding::utf16_to_utf8(s);
}

template <>
std::wstring transform_string(const std::string &s) {
  return encoding::utf8_to_utf16(s);
}

template <>
std::wstring transform_string(const std::wstring &s) {
  return s;
}

std::string format(int level, const char *file, const char *function, int line, std::basic_string<char> message) {
  std::basic_stringstream<char> ss;
  const char GROUP_BEGIN = '[';
  const char GROUP_END = ']';
  if ((log_context_.content & LOG_CONTENT_TIME) != 0) {
    ss << GROUP_BEGIN;
    const auto now = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    time_t now_t = std::chrono::system_clock::to_time_t(now);
    ss << std::put_time(std::localtime(&now_t), "%F %T.") << (now.time_since_epoch().count() % 1000);
    ss << GROUP_END;
  }
  if ((log_context_.content & LOG_CONTENT_LEVEL) != 0) {
    ss << GROUP_BEGIN;
    if (level < LOG_LEVEL_OFF) {
      level = LOG_LEVEL_OFF;
    }
    if (level > LOG_LEVEL_DEBUG) {
      level = LOG_LEVEL_DEBUG;
    }
    ss << LOG_LEVEL_STRING[level];
    ss << GROUP_END;
  }
  if ((log_context_.content & LOG_CONTENT_APP_NAME) != 0) {
    ss << GROUP_BEGIN;
    ss << log_context_.app_name;
    ss << GROUP_END;
  }
  if ((log_context_.content & LOG_CONTENT_FULL_FILE_NAME) != 0) {
    ss << GROUP_BEGIN << file << GROUP_END;
  }
  if ((log_context_.content & LOG_CONTENT_FILE_NAME) != 0) {
    ss << GROUP_BEGIN;
    const char *file_name = strrchr(file, '/');
    ss << (file_name == nullptr ? file : file_name + 1);
    ss << GROUP_END;
  }
  if ((log_context_.content & LOG_CONTENT_FULL_FUNC_NAME) != 0) {
    ss << GROUP_BEGIN << function << GROUP_END;
  }
  if ((log_context_.content & LOG_CONTENT_FUNC_NAME) != 0) {
    ss << GROUP_BEGIN;
    const char *func_name = strrchr(function, ':');
    ss << (func_name == nullptr ? function : func_name + 1);
    ss << GROUP_END;
  }
  if ((log_context_.content & LOG_CONTENT_LINE) != 0) {
    ss << GROUP_BEGIN << 'L' << line << GROUP_END;
  }
  if ((log_context_.content & LOG_CONTENT_PID) != 0) {
    ss << GROUP_BEGIN << 'P' << xl::process::pid() << GROUP_END;
  }
  if ((log_context_.content & LOG_CONTENT_TID) != 0) {
    ss << GROUP_BEGIN << 'T' << xl::process::tid() << GROUP_END;
  }
  ss << message.c_str() << std::endl;
  return ss.str();
}

void print(std::string log) {
#ifdef _WIN32
  if ((log_context_.target & LOG_TARGET_DEBUGGER) != 0) {
    std::wstring s = encoding::utf8_to_utf16(log);
    ::OutputDebugStringW(s.c_str());
  }
#endif

  if ((log_context_.target & LOG_TARGET_STDOUT) != 0) {
    printf("%s", log.c_str());
  }

  if ((log_context_.target & LOG_TARGET_FILE) != 0 && log_context_.log_file != NULL) {
    fprintf(log_context_.log_file, "%s", log.c_str());
    fflush(log_context_.log_file);
  }
}

void thread_log(int level, const char *file, const char *function, int line, std::string message) {
  std::string log = format(level, file, function, line, std::move(message));
  print(std::move(log));
}

} // namespace

void log(int level, const char *file, const char *function, int line, std::string message) {
  if (log_context_.level < level || log_context_.target == 0) {
    return;
  }
  thread_log(level, file, function, line, std::move(message));
}

bool setup(const TCHAR *app_name, int level, int content, int target, const TCHAR *log_file) {
  if (level <= LOG_LEVEL_OFF) {
    return false;
  }
  FILE *f = NULL;
  if ((target & LOG_TARGET_FILE) != 0 && log_file != NULL) {
    f = _tfopen(log_file, _T("a"));
    if (f == NULL) {
      return false;
    }
  }
  thread_setup(app_name, level, content, target, f);
  return true;
}

namespace {

string_ref trim_spaces(const string_ref &s) {
  const char *begin = s.c_str();
  const char *end = s.c_str() + s.length();
  bool stop = false;
  while (begin != end && !stop) {
    switch (*begin) {
    case ' ':
    case '\t':
    case '\r':
    case '\n':
      ++begin;
      break;
    default:
      stop = true;
      break;
    }
  }
  stop = false;
  while (begin != end && !stop) {
    switch (*(end - 1)) {
    case ' ':
    case '\t':
    case '\r':
    case '\n':
      --end;
      break;
    default:
      stop = true;
      break;
    }
  }
  return string_ref(begin == end ? nullptr : begin, end - begin);
};

const char *SECTION_LOG = "Log";

const char *KEY_APP_NAME = "AppName";
const char *KEY_LOG_LEVEL = "LogLevel";
const char *KEY_LOG_CONTENT = "LogContent";
const char *KEY_LOG_TARGET = "LogTarget";
const char *KEY_LOG_FILE = "LogFile";

const char *VALUE_LOG_LEVEL_OFF = "Off";
const char *VALUE_LOG_LEVEL_FATAL = "Fatal";
const char *VALUE_LOG_LEVEL_ERROR = "Error";
const char *VALUE_LOG_LEVEL_WARN = "Warn";
const char *VALUE_LOG_LEVEL_INFO = "Info";
const char *VALUE_LOG_LEVEL_DEBUG = "Debug";
const char *VALUE_LOG_LEVEL_DEFAULT = "Default";

const char *VALUE_LOG_CONTENT_TIME = "Time";
const char *VALUE_LOG_CONTENT_LEVEL = "Level";
const char *VALUE_LOG_CONTENT_APP_NAME = "AppName";
const char *VALUE_LOG_CONTENT_FULL_FILE_NAME = "FullFileName";
const char *VALUE_LOG_CONTENT_FILE_NAME = "FileName";
const char *VALUE_LOG_CONTENT_FULL_FUNC_NAME = "FullFuncName";
const char *VALUE_LOG_CONTENT_FUNC_NAME = "FuncName";
const char *VALUE_LOG_CONTENT_LINE = "Line";
const char *VALUE_LOG_CONTENT_PID = "PID";
const char *VALUE_LOG_CONTENT_TID = "TID";
const char *VALUE_LOG_CONTENT_ALL = "All";
const char *VALUE_LOG_CONTENT_DEFAULT = "Default";

const char *VALUE_LOG_TARGET_STDOUT = "StdOut";
const char *VALUE_LOG_TARGET_FILE = "File";
#ifdef _WIN32
const char *VALUE_LOG_TARGET_DEBUGGER = "Debugger";
#endif
const char *VALUE_LOG_TARGET_ALL = "All";
const char *VALUE_LOG_TARGET_DEFAULT = "Default";

void parse_settings(
    const ini &ini_file, native_string &app_name, int &level, int &content, int &target, native_string &log_file) {
  std::string ini_app_name = ini_file.get_value(SECTION_LOG, KEY_APP_NAME);
#if defined(_WIN32) && defined(_UNICODE)
  app_name = encoding::utf8_to_utf16(ini_app_name);
#else
  app_name = std::move(ini_app_name);
#endif

  std::string ini_log_level = ini_file.get_value(SECTION_LOG, KEY_LOG_LEVEL);
  if (ini_log_level == VALUE_LOG_LEVEL_OFF) {
    level = LOG_LEVEL_OFF;
  } else if (ini_log_level == VALUE_LOG_LEVEL_FATAL) {
    level = LOG_LEVEL_FATAL;
  } else if (ini_log_level == VALUE_LOG_LEVEL_ERROR) {
    level = LOG_LEVEL_ERROR;
  } else if (ini_log_level == VALUE_LOG_LEVEL_WARN) {
    level = LOG_LEVEL_WARN;
  } else if (ini_log_level == VALUE_LOG_LEVEL_INFO) {
    level = LOG_LEVEL_INFO;
  } else if (ini_log_level == VALUE_LOG_LEVEL_DEBUG) {
    level = LOG_LEVEL_DEBUG;
  } else if (ini_log_level == VALUE_LOG_LEVEL_DEFAULT) {
    level = LOG_LEVEL_DEFAULT;
  } else {
    // ignore illegal values
  }

  std::string ini_log_content = ini_file.get_value(SECTION_LOG, KEY_LOG_CONTENT);
  content = 0;
  auto contents = string::split_ref(ini_log_content, ",");
  for (auto &ref : contents) {
    ref = trim_spaces(ref);
    if (ref == VALUE_LOG_CONTENT_TIME) {
      content |= LOG_CONTENT_TIME;
    } else if (ref == VALUE_LOG_CONTENT_LEVEL) {
      content |= LOG_CONTENT_LEVEL;
    } else if (ref == VALUE_LOG_CONTENT_APP_NAME) {
      content |= LOG_CONTENT_APP_NAME;
    } else if (ref == VALUE_LOG_CONTENT_FULL_FILE_NAME) {
      content |= LOG_CONTENT_FULL_FILE_NAME;
    } else if (ref == VALUE_LOG_CONTENT_FILE_NAME) {
      content |= LOG_CONTENT_FILE_NAME;
    } else if (ref == VALUE_LOG_CONTENT_FULL_FUNC_NAME) {
      content |= LOG_CONTENT_FULL_FUNC_NAME;
    } else if (ref == VALUE_LOG_CONTENT_FUNC_NAME) {
      content |= LOG_CONTENT_FUNC_NAME;
    } else if (ref == VALUE_LOG_CONTENT_LINE) {
      content |= LOG_CONTENT_LINE;
    } else if (ref == VALUE_LOG_CONTENT_PID) {
      content |= LOG_CONTENT_PID;
    } else if (ref == VALUE_LOG_CONTENT_TID) {
      content |= LOG_CONTENT_TID;
    } else if (ref == VALUE_LOG_CONTENT_DEFAULT) {
      content = LOG_CONTENT_DEFAULT;
    } else if (ref == VALUE_LOG_CONTENT_ALL) {
      content = LOG_CONTENT_ALL;
    } else {
      // ignore illegal values
    }
  }

  std::string ini_log_target = ini_file.get_value(SECTION_LOG, KEY_LOG_TARGET);
  target = 0;
  auto targets = string::split_ref(ini_log_target, ",");
  for (auto &ref : targets) {
    ref = trim_spaces(ref);
    if (ref == VALUE_LOG_TARGET_STDOUT) {
      target |= LOG_TARGET_STDOUT;
    } else if (ref == VALUE_LOG_TARGET_FILE) {
      target |= LOG_TARGET_FILE;
#ifdef _WIN32
    } else if (ref == VALUE_LOG_TARGET_DEBUGGER) {
      target |= LOG_TARGET_DEBUGGER;
#endif
    } else if (ref == VALUE_LOG_TARGET_DEFAULT) {
      target = LOG_TARGET_DEFAULT;
    } else if (ref == VALUE_LOG_TARGET_ALL) {
      target = LOG_TARGET_ALL;
    } else {
      // ignore illegal values
    }
  }

  if ((target & LOG_TARGET_FILE) != 0) {
    std::string ini_log_file = ini_file.get_value(SECTION_LOG, KEY_LOG_FILE);
#if defined(_WIN32) && defined(_UNICODE)
    log_file = encoding::utf8_to_utf16(ini_log_file);
#else
    log_file = std::move(ini_log_file);
#endif
  }
}

} // namespace

bool setup_from_file(const TCHAR *log_setting_file) {
  ini ini_file;
  if (!ini_file.load(log_setting_file)) {
    return false;
  }

  native_string app_name;
  int level = LOG_LEVEL_DEFAULT;
  int content = LOG_CONTENT_DEFAULT;
  int target = LOG_TARGET_ALL;
  native_string log_file;
  parse_settings(ini_file, app_name, level, content, target, log_file);

  return setup(app_name.c_str(), level, content, target, log_file.c_str());
}

} // namespace logging

} // namespace xl
