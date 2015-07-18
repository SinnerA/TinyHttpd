#ifndef _UTILITY_H_
#define _UTILITY_H_

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
#include <pthread.h>
#include <netinet/tcp.h>
#include <time.h>
#include <sys/stat.h>

using namespace std;

#define DOCROOT 1
#define DOMAIN  2

extern string docroot;
extern string domain;

//---------------------项目实用工具函数---------------------------------------

//得到系统时间
string time_gmt();

//根据http请求包中的url和配置文件中的docroot配置选项构造真正的url
string make_real_url(const string& url);

//测试文件是否存在
inline int is_file_existed(const char *path){
	int ret = open(path, O_RDONLY | O_EXCL);
	close(ret);
	return ret;
}

//获得文件长度
int get_file_length(const char *path);

//获得文件最后修改时间
string get_file_modified_time(const char *path);
 
//初始化全局变量config_keyword_map，必须在使用config_keyword_map前调用此函数
void init_config_keyword_map();

//解析配置文件
int parse_config(const char *path);

//设置文件描述符为非阻塞模式
int set_nonblocking(int fd);

//设置套接字SO_REUSEADDR选项
void set_reuse_addr(int sockfd);

//开启套接字TCP_NODELAY选项，关闭nagle算法
void set_off_tcp_nagle(int sockfd);

//关闭套接字TCP_NODELAY选项，开启nagle算法
void set_on_tcp_nagle(int sockfd);

//开启套接字TCP_CORK选项
void set_on_tcp_cork(int sockfd);

//关闭套接字TCP_CORK选项
void set_off_tcp_cork(int sockfd);

//设置套接字SO_RCVTIMEO选项，接收超时
void set_recv_timeo(int sockfd, int sec, int usec);

//设置套接字SO_SNDTIMEO选项，发送超时
void set_snd_timeo(int sockfd, int sec, int usec);

//--------------------------------系统函数的包裹函数---------------------------
int Socket(int domain, int type, int protocol);
void Listen(int sockfd, int backlog);
void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
struct servent* Getservbyname(const char *name, const char *proto);
int Epoll_create(int size);
void Epoll_ctl(int epollfd, int op, int fd, struct epoll_event *event);
int Epoll_wait(int epollfd, struct epoll_event *events, int maxevents, int timeout);

void *Calloc(size_t n, size_t size);//分配n个长度为size的内存，并自动清零
void *Malloc(size_t size);
void Free(void *ptr);

#endif
