##Web Server in C

#Overview

This project is a simple web server implemented in C. It listens for HTTP requests on port 8080 and serves static files. The server handles common HTTP requests such as GET and provides appropriate responses, including a custom 404 error page for missing files.

#Features

- Handles HTTP GET requests.
- Serves static files (HTML, CSS, JS, images, etc.).
- Returns a 404 error page if a requested file is not found.
- Uses fork() to handle multiple client connections.
- Supports basic MIME types (HTML, CSS, JavaScript, PNG, JPEG, txt).

#Installation & Compilation

- To compile the web server, use the following command:
gcc -o webserver webserver.c

#Running the Server

- Run the server using:
./webserver

-The server will start listening on port 8080.

#Testing the Server

- Open a browser and navigate to:
http://localhost:8080/

- This should load the index.html file.

- To test the 404 error handling, try accessing a non-existent file:
http://localhost:8080/unknown.html

#Future Enhancements

- Add support for HTTP POST requests.
- Implement logging for incoming requests.
- Introduce multi-threading instead of fork() for better performance.
