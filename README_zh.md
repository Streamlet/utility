# selfupdate

[(Here is the English Version)](README.md)

一个用于客户端程序的自升级模块，以及对应的服务端程序

## 功能

* 客户端 SDK
  * 新版本查询
  * 支持断点续传
  * 安装并能够自删除
* 服务端
  * 可监听端口号或 Unix Socket 文件
  * 提供 HTTP 服务
  * 灵活的升级策略配置

## 开始使用

1. 运行 `srcdep` 来下载本项目依赖的包。（参见：https://github.com/Streamlet/srcdep）
2. 运行 `build_thirdparty.py` 来编译依赖的包。
3. 运行 `gn gen out` 和 `ninja -C out` 来编译本项目。
4. 运行 `src/sample/run_server.py` 来启动服务器进程。它会创建 `out/test` 并监听本地 8080 端口。
5. 运行 `out/test/client/client` 进行一次升级流程。如果升级成功的话，`out/test/client/client` 会被替换成新版本。

## 开发

### 概念

* 客户端: 客户端会查询新版本并下载新包。然后把安装程序复制到一个临时目录并运行安装程序。
* 安装程序: 安装程序运行的时候，会先等待客户端进程退出，然后安装新包（目前只是解压）、删除自己并运行新的客户端。
* 服务端: 提供升级信息查询以及提供包下载。

客户端和安装程序可以被分离成两个可执行文件，也可以合并成一个可执行文件。`src/sample` 中使用了合并的形式。

### 客户端

* 包含文件 `src/include/selfupdate.h`
* 调用 `selfupdate::Query` 来查询新版本并处理查询结果。
  * 参数 `query_url` 形式为 `http(s)://server.domain/packageName/clientVersion`。
  * 参数 `query_body` 在目前的服务端实现中不使用，请留空。
  * 结果字段 `PackageInfo.has_new_version` 表示针对目前的客户端版本，服务器上是否有合适的新版本。
  * 结果字段 `PackageInfo.force` 如果为 true，表示服务端希望客户端进行强制升级。
* 调用 `selfupdate::Download` 来下载新包。
  * 可以使用参数 `download_progress_monitor` 来给用户展示下载进度。
* 下载完成后，在合适的时机调用 `selfupdate::Install` 进行升级。
  * 如果安装程序和客户端是分离的, 通过 `installer_path` 传入安装程序路径。
  * 如果客户端主程序不在软件根目录，通过 `install_location` 传入根目录。

### 安装程序

* 包含文件 `src/include/installer.h`
* 调用 `selfupdate::IsInstallMode` 来检查命令行参数。当客户端和安装程序合并的时候，用它区分是正常运行还是正在安装。
* 调用 `selfupdate::DoInstall` 进行升级。

### 服务端

* 监听
  * 如果是服务端是直接提供服务的，可用命令行参数 `--port` 来监听一个本地端口。
  * 如果服务端是作为其他网络服务器的上游的，可用命令行参数 `--sock` 监听一个 sock 文件。
* 配置
  * 用命令行参数 `--config` 传入配置文件的根目录，根目录下的每个 `.yaml` 或 `.yml` 文件都会被递归读取。
  * 配置文件样例：[src/server/config/sample_config.yaml](src/server/config/sample_config.yaml)
* 如果通过 `--root` 传入了根目录，服务器将支持此目录中的文件下载。在没有其他文件服务的时候，包文件可以通过这种方式提供出去。