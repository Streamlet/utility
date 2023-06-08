#!/usr/bin/python
# -*- coding: utf-8 -*-

import os, sys, shutil, zipfile, hashlib
from http.server import HTTPServer, BaseHTTPRequestHandler

if sys.platform == 'win32':
    old_filename = 'old_client.exe'
    new_filename = 'new_client.exe'
else:
    old_filename = 'old_client'
    new_filename = 'new_client'

def prepare(dir):
    src_file = os.path.join(dir, old_filename)
    target_dir = os.path.join(dir, 'test')
    if not os.path.exists(target_dir):
        os.makedirs(target_dir)
    if os.path.exists(os.path.join(target_dir, new_filename)):
        os.unlink(os.path.join(target_dir, new_filename))
    shutil.copy(src_file, target_dir)


def make_package(dir):
    src_file = os.path.join(dir, new_filename)
    package_file = os.path.join(dir, 'client_test.zip')
    with zipfile.ZipFile(package_file, 'w') as zip:
        zip.write(src_file, new_filename)

    package_file_size = os.stat(package_file).st_size

    sha256 = hashlib.sha256()
    with open(package_file, 'rb') as f:
        BLOCK_SIZE = 1024 * 1024
        while True:
            buffer = f.read(BLOCK_SIZE)
            if (buffer is None or len(buffer) == 0):
                break
            sha256.update(buffer)
    sha256_hash = sha256.hexdigest().lower()

    return package_file, package_file_size, sha256_hash

dir = sys.argv[1]
prepare(dir)
package_file, package_file_size, sha256_hash = make_package(dir)
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

class WebServer(BaseHTTPRequestHandler):
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


httpd = HTTPServer(('localhost', 8080), WebServer)
httpd.serve_forever()
