# Utility
Provides common utility for developing cross-platform applications, supporting C++11, 14, 17, 20 and latest.

![](https://github.com/Streamlet/utility/actions/workflows/windows.yml/badge.svg) ![](https://github.com/Streamlet/utility/actions/workflows/winxp.yml/badge.svg) ![](https://github.com/Streamlet/utility/actions/workflows/linux.yml/badge.svg) ![](https://github.com/Streamlet/utility/actions/workflows/macos.yml/badge.svg)
---
[（中文版本见这里）](README_zh.md)

## Feature List

* **Meta**
  * **scope_exit**: A light-weight implement for auto clean up resources when exit scope, like LOKI_ON_BLOCK_EXIT, BOOST_SCOPE_EXIT, or absl::Cleanup, etc.
  * **reflect**: Define a struct that can get or set members by its string names.
* **string**
  * **native_string**: Write uniform _T('char'), _T("string"), class native_string, and _tcs* functions, to use `char` based literal, std::string, str* functions for POSIX (or Windows without `_UNICODE` defined), and `wchar_t` based literal, std::wstring, wcs* functions for Windows with `_UNICODE` defined.
  * **string utility**: string_ref, replace, split, and join, etc.
  * **encoding**: Tranform between native <=> utf-8, utf-8 <=> utf-16
* **crypto**: md5, sha1, sha224, sha256, sha384, sha512. (port from libreSSL)
* **file**
  * **file**: Read/write as binary, as text, as test specifying encoding.
  * **fs**: touch, exists, size, is_dir, unlink, mkdir, mkdirs, rmdir, enum_dir, remove, remove_all, rename, etc.
  * **path**: dirname, filename, basename, extname, join, etc.
  * **zip**: touch, add file, compress folder, read file, extract all, etc.
* **config**
  * **ini**: Section operations (enum, has, add, remove), key-value operations (enum, has, get, set, remove).
  * **json**: Define a struct and dump to or parse from json string. (using yyjson)
  * **xml**: Define a struct and dump to or parse from xml string. (using rapidxml)
* **log**: A light-weight logger, supporting levels and std::stringstream based serializing (no formatter).
* **process**
  * **process utility**: executable_path, pid, tid, start, wait, kill, sleep, etc.
  * **cmdline_options**: Parse argc and argv, or a full command line string, and extract arguments.
* **thread**
  * **synchronous**: Provides event, locker, auto_locker, etc. Like <mutex> and <conditinal_variable>, supports Windows XP.
  * **task_thread**: A thread accepting tasks and executing tasks.
  * **thread**: Like <thread>, supports Windows XP.
* **net**
  * **url**: Extract url parts, RFC 3986 encode and decode.
  * **http**: A light-weight http client, using WinHTTP for window and cURL for POSIX.

## Build and Test

1. **Fetch dependencies**. Run `./fetch_deps` or `fetch_deps.bat` in the root directory of the project. If [srcdep](https://github.com/Streamlet/srcdep) is installed, just run `srcdep`. This step needs python environment and PyYAML library.
2. **Prepare for gn-ninja environment**. Run `build/fetch_binaries.py`, or install gn and ninja to your PATH. See `build/README.md` for details.
3. **Build**. Run `gn gen out` and `ninja -C out`. (or `build/bin/gn gen out` and `build/bin/ninja -C out` if gn and ninja is download through `build/fetch_binaries.py` and not installed to PATH.) This step needs libcurl-dev package on POSIX.
4. **Test**. Run `out/unittest`.
