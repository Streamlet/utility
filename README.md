# selfupdate

[（中文版本见这里）](README_zh.md)

A library used for self update of client programs, and also its server side program.

## Features

* Client SDK
  * New version query
  * Resumable package downloading
  * Install and delete self
* Server
  * Listen to a port or a unix socket file
  * HTTP service
  * Flexible policy configuration

## Getting Start

1. Run `srcdep` to fetch dependencies. (ref: https://github.com/Streamlet/srcdep)
2. Run `build_thirdparty.py` to compile dependencies.
3. Run `gn gen out` and `ninja -C out` to build this project.
4. Run `src/sample/run_server.py` to launch server. It will make `out/test` and listen to local port 8080.
5. Run `out/test/client/client` to perform an upgrade progress. `out/test/client/client` will be replaced to new version if upgrading succeeded.

## Developing

### Concept

* Client: Client will query new version and download package. If ready, it will copy the Installer to a temporary directory and launches Installer.
* Installer: When the Installer is running, it will first wait for the Client process to exit. Then it install the new package (currently just unpack the package file), delete itself and launch the new Client.
* Server: Provide update info and package file downloading.

The Client and Installer could be either separated executeable, or merged together. The `src/sample` uses merged form.

### Client side

* Include `src/include/selfupdate.h`
* Call `selfupdate::Query` to query new version and process the result.
  * The parameter `query_url` is in form of `http(s)://server.domain/packageName/clientVersion`.
  * The parameter `query_body` should be empty for our server implement.
  * The result field `PackageInfo.has_new_version` indicates whether there is a suitable new version for the current version of the client.
  * If the result field `PackageInfo.force` is true, the Client is expected to do a force upgrade.
* Call `selfupdate::Download` to download package.
  * The parameter `download_progress_monitor` enables Client to show a visible progress to users.
* When downloading accomplished, Call `selfupdate::Install` at a proper time, to perform the upgrade.
  * If the Installer is separated from the Client, pass the path of the Installer through `installer_path`.
  * If the main executable of Client is not in the root directory of the application, pass root directory through `install_location`

### Installer side

* Include `src/include/installer.h`
* Call `selfupdate::IsInstallMode` to check commaned line. If the Installer is merged with the Client, use this function to check whether it it running normally or installing.
* Call `selfupdate::DoInstall` to perform the install progress.

### Server side

* Listening
  * Listening to a local port by command line arguments `--port` if it providing HTTP service directly.
  * Listening to a sock file by command line arguments `--sock` if it acts as an upstream of other web server.
* Configuration
  * Pass root directory of configuration files by command line arguments `--config`. Each `.yaml` or `.yml` file will be read inside the configuration directory, recursively.
  * Configuration file sample: [src/server/config/sample_config.yaml](src/server/config/sample_config.yaml)
* If `--root` is specified, the server will support file downloading inside the root directory. The package file could be provided in this way, if there is no other file service.
