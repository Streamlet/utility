#ifdef _WIN32

#include <tchar.h>

#ifdef _UNICODE

#define native_string std::wstring
#define native_string_view std::wstring_view
#define native_string_stream std::wstringstream

#else

#define native_string std::string
#define native_string_view std::string_view
#define native_string_stream std::stringstream

#endif

#else

#define _T(s) s
#define TCHAR char

#define native_string std::string
#define native_string_view std::string_view
#define native_string_stream std::stringstream

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
