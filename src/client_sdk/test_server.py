#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
from http.server import HTTPServer, BaseHTTPRequestHandler

package_info = '''
{
    "package_name": "selfupdate",
    "package_version": "1.0",
    "package_url": "https://proof.ovh.net/files/1Mb.dat",
    "package_size": 1048576,
    "package_format": "zip",
    "package_hash": {
        "md5": "d6dd8a9fdac2c00e4d7bc9b0ab2d383d",
        "sha1": "385da5a9ad0382ac3fc8b50fe76c37f04c5907c7",
        "sha256": "92c4ff2dbefdde59ed9f0f3cf89d7ec991c819470b3513dbfac8ee877f805a6e",
    },
    "update_title": "SelfUpdate 1.0",
    "update_description": "This upgrade is very important!",
}'''

package_content = '''
sampple downloaded content
'''


class web_server(BaseHTTPRequestHandler):

    def do_HEAD(self):
        if self.path == '/download':
            self.send_response(200)
            self.send_header("Content-Length", str(len(package_content)))
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
            self.wfile.write(package_content.encode())
        else:
            self.send_error(404)


def main():
    httpd = HTTPServer(('localhost', 8080), web_server)
    httpd.serve_forever()


if __name__ == '__main__':
    os.chdir(os.path.dirname(os.path.realpath(__file__)))
    main()
