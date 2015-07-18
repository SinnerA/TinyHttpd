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

//---------------------��Ŀʵ�ù��ߺ���---------------------------------------

//�õ�ϵͳʱ��
string time_gmt();

//����http������е�url�������ļ��е�docroot����ѡ���������url
string make_real_url(const string& url);

//�����ļ��Ƿ����
inline int is_file_existed(const char *path){
	int ret = open(path, O_RDONLY | O_EXCL);
	close(ret);
	return ret;
}

//����ļ�����
int get_file_length(const char *path);

//����ļ�����޸�ʱ��
string get_file_modified_time(const char *path);
 
//��ʼ��ȫ�ֱ���config_keyword_map��������ʹ��config_keyword_mapǰ���ô˺���
void init_config_keyword_map();

//���������ļ�
int parse_config(const char *path);

//�����ļ�������Ϊ������ģʽ
int set_nonblocking(int fd);

//�����׽���SO_REUSEADDRѡ��
void set_reuse_addr(int sockfd);

//�����׽���TCP_NODELAYѡ��ر�nagle�㷨
void set_off_tcp_nagle(int sockfd);

//�ر��׽���TCP_NODELAYѡ�����nagle�㷨
void set_on_tcp_nagle(int sockfd);

//�����׽���TCP_CORKѡ��
void set_on_tcp_cork(int sockfd);

//�ر��׽���TCP_CORKѡ��
void set_off_tcp_cork(int sockfd);

//�����׽���SO_RCVTIMEOѡ����ճ�ʱ
void set_recv_timeo(int sockfd, int sec, int usec);

//�����׽���SO_SNDTIMEOѡ����ͳ�ʱ
void set_snd_timeo(int sockfd, int sec, int usec);

//--------------------------------ϵͳ�����İ�������---------------------------
int Socket(int domain, int type, int protocol);
void Listen(int sockfd, int backlog);
void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
struct servent* Getservbyname(const char *name, const char *proto);
int Epoll_create(int size);
void Epoll_ctl(int epollfd, int op, int fd, struct epoll_event *event);
int Epoll_wait(int epollfd, struct epoll_event *events, int maxevents, int timeout);

void *Calloc(size_t n, size_t size);//����n������Ϊsize���ڴ棬���Զ�����
void *Malloc(size_t size);
void Free(void *ptr);

#endif
