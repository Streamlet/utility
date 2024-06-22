#pragma once

#include <memory>
#include <sstream>

#define LOG_LEVEL_OFF 0
#define LOG_LEVEL_FATAL 1
#define LOG_LEVEL_ERROR 2
#define LOG_LEVEL_WARN 3
#define LOG_LEVEL_INFO 4
#define LOG_LEVEL_DEBUG 5

#ifdef _DEBUG
#define LOG_LEVEL_DEFAULT LOG_LEVEL_DEBUG
#else
#define LOG_LEVEL_DEFAULT LOG_LEVEL_INFO
#endif

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_DEFAULT
#endif

namespace logging {

void log(int level, const char *file, const char *function, int line, std::string message);
#ifdef _WIN32
void log(int level, const wchar_t *file, const wchar_t *function, int line, std::wstring message);
#endif

template <typename CharType, typename Tn>
inline void log_va(std::basic_stringstream<CharType> &ss,
                   int level,
                   const CharType *file,
                   const CharType *function,
                   int line,
                   Tn argn) {
  ss << argn;
  log(level, file, function, line, std::move(ss.str()));
}

template <typename CharType, typename T0, typename... T>
inline void log_va(std::basic_stringstream<CharType> &ss,
                   int level,
                   const CharType *file,
                   const CharType *function,
                   int line,
                   T0 arg0,
                   T... rest) {
  if constexpr (std::is_same<CharType, char>()) {
    static_assert(!std::is_same<T0, const wchar_t *>(), "LOG_* macro only support narrow char strings. If you want "
                                                        "to log wide char strings, please use WLOG_* macros.");
  } else if constexpr (std::is_same<CharType, wchar_t>()) {
    static_assert(!std::is_same<T0, const char *>(), "WLOG_* macro only support wide char strings. If you want to "
                                                     "log narrow char strings, please use LOG_* macros.");
  }
  ss << arg0;
  log_va(ss, level, file, function, line, std::forward<T>(rest)...);
}

template <typename... T>
inline void log_va(int level, const char *file, const char *function, int line, T... args) {
  std::stringstream ss;
  log_va<char>(ss, level, file, function, line, std::forward<T>(args)...);
}

#ifdef _WIN32
template <typename... T>
inline void wlog_va(int level, const wchar_t *file, const wchar_t *function, int line, T... args) {
  std::wstringstream ss;
  log_va<wchar_t>(ss, level, file, function, line, std::forward<T>(args)...);
}

#define __W(x) L##x
#define _W(x) __W(x)

#endif

} // namespace logging

#define LOG(level, ...) logging::log_va(level, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#ifdef _WIN32
#define WLOG(level, ...) logging::wlog_va(level, _W(__FILE__), _W(__FUNCTION__), __LINE__, __VA_ARGS__)
#endif

#if defined(_WIN32) && defined(_UNICODE)
#define TLOG(level, ...) WLOG(level, __VA_ARGS__)
#else
#define TLOG(level, ...) LOG(level, __VA_ARGS__)
#endif

#if (LOG_LEVEL >= LOG_LEVEL_FATAL)
#define LOG_FATAL(...) LOG(LOG_LEVEL_FATAL, __VA_ARGS__)
#define TLOG_FATAL(...) TLOG(LOG_LEVEL_FATAL, __VA_ARGS__)
#ifdef _WIN32
#define WLOG_FATAL(...) WLOG(LOG_LEVEL_FATAL, __VA_ARGS__)
#endif
#else
#define LOG_FATAL(...)
#define TLOG_FATAL(...)
#ifdef _WIN32
#define WLOG_FATAL(...)
#endif
#endif

#if (LOG_LEVEL >= LOG_LEVEL_ERROR)
#define LOG_ERROR(...) LOG(LOG_LEVEL_ERROR, __VA_ARGS__)
#define TLOG_ERROR(...) TLOG(LOG_LEVEL_ERROR, __VA_ARGS__)
#ifdef _WIN32
#define WLOG_ERROR(...) WLOG(LOG_LEVEL_ERROR, __VA_ARGS__)
#endif
#else
#define LOG_ERROR(...)
#define TLOG_ERROR(...)
#ifdef _WIN32
#define WLOG_ERROR(...)
#endif
#endif

#if (LOG_LEVEL >= LOG_LEVEL_WARN)
#define LOG_WARN(...) LOG(LOG_LEVEL_WARN, __VA_ARGS__)
#define TLOG_WARN(...) TLOG(LOG_LEVEL_WARN, __VA_ARGS__)
#ifdef _WIN32
#define WLOG_WARN(...) WLOG(LOG_LEVEL_WARN, __VA_ARGS__)
#endif
#else
#define LOG_WARN(...)
#define TLOG_WARN(...)
#ifdef _WIN32
#define WLOG_WARN(...)
#endif
#endif

#if (LOG_LEVEL >= LOG_LEVEL_INFO)
#define LOG_INFO(...) LOG(LOG_LEVEL_INFO, __VA_ARGS__)
#define TLOG_INFO(...) TLOG(LOG_LEVEL_INFO, __VA_ARGS__)
#ifdef _WIN32
#define WLOG_INFO(...) WLOG(LOG_LEVEL_INFO, __VA_ARGS__)
#endif
#else
#define LOG_INFO(...)
#define TLOG_INFO(...)
#ifdef _WIN32
#define WLOG_INFO(...)
#endif
#endif

#if (LOG_LEVEL >= LOG_LEVEL_DEBUG)
#define LOG_DEBUG(...) LOG(LOG_LEVEL_DEBUG, __VA_ARGS__)
#define TLOG_DEBUG(...) TLOG(LOG_LEVEL_DEBUG, __VA_ARGS__)
#ifdef _WIN32
#define WLOG_DEBUG(...) WLOG(LOG_LEVEL_DEBUG, __VA_ARGS__)
#endif
#else
#define LOG_DEBUG(...)
#define TLOG_DEBUG(...)
#ifdef _WIN32
#define WLOG_DEBUG(...)
#endif
#endif
