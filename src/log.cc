#include "log.h"
#include "log_init.h"
#include "native_string.h"
#include "process.h"
#include "string.h"
#include <chrono>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <loki/ScopeGuard.h>
#include <map>
#include <string_view>

#ifdef _WIN32
#include "encoding.h"
#include <Windows.h>
#else
#include <strings.h>
#define strnicmp strncasecmp
#endif

namespace xl {

namespace logging {

namespace {

struct GlobalLogContext {
  std::string app_name = "";
#if defined(_WIN32) && defined(_UNICODE)
  std::wstring app_name_w = L"";
#endif
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
static const wchar_t *LOG_LEVEL_WSTRING[] = {L"OFF", L"FATAL", L"ERROR", L"WARN", L"INFO", L"DEBUG"};

void thread_setup(native_string app_name, int level, int content, int target, FILE *log_file) {
  if (log_context_.log_file != NULL) {
    fclose(log_context_.log_file);
    log_context_.log_file = NULL;
  }
#if defined(_WIN32) && defined(_UNICODE)
  log_context_.app_name = std::move(encoding::utf16_to_utf8(app_name));
  log_context_.app_name_w = std::move(app_name);
#else
  log_context_.app_name = std::move(app_name);
#endif
  log_context_.level = level;
  log_context_.content = content;
  log_context_.target = target;
  log_context_.log_file = log_file;
}

template <typename CharType>
std::basic_string<CharType>
format(int level, const CharType *file, const CharType *function, int line, std::basic_string<CharType> message) {
  std::basic_stringstream<CharType> ss;
  const CharType GROUP_BEGIN = (CharType)'[';
  const CharType GROUP_END = (CharType)']';
  if ((log_context_.content & LOG_CONTENT_TIME) != 0) {
    ss << GROUP_BEGIN;
    const auto now = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    time_t now_t = std::chrono::system_clock::to_time_t(now);
#if defined(_WIN32) && defined(_UNICODE)
    if constexpr (std::is_same<CharType, wchar_t>()) {
      ss << std::put_time(std::localtime(&now_t), L"%F %T.") << (now.time_since_epoch().count() % 1000);
    } else {
      ss << std::put_time(std::localtime(&now_t), "%F %T.") << (now.time_since_epoch().count() % 1000);
    }
#else
    ss << std::put_time(std::localtime(&now_t), "%F %T.") << (now.time_since_epoch().count() % 1000);
#endif
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
#if defined(_WIN32) && defined(_UNICODE)
    if constexpr (std::is_same<CharType, wchar_t>()) {
      ss << LOG_LEVEL_WSTRING[level];
    } else {
      ss << LOG_LEVEL_STRING[level];
    }
#else
    ss << LOG_LEVEL_STRING[level];

#endif
    ss << GROUP_END;
  }
  if ((log_context_.content & LOG_CONTENT_APP_NAME) != 0) {
    ss << GROUP_BEGIN;
#if defined(_WIN32) && defined(_UNICODE)
    if constexpr (std::is_same<CharType, wchar_t>()) {
      ss << log_context_.app_name_w.c_str();
    } else {
      ss << log_context_.app_name.c_str();
    }
#else
    ss << log_context_.app_name.c_str();
#endif
    ss << GROUP_END;
  }
  if ((log_context_.content & LOG_CONTENT_FULL_FILE_NAME) != 0) {
    ss << GROUP_BEGIN << file << GROUP_END;
  }
  if ((log_context_.content & LOG_CONTENT_FILE_NAME) != 0) {
    ss << GROUP_BEGIN;
#if defined(_WIN32) && defined(_UNICODE)
    if constexpr (std::is_same<CharType, wchar_t>()) {
      const wchar_t *file_name = wcsrchr(file, L'/');
      ss << (file_name == nullptr ? file : file_name + 1);
    } else {
      const char *file_name = strrchr(file, '/');
      ss << (file_name == nullptr ? file : file_name + 1);
    }
#else
    const char *file_name = strrchr(file, '/');
    ss << (file_name == nullptr ? file : file_name + 1);
#endif
    ss << GROUP_END;
  }
  if ((log_context_.content & LOG_CONTENT_FULL_FUNC_NAME) != 0) {
    ss << GROUP_BEGIN << function << GROUP_END;
  }
  if ((log_context_.content & LOG_CONTENT_FUNC_NAME) != 0) {
    ss << GROUP_BEGIN;
#if defined(_WIN32) && defined(_UNICODE)
    if constexpr (std::is_same<CharType, wchar_t>()) {
      const wchar_t *func_name = wcsrchr(function, L':');
      ss << (func_name == nullptr ? function : func_name + 1);
    } else {
      const char *func_name = strrchr(function, ':');
      ss << (func_name == nullptr ? function : func_name + 1);
    }
#else
    const char *func_name = strrchr(function, ':');
    ss << (func_name == nullptr ? function : func_name + 1);
#endif
    ss << GROUP_END;
  }
  if ((log_context_.content & LOG_CONTENT_LINE) != 0) {
    ss << GROUP_BEGIN << (CharType)'L' << line << GROUP_END;
  }
  if ((log_context_.content & LOG_CONTENT_PID) != 0) {
    ss << GROUP_BEGIN << (CharType)'P' << xl::process::pid() << GROUP_END;
  }
  if ((log_context_.content & LOG_CONTENT_TID) != 0) {
    ss << GROUP_BEGIN << (CharType)'T' << xl::process::tid() << GROUP_END;
  }
  ss << message.c_str() << std::endl;
  return std::move(ss.str());
}

template <typename CharType>
void print(std::basic_string<CharType> log) {
#ifdef _WIN32
  if ((log_context_.target & LOG_TARGET_DEBUGGER) != 0) {
#if defined(_UNICODE)
    if constexpr (std::is_same<CharType, wchar_t>()) {
      ::OutputDebugStringW(log.c_str());
    } else {
      ::OutputDebugStringA(log.c_str());
    }
#else
    ::OutputDebugStringA(log.c_str());
#endif
  }
#endif

  if ((log_context_.target & LOG_TARGET_STDOUT) != 0) {
#if defined(_WIN32) && defined(_UNICODE)
    if constexpr (std::is_same<CharType, wchar_t>()) {
      wprintf(L"%s", log.c_str());
    } else {
      printf("%s", log.c_str());
    }
#else
    printf("%s", log.c_str());
#endif
  }

  if ((log_context_.target & LOG_TARGET_FILE) != 0 && log_context_.log_file != NULL) {
#if defined(_WIN32) && defined(_UNICODE)
    if constexpr (std::is_same<CharType, wchar_t>()) {
      std::string log_utf8 = encoding::utf16_to_utf8(log);
      fprintf(log_context_.log_file, "%s", log_utf8.c_str());
    } else {
      fprintf(log_context_.log_file, "%s", log.c_str());
    }
#else
    fprintf(log_context_.log_file, "%s", log.c_str());
#endif
    fflush(log_context_.log_file);
  }
}

void thread_log(int level, const char *file, const char *function, int line, std::string message) {
  std::string log = format<char>(level, file, function, line, std::move(message));
  print(std::move(log));
}

#if defined(_WIN32) && defined(_UNICODE)

void thread_log(int level, const wchar_t *file, const wchar_t *function, int line, std::wstring message) {
  std::wstring log = format<wchar_t>(level, file, function, line, std::move(message));
  print(std::move(log));
}

#endif

} // namespace

void log(int level, const char *file, const char *function, int line, std::string message) {
  if (log_context_.level < level || log_context_.target == 0) {
    return;
  }
  thread_log(level, file, function, line, std::move(message));
}

#if defined(_WIN32) && defined(_UNICODE)
void log(int level, const wchar_t *file, const wchar_t *function, int line, std::wstring message) {
  if (log_context_.level < level || log_context_.target == 0) {
    return;
  }
  thread_log(level, file, function, line, std::move(message));
}
#endif

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

std::string read_string(const TCHAR *log_setting_file) {
  FILE *f = _tfopen(log_setting_file, _T("r"));
  if (f == NULL) {
    return "";
  }
  LOKI_ON_BLOCK_EXIT(fclose, f);
  if (fseek(f, 0, SEEK_END) != 0) {
    return "";
  }
  long length = ftell(f);
  if (fseek(f, 0, SEEK_SET) != 0) {
    return "";
  }
  std::string s;
  s.resize(length);
  fread(&s[0], 1, length, f);
  return std::move(s);
}
std::string_view trim_spaces(const std::string_view &s) {
  size_t begin = s.find_first_not_of(" \t\r\n");
  size_t end = s.find_last_not_of(" \t\r\n");
  if (begin == s.npos || end == s.npos || begin > end) {
    return std::string_view();
  }
  return std::string_view(s.data() + begin, end - begin + 1);
};

std::map<std::string_view, std::string_view> split_kv(const std::string &s) {
  std::vector<string_ref> lines = string::split_ref(s, "\n");
  std::map<std::string_view, std::string_view> m;
  for (const auto &ref : lines) {
    std::string_view line = (std::string_view)ref;
    size_t semicolon_pos = line.find_first_of(";");
    if (semicolon_pos != line.npos) {
      line = line.substr(0, semicolon_pos);
    }
    size_t equal_pos = line.find_first_of("=");
    if (equal_pos == line.npos) {
      continue;
    }
    std::string_view key = trim_spaces(line.substr(0, equal_pos));
    std::string_view value = trim_spaces(line.substr(equal_pos + 1));
    m.insert(std::make_pair(key, value));
  }
  return std::move(m);
}

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

void parse_settings(const std::map<std::string_view, std::string_view> &kv,
                    native_string &app_name,
                    int &level,
                    int &content,
                    int &target,
                    native_string &log_file) {
  for (const auto &item : kv) {
    const auto &k = item.first;
    const auto &v = item.second;
    if (strnicmp(KEY_APP_NAME, k.data(), k.length()) == 0) {
#if defined(_WIN32) && defined(_UNICODE)
      app_name = encoding::utf8_to_utf16(v.data(), v.length());
#else
      app_name.assign(v.data(), v.length());
#endif
    } else if (strnicmp(KEY_LOG_LEVEL, k.data(), k.length()) == 0) {
      auto l = trim_spaces(v);
      if (strnicmp(VALUE_LOG_LEVEL_OFF, l.data(), l.length()) == 0) {
        level = LOG_LEVEL_OFF;
      } else if (strnicmp(VALUE_LOG_LEVEL_FATAL, l.data(), l.length()) == 0) {
        level = LOG_LEVEL_FATAL;
      } else if (strnicmp(VALUE_LOG_LEVEL_ERROR, l.data(), l.length()) == 0) {
        level = LOG_LEVEL_ERROR;
      } else if (strnicmp(VALUE_LOG_LEVEL_WARN, l.data(), l.length()) == 0) {
        level = LOG_LEVEL_WARN;
      } else if (strnicmp(VALUE_LOG_LEVEL_INFO, l.data(), l.length()) == 0) {
        level = LOG_LEVEL_INFO;
      } else if (strnicmp(VALUE_LOG_LEVEL_DEBUG, l.data(), l.length()) == 0) {
        level = LOG_LEVEL_DEBUG;
      } else if (strnicmp(VALUE_LOG_LEVEL_DEFAULT, l.data(), l.length()) == 0) {
        level = LOG_LEVEL_DEFAULT;
      } else {
        // ignore illegal values
      }
    } else if (strnicmp(KEY_LOG_CONTENT, k.data(), k.length()) == 0) {
      content = 0;
      auto contents = string::split_ref(v.data(), ",", 0, v.length());
      for (const auto &ref : contents) {
        std::string_view c = (std::string_view)ref;
        c = trim_spaces(c);
        if (strnicmp(VALUE_LOG_CONTENT_TIME, c.data(), c.length()) == 0) {
          content |= LOG_CONTENT_TIME;
        } else if (strnicmp(VALUE_LOG_CONTENT_LEVEL, c.data(), c.length()) == 0) {
          content |= LOG_CONTENT_LEVEL;
        } else if (strnicmp(VALUE_LOG_CONTENT_APP_NAME, c.data(), c.length()) == 0) {
          content |= LOG_CONTENT_APP_NAME;
        } else if (strnicmp(VALUE_LOG_CONTENT_FULL_FILE_NAME, c.data(), c.length()) == 0) {
          content |= LOG_CONTENT_FULL_FILE_NAME;
        } else if (strnicmp(VALUE_LOG_CONTENT_FILE_NAME, c.data(), c.length()) == 0) {
          content |= LOG_CONTENT_FILE_NAME;
        } else if (strnicmp(VALUE_LOG_CONTENT_FULL_FUNC_NAME, c.data(), c.length()) == 0) {
          content |= LOG_CONTENT_FULL_FUNC_NAME;
        } else if (strnicmp(VALUE_LOG_CONTENT_FUNC_NAME, c.data(), c.length()) == 0) {
          content |= LOG_CONTENT_FUNC_NAME;
        } else if (strnicmp(VALUE_LOG_CONTENT_LINE, c.data(), c.length()) == 0) {
          content |= LOG_CONTENT_LINE;
        } else if (strnicmp(VALUE_LOG_CONTENT_PID, c.data(), c.length()) == 0) {
          content |= LOG_CONTENT_PID;
        } else if (strnicmp(VALUE_LOG_CONTENT_TID, c.data(), c.length()) == 0) {
          content |= LOG_CONTENT_TID;
        } else if (strnicmp(VALUE_LOG_CONTENT_DEFAULT, c.data(), c.length()) == 0) {
          content = LOG_CONTENT_DEFAULT;
        } else if (strnicmp(VALUE_LOG_CONTENT_ALL, c.data(), c.length()) == 0) {
          content = LOG_CONTENT_ALL;
        } else {
          // ignore illegal values
        }
      }
    } else if (strnicmp(KEY_LOG_TARGET, k.data(), k.length()) == 0) {
      target = 0;
      auto targets = string::split_ref(v.data(), ",", 0, v.length());
      for (const auto &ref : targets) {
        std::string_view t = (std::string_view)ref;
        t = trim_spaces(t);
        if (strnicmp(VALUE_LOG_TARGET_STDOUT, t.data(), t.length()) == 0) {
          target |= LOG_TARGET_STDOUT;
        } else if (strnicmp(VALUE_LOG_TARGET_FILE, t.data(), t.length()) == 0) {
          target |= LOG_TARGET_FILE;
#ifdef _WIN32
        } else if (strnicmp(VALUE_LOG_TARGET_DEBUGGER, t.data(), t.length()) == 0) {
          target |= LOG_TARGET_DEBUGGER;
#endif
        } else if (strnicmp(VALUE_LOG_TARGET_DEFAULT, t.data(), t.length()) == 0) {
          target = LOG_TARGET_DEFAULT;
        } else if (strnicmp(VALUE_LOG_TARGET_ALL, t.data(), t.length()) == 0) {
          target = LOG_TARGET_ALL;
        } else {
          // ignore illegal values
        }
      }
    } else if (strnicmp(KEY_LOG_FILE, k.data(), k.length()) == 0) {
#if defined(_WIN32) && defined(_UNICODE)
      log_file = encoding::utf8_to_utf16(v.data(), v.length());
#else
      log_file.assign(v.data(), v.length());
#endif
    } else {
      // ignore illegal keys
    }
  }
  if ((target & LOG_TARGET_FILE) == 0 && !log_file.empty()) {
    log_file.clear();
  }
}

} // namespace

bool setup_from_file(const TCHAR *log_setting_file) {
  std::string s = read_string(log_setting_file);
  if (s.empty()) {
    return false;
  }
  std::map<std::string_view, std::string_view> kv = split_kv(s);
  if (kv.empty()) {
    return false;
  }

  native_string app_name;
  int level = LOG_LEVEL_DEFAULT;
  int content = LOG_CONTENT_DEFAULT;
  int target = LOG_TARGET_ALL;
  native_string log_file;
  parse_settings(kv, app_name, level, content, target, log_file);

  return setup(app_name.c_str(), level, content, target, log_file.c_str());
}

} // namespace logging

} // namespace xl
