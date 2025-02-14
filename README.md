Web Server in C

Overview

This project is a simple web server implemented in C. It listens for HTTP requests on port 8080 and serves static files from the public/ directory. The server handles common HTTP requests such as GET and provides appropriate responses, including a custom 404 error page for missing files.

Features

Handles HTTP GET requests.

Serves static files (HTML, CSS, JS, images, etc.).

Returns a 404 error page if a requested file is not found.

Uses fork() to handle multiple client connections.

Supports basic MIME types (HTML, CSS, JavaScript, PNG, JPEG, MP4).
