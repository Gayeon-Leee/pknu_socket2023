#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 50000000

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
                     "<img src=\"Test/test.jpg\"></center></body></html>\r\n";

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
    FILE* fp = fopen("Test/test.jpg", "rb");
    if (fp) {
        fseek(fp, 0, SEEK_END);  // 파일의 끝으로 이동하여 파일 크기 확인
        long fileSize = ftell(fp);  // 파일 크기 계산
        fseek(fp, 0, SEEK_SET);  // 파일 읽기 위치를 파일의 처음으로 이동

        char* fileBuf = malloc(fileSize);  // 파일 크기에 맞는 버퍼 할당
        if (fileBuf == NULL)
            error_handling("malloc() error");

        size_t bytesRead = fread(fileBuf, 1, fileSize, fp);  // 파일 데이터 읽기
        if (bytesRead != fileSize) {
            free(fileBuf);
            fclose(fp);
            error_handling("fread() error");
        }

        // 파일 데이터 전송
        write(clnt_sock, fileBuf, bytesRead);

        free(fileBuf);
        fclose(fp);
    }

    close(clnt_sock);
    close(serv_sock);
    return 0;
}

void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
