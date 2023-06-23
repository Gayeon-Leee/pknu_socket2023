#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <fcntl.h>

void error_handling(char *message);

int main(int argc, char *argv[]) {
    int serv_sock;
    int clnt_sock;

    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;

    char webpage[] = "HTTP/1.1 200 OK\r\n"
                     "Server: Linux Web Server\r\n"
                     "Content-Type: text/html; charset=UTF-8\r\n\r\n"
                     "<!DOCTYPE html>\r\n"
                     "<html><head><title>My Web Page</title>\r\n"
                     "<style>body {background-color: #FFFF00;}</style></head>\r\n"
                     "<body><center><h1>Hello world!!</h1><br>\r\n"
                     "<img src=\"test.jpeg\"></center></body></html>\r\n";

    if (argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error");

    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    clnt_addr_size = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    if (clnt_sock == -1)
        error_handling("accept() error");

    // 웹 페이지 응답 전송
    write(clnt_sock, webpage, sizeof(webpage) - 1);

    // 이미지 파일 읽어서 전송
    int file_fd = open("test.jpeg", O_RDONLY);
    if (file_fd == -1)
        error_handling("open() error");

    struct stat file_stat;
    if (fstat(file_fd, &file_stat) == -1) {
        close(file_fd);
        error_handling("fstat() error");
    }

    // 이미지 파일의 Content-Type 설정
    char header[256];
    sprintf(header, "HTTP/1.1 200 OK\r\n"
                    "Server: Linux Web Server\r\n"
                    "Content-Type: image/jpeg\r\n"
                    "Content-Length: %ld\r\n\r\n"
										"<img src=\"test.jpeg\">", file_stat.st_size);

    // 헤더 전송
    write(clnt_sock, header, strlen(header));

    // 파일 데이터 전송
    off_t offset = 0;
    ssize_t sent_bytes = sendfile(clnt_sock, file_fd, &offset, file_stat.st_size);
    if (sent_bytes == -1) {
        close(file_fd);
        error_handling("sendfile() error");
    }

    if (sent_bytes != file_stat.st_size) {
        close(file_fd);
        error_handling("sendfile() error: incomplete transfer");
    }

    close(file_fd);
    close(clnt_sock);
    close(serv_sock);
    return 0;
}

void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
