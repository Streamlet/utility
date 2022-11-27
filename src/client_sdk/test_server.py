#!/usr/bin/python
# -*- coding: utf-8 -*-

import os, sys, zipfile, hashlib
from http.server import HTTPServer, BaseHTTPRequestHandler


def make_package(dir):
    if sys.platform == 'win32':
        filename = 'client_sdk_test.exe'
    else:
        filename = 'client_sdk_test'
    src_file = os.path.join(dir, filename)
    package_file = os.path.join(dir, 'client_sdk_test.zip')
    with zipfile.ZipFile(package_file, 'w') as zip:
        zip.write(src_file, filename)

    package_file_size = os.stat(package_file).st_size

    sha256 = hashlib.sha256()
    with open(package_file, 'rb') as f:
        BLOCK_SIZE = 1024 * 1024
        while True:
            buffer = f.read(BLOCK_SIZE)
            sha256.update(buffer)
            if (len(buffer) < BLOCK_SIZE):
                break
    sha256_hash = sha256.hexdigest().lower()

    return package_file, package_file_size, sha256_hash


package_file, package_file_size, sha256_hash = make_package(sys.argv[1])

package_info = '''
{
    "package_name": "selfupdate",
    "package_version": "1.0",
    "package_url": "http://localhost:8080/download",
    "package_size": %d,
    "package_format": "zip",
    "package_hash": {
        "sha256": "%s",
    },
    "update_title": "SelfUpdate 1.0",
    "update_description": "This upgrade is very important!",
}''' % (package_file_size, sha256_hash)


class web_server(BaseHTTPRequestHandler):

    def do_HEAD(self):
        if self.path == '/download':
            self.send_response(200)
            self.send_header("Content-Length", str(package_file_size))
            self.end_headers()
        else:
            self.send_error(404)

    def do_GET(self):
        if self.path == '/query':
            self.send_response(200)
            self.end_headers()
            self.wfile.write(package_info.encode())
        elif self.path == '/download':
            self.send_response(200)
            self.end_headers()
            with open(package_file, 'rb') as f:
                self.wfile.write(f.read())
        else:
            self.send_error(404)


def main():
    httpd = HTTPServer(('localhost', 8080), web_server)
    httpd.serve_forever()


if __name__ == '__main__':
    os.chdir(os.path.dirname(os.path.realpath(__file__)))
    main()
