#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define PORT 8080
#define BUFFER_SIZE 1024

//Function to determine the Content-Type based on the file extension
const char *get_content_type(const char *file_path) {
    const char *extension  = strrchr(file_path, '.');
    if (extension) {
        if (strcmp(extension, ".html") == 0) return "text/html";
        if (strcmp(extension, ".css") == 0) return "text/css";
        if (strcmp(extension, ".jpg") == 0 || strcmp(extension, ".jpeg") == 0) return "image/jpeg";
        if (strcmp(extension, ".txt") == 0) return "text/plain";
    }
    return "application/octet-stream"; //Default for unknown file types
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);

    if (bytes_received < 0) {
        perror("recv failed");
        close(client_socket);
        return;
    }

    buffer[bytes_received] = '\0';

    //Extract requested file (assumes format "GET /filename HTTP/1.1")
    char method[10], file_path[50];
    sscanf(buffer, "%s %s", method, file_path);

    //Default to "index.html" if root is requested
    if (strcmp(file_path, "/") == 0) {
        strcpy(file_path, "/index.html");
    }

    //Remove leading "/" for file opening
    char file_name[50];
    snprintf(file_name, sizeof(file_name), ".%s", file_path);

    //Open the requested file
    int file_fd = open(file_name, O_RDONLY);
    if (file_fd < 0) {
        //Serve 404.html when a file is not found
        int error_fd = open("./404.html", O_RDONLY);
        if (error_fd < 0) {
            //If 404.html is also missing, send a plain text fallback
            char error_response[] = 
                "HTTP/1.1 404 Not Found\r\n"
                "Content-Length: 13\r\n"
                "Content-Type: text/plain\r\n\r\n"
                "404 Not Found";
            send(client_socket, error_response, strlen(error_response), 0);
        } else {
            //Send 404.html file content
            char header[] = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n";
            send(client_socket, header, strlen(header), 0);

            char file_buffer[BUFFER_SIZE];
            int read_bytes;
            while ((read_bytes = read(error_fd, file_buffer, BUFFER_SIZE)) > 0) {
                send(client_socket, file_buffer, read_bytes, 0);
            }
            close(error_fd);
        } 
    } else {
        //Determine the Content-Type based on the file extension
        const char *content_type = get_content_type(file_path);

        //Send the HTTP header with the correct Content-Typr
        char success_header[BUFFER_SIZE]; 
        snprintf(success_header, sizeof(success_header), 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: %s\r\n\r\n", content_type);
        send(client_socket, success_header, strlen(success_header), 0);

        //Send file content
        char file_buffer[BUFFER_SIZE];
        int read_bytes;
        while ((read_bytes = read(file_fd, file_buffer, BUFFER_SIZE)) > 0) {
            send(client_socket, file_buffer, read_bytes, 0);
        }
        close(file_fd);
    }
    close(client_socket);
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    //Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    //Bind socket to port
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    //Listen for connections
    if (listen(server_fd, 10) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server running on port %d...\n", PORT);

    //Accept client connections
    while ((client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_len)) > 0) {
        if (fork() == 0) { //Create a child process
            close(server_fd); //Child process doesn't need server socket
            handle_client(client_socket);
            exit(0); //Exit child process after handling client
        }
        close(client_socket); //PArent process closes the socket
    }

    //Close server
    close(server_fd);
    return 0;
}