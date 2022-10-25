# selfupdate

[(Here is the English Version)](README.md)

一个用于客户端程序的自升级模块，以及对应的服务端程序

## 功能

* 客户端 SDK
  * 以静态库形式提供，便于内嵌
  * 升级检测
  * 下载
  * 安装
* 服务端
  * 以可执行文件形式提供
  * 监听端口号或 Unix Socket 文件
  * 提供 HTTP 服务供客户端升级检测
  * 提供简易 HTTP 管理页面用于管理版本配置
