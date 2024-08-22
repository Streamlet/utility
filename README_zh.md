# Utility
提供开发跨平台应用程序所需的通用功能，支持 C++11, 14, 17, 20 以及最新语言标准。

![](https://github.com/Streamlet/utility/actions/workflows/windows.yml/badge.svg) ![](https://github.com/Streamlet/utility/actions/workflows/winxp.yml/badge.svg) ![](https://github.com/Streamlet/utility/actions/workflows/linux.yml/badge.svg) ![](https://github.com/Streamlet/utility/actions/workflows/macos.yml/badge.svg)
---
[(Here is the English Version)](README.md)

## 功能列表

* **Meta**
  * **scope_exit**: 一个用于自动清理资源的轻量级工具，类似 LOKI_ON_BLOCK_EXIT、BOOST_SCOPE_EXIT、absl::Cleanup 等。
  * **reflect**: 定义一个结构体，用字符串名字去读写它的成员。
* **string**
  * **native_string**: 书写统一的 _T('char')、_T("string")、class native_string 和 _tcs* 系列函数，实际上在 POSIX 平台以及 Windows 平台（未定义 `_UNICODE` 时）使用基于 `char` 的字面量、std::string、str* 系列函数，在 Windows 平台（定义 `_UNICODE` 时）使用基于 `wchar_t` 的字面量、std::wstring 和 wcs* 系列函数。
  * **string utility**: string_ref、replace、split、and join。
  * **encoding**: 在这些编码之间转换：native <=> utf-8, utf-8 <=> utf-16
* **crypto**: md5、sha1、sha224、sha256、sha384、sha512。（来自 libreSSL）
* **file**
  * **file**: 以二进制、文本、文本指定编码的形式都写文件。
  * **fs**: touch、exists、size、is_dir、unlink、mkdir、mkdirs、rmdir、enum_dir、remove、remove_all、rename 等。
  * **path**: dirname、filename、basename、extname、join 等。
  * **zip**: 创建空压缩包、添加文件、压缩目录、读取压缩包内文件、全部解压等。
* **config**
  * **ini**: 段操作（枚举、是否存在、添加、删除）、键值对操作（枚举、是否存在、读、写、删除）。
  * **json**: 定义一个结构体，从结构体输出到 json 字符串，或者从 json 字符串解析到结构体。（使用 rapidjson）
  * **xml**: 定义一个结构体，从结构体输出到 xml 字符串，或者从 xml 字符串解析到结构体。（使用 rapidjxml）
* **log**: 一个轻量级的日志系统，支持日志级别，以及基于 std::stringstream 的序列化（没有格式处理）。（当前没有使用独立线程）
* **process**
  * **process utility**: executable_path、pid、tid、start、wait、kill、sleep 等，
  * **cmdline_options**: 解析 argc 和 argv，或者从完整的命令行解析，提取出命令行参数。
* **net**
  * **url**: 解析 url 的各个组成部分，以及基于 RFC 3986 的编解码。
  * **http**: 一个轻量的 HTTP 客户端，在 Windows 上使用 WinHTTP，在 POSIX 系统使用 cURL。

## 编译和测试

1. **获取依赖**。在项目根目录运行 `./fetch_deps` 或 `fetch_deps.bat`。如果 [srcdep](https://github.com/Streamlet/srcdep) 已经安装，那么只需要运行 `srcdep` 即可。这一步需要 python 环境以及 PyYAML 库。
2. **准备 gn-ninja 环境**。 运行 `build/fetch_binaries.py`，或者将 gn 和 ninja 所在目录放到 PATH 中。详见 `build/README.md`。
3. **编译**。运行 `gn gen out` 和 `ninja -C out`。（如果 gn 和 ninja 是通过 `build/fetch_binaries.py` 下载的、且并没有安装到 PATH，那么请运行 `build/bin/gn gen out` 和 `build/bin/ninja -C out`。）对于 POSIX 系统，这一步需要 libcurl-dev 包。
4. **测试**。运行 `out/unittest`.
