#include <stdio.h>
#include <arpa/inet.h>

int main()
{
	int = 0x12345678;
	char* ptr =(char*)&data;

	for(int i = 0; i < sizeof(int); i++) {
		printf("%d, %x\n");
	}
	return 0;
}
