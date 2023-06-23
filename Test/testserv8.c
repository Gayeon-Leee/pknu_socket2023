#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8000
#define MAX_BUFFER_SIZE 1024

// 웹 페이지 배열
char webpage[] =
    "HTTP/1.1 200 OK\r\n"
    "Server: Linux Web Server\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n\r\n"
    "<!DOCTYPE html>\r\n"
    "<html><head><title>My Web Page</title>\r\n"
    "<style>body {background-color: #FFFF00;}</style></head>\r\n"
    "<body><center><h1>Hello world!!</h1><br>\r\n"
    "<img src=\"test.jpeg\"></center></body></html>\r\n";

// 이미지 파일 읽기
char* read_image_file(const char* filename, long* file_size) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error: failed to open image file");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    *file_size = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(*file_size);
    if (buffer == NULL) {
        perror("Error: failed to allocate memory");
        exit(1);
    }

    size_t result = fread(buffer, 1, *file_size, file);
    if (result != *file_size) {
        perror("Error: failed to read image file");
        exit(1);
    }

    fclose(file);

    return buffer;
}

// HTTP 응답 전송 함수
void send_response(int client_socket, const char* response, long response_size) {
    send(client_socket, response, response_size, 0);
}

// GET 요청 처리 함수
void handle_get(int client_socket) {
    send_response(client_socket, webpage, strlen(webpage));
}

// 이미지 파일 전송 처리 함수
void handle_image(int client_socket) {
    long image_size;
    char* image_data = read_image_file("test.jpeg", &image_size);

    char response_header[MAX_BUFFER_SIZE];
    sprintf(response_header, "HTTP/1.1 200 OK\r\n"
                             "Server: Linux Web Server\r\n"
                             "Content-Type: image/jpeg\r\n"
                             "Content-Length: %ld\r\n\r\n",
                             image_size);

    send_response(client_socket, response_header, strlen(response_header));
    send_response(client_socket, image_data, image_size);

    free(image_data);
}

// 클라이언트 요청 처리 함수
void handle_request(int client_socket) {
    char request[MAX_BUFFER_SIZE];
    recv(client_socket, request, sizeof(request), 0);

    if (strstr(request, "GET /test.jpeg") != NULL) {
        handle_image(client_socket);
    } else {
        handle_get(client_socket);
    }

    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;

    // 소켓 생성
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error: socket creation failed");
        exit(1);
    }

    // 서버 주소 설정
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // 소켓과 서버 주소를 바인딩
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error: bind failed");
        exit(1);
    }

    // 클라이언트의 접속 대기
    if (listen(server_socket, 10) == -1) {
        perror("Error: listen failed");
        exit(1);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        // 클라이언트 접속 수락
        addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size);
        if (client_socket == -1) {
            perror("Error: accept failed");
            exit(1);
        }

        printf("Client connected\n");

        // 클라이언트 요청 처리
        handle_request(client_socket);
    }

    // 소켓 닫기
    close(server_socket);

    return 0;
}
