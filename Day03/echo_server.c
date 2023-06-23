#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char *message);

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock; // 각각 서버, 클라이언트 소켓에 파일 디스크립터 저장할 변수
	char message[BUF_SIZE];
	int str_len, i;

	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t clnt_adr_sz;

	if(argc != 2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock == -1)
		error_handling("socket() error");

	memset(&serv_adr, 0, sizeof(serv_adr)); // 메모리 초기화 함수.. 구조체 초기화 할 때 주로 사용
	// C는 구조체 자체(내부)에서 초기화 안됨! 그래서 memset으로 밖에 빼서 따로 초기화 하는 것임
	// 참조 - 클래스
	// 클래스 구성요소 : 멤버변수->속성, 멤버함수->기능구현, (생성자)
	// 클래스의 속성은 기본적으로 private.. 외부에서 접근 불가능함. 
	// 클래스 속성에 접근하기 위해 사용하는 것이 메서드(함수)임!
	serv_adr.sin_family = AF_INET; // 구조체의 속성 public이라서 외부에서 접근 가능 한 것
	// 클래스의 속성도 public으로 작성 할 수 있지만 함부로 수정X 위해서 private 으로 함
	// -- 객체 지향의 특성!
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("bind() error");
		// bind() 함수는 서브 소켓에 주소 전달해서  주소 할당 하는 역할

	if(listen(serv_sock, 5) == -1)
		error_handling("listen() error");

	clnt_adr_sz = sizeof(clnt_adr);

	for(i=0; i<5; i++) 
	{
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
		// 클라이언트와 데이터 주고받을 수 있는 소켓을 생성하는게 accept() 함수
		// => for문 안에서 함수 만들어야 반복생성!
		if(clnt_sock == -1)
			error_handling("accept() error");
		else
			printf("Connected client %d \n", i+1);
		while((str_len = read(clnt_sock, message, BUF_SIZE)) != 0)
			write(clnt_sock, message, str_len);	// 오류 없으면 while문으로 반복 전달

		close(clnt_sock);
	}
	close(serv_sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
