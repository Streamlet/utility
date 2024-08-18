#!/usr/bin/python

import json
from http.server import HTTPServer, BaseHTTPRequestHandler


class HTTPRequestHandler(BaseHTTPRequestHandler):

    def do_REQUEST(self):

        self.send_response(200)
        self.send_header('EchoServerVersion', '1.0')
        self.send_header('Content-Type', 'application/json')
        self.end_headers()

        response = {
            'version': self.request_version,
            'method': self.command,
            'path': self.path,
            'header': {},
        }
        for h in self.headers:
            response['header'][h] = self.headers[h]
        content_length = self.headers['Content-Length']
        if content_length is not None:
            content = self.rfile.read(int(content_length))
            response['body'] = content.decode('utf-8')

        self.wfile.write(json.dumps(response).encode('utf-8'))

    do_OPTIONS = do_REQUEST
    do_HEADER = do_REQUEST
    do_GET = do_REQUEST
    do_POST = do_REQUEST
    do_PUT = do_REQUEST
    do_DELETE = do_REQUEST
    do_TRACE = do_REQUEST
    do_CONNECT = do_REQUEST


def main():
    server = HTTPServer(('', 8080), HTTPRequestHandler)
    server.serve_forever()


if __name__ == "__main__":
    main()
