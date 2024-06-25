#pragma once

#ifdef _WIN32
#include <tchar.h>
#else
#define _T(s) s
#define TCHAR char
#endif

#include <string>

namespace xl {

using native_string = std::basic_string<TCHAR>;
using native_string_stream = std::basic_stringstream<TCHAR>;
#if __cplusplus >= 201703L
using native_string_view = std::basic_string_view<TCHAR>;
#endif

#if __cplusplus >= 201103L
#if defined(_WIN32) && defined(_UNICODE)
template <typename T>
inline native_string to_native_string(T _Val) {
  return std::to_wstring(_Val);
}
#else
inline native_string to_native_string(T _Val) {
  return std::to_string(_Val);
}
#endif
#endif

} // namespace xl

#ifndef _WIN32

// Cross platfoem
#define strnicmp strncasecmp

// Program
#define _tmain main

// Locale-specific Information
#define _tsetlocale setlocale

// Input and Output
#define _tfopen fopen
#define _tfreopen freopen
#define _tremove remove
#define _trename rename

#define _ftprintf fprintf
#define _tprintf printf
#define _stprintf sprintf
#define _vftprintf vfprintf
#define _vtprintf vprintf
#define _vstprintf vsprintf
#define _ftscanf fscanf
#define _tscanf scanf
#define _stscanf sscanf
#define _fgetts fgets
#define _fputts fputs
#define _gettchar getchar
#define _getts gets
#define _puttchar putchar
#define _putts puts
#define _tperror perror

// Utility functions
#define _ttof atof
#define _ttoi atoi
#define _ttol atol
#define _tcstod strtod
#define _tcstol strtol
#define _tcstoul strtoul
#define _tsystem system
#define _tgetenv getenv
#define _tcscpy strcpy
#define _tcsncpy strncpy
#define _tcscat strcat
#define _tcsncat strncat
#define _tcscmp strcmp
#define _tcsncmp strncmp
#define _tcscoll strcoll
#define _tcschr strchr
#define _tcsrchr strrchr
#define _tcsspn strspn
#define _tcscspn strcspn
#define _tcspbrk strpbrk
#define _tcsstr strstr
#define _tcslen strlen
#define _tcserror strerror
#define _tcstok strtok
#define _tcsxfrm strxfrm

// Time and Date functions
#define _tasctime asctime
#define _tctime ctime
#define _tcsftime strftime

#endif
