#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <strings.h>
#include <string>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <utility>
#include <fstream>
#include <sstream>
#include <map>
#include <iostream>
#include <string.h>
#include <arpa/inet.h>
#include <iostream>
#include <assert.h>

using namespace std;

int main(int argc, char *argv[]){
	if(argc < 2){
		printf("usage: %s ip_address\n", basename(argv[0]));
		return 1;
	}
	
	const char* ip = argv[1];
	printf("ip is %s\n", ip);
	
	int ret = 0;
	struct sockaddr_in server_address;
	bzero(&server_address, sizeof(server_address));
	server_address.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &server_address.sin_addr);
	server_address.sin_port = htons(80);
	
	int sockfd = socket(PF_INET, SOCK_STREAM, 0);
	assert(sockfd >= 0);
	
	ret = connect(sockfd, (struct sockaddr*)&server_address, sizeof(server_address));
	if(ret < 0) printf("connect to server failed\n");
	
	printf("发送:\n");
	char writeBuf[] = "GET www.baidu.com/index/index.html HTTP/1.1\r\nAccept-Language: zh-cn\r\n\r\n";
	write(sockfd, writeBuf, sizeof(writeBuf));
	printf("发送结束\n");
	
	printf("开始读取\n");
	char readBuf[102400];
	int index = 0;
	while((ret = read(sockfd, readBuf + index, sizeof(readBuf))) > 0){
		printf("read %d bytes\n", ret);
		index = index + ret;
		if(index >= 10240)
			break;
	}
	readBuf[index] = '\0';
	printf("服务器返回的数据：%d bytes\n%s\n", index, readBuf);
	printf("读取结束\n");
	return 0;
}
