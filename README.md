# selfupdate

[（中文版本见这里）](README_zh.md)

A module used for self update of client programs, and also its server side program.

## Features

* Client SDK
  * Provided in the form of static library, easy to be embedded
  * Update Detecting
  * Downloading
  * Installing
* Server
  * Provided in the form of executable
  * Listening to a port or a unix socket file
  * Providing HTTP service for client sdk to query
  * Providing simple HTTP webpages, to manage versions
