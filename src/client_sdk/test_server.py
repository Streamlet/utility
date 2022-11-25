#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
from http.server import HTTPServer, BaseHTTPRequestHandler


class web_server(BaseHTTPRequestHandler):

    def do_GET(self):
        print(self.path)
        if self.path == '/query':
            self.send_response(200)
            self.end_headers()
            self.wfile.write('''
{
    "title": "SampleSoftware",
    "description": "This upgrade is very important!",
    "server_version": "1.0",
    "package_url": "http://localhost:8080/download",
    "package_format": "zip",
}'''.encode())
        elif self.path == '/download':
            self.send_response(200)
            self.end_headers()
        else:
            self.send_error(404)


def main():
    httpd = HTTPServer(('localhost', 8080), web_server)
    httpd.serve_forever()


if __name__ == '__main__':
    os.chdir(os.path.dirname(os.path.realpath(__file__)))
    main()
