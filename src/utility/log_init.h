#pragma once
#include "log.h"

#if defined(_WIN32)
#include <tchar.h>
#else
#define TCHAR char
#endif

namespace logging {

enum LogTarget {
  LOG_TARGET_STDOUT = 0x01,
  LOG_TARGET_FILE = 0x02,
#ifdef _WIN32
  LOG_TARGET_DEBUGGER = 0x04,
#endif

  LOG_TARGET_ALL = -1,
  LOG_TARGET_DEFAULT = LOG_TARGET_ALL,
};

enum LogContent {
  LOG_CONTENT_TIME = 0x01,
  LOG_CONTENT_LEVEL = 0x02,
  LOG_CONTENT_APP_NAME = 0x04,

  LOG_CONTENT_FULL_FILE_NAME = 0x10,
  LOG_CONTENT_FILE_NAME = 0x20,
  LOG_CONTENT_FULL_FUNC_NAME = 0x40,
  LOG_CONTENT_FUNC_NAME = 0x80,
  LOG_CONTENT_LINE = 0x100,

  LOG_CONTENT_PID = 0x1000,
  LOG_CONTENT_TID = 0x2000,

  LOG_CONTENT_ALL = -1,
  LOG_CONTENT_DEFAULT = (LOG_CONTENT_ALL & (~LOG_CONTENT_FULL_FILE_NAME) & (~LOG_CONTENT_FULL_FUNC_NAME)),
};

bool setup(const TCHAR *app_name,
           int level = LOG_LEVEL_DEFAULT,
           int content = LOG_CONTENT_DEFAULT,
           int target = LOG_TARGET_DEFAULT,
           const TCHAR *log_file = NULL);
bool setup_from_file(const TCHAR *log_setting_file);

/**
 * Log Setting File Format
 *
 * It is an INI-like file, without sections, UTF-8 encoded without BOM.
 * Sample:
 *
 * ; texts after semicolon will be recognized as comments
 * AppName    = MyApp   ; AppName defines app_name arguments of setuo_log
 * LogLevel   = Default ; valid values are: Off, Fatal, Error, Warn, Info, Debug or Default. Case insensitive.
 * LogContent = Default ; valid values are: Time, Level, AppName, FileName, FullFileName,
 *                      ;                   FuncName, FullFuncName, Line, PID, TID, All or Default.
 *                      ; Case insensitive, order insensitive. Can be combined by commas.
 * LogTarget  = Default ; valid values are: StdOut, File, Debugger(Windows only), All or Default.
 *                      ; Case insensitive, order insensitive. Can be combined by commas.
 * LogFile    = <Path>  ; If LogTarget contains File, LogFile specifies which file to save the log.
 */

} // namespace logging
