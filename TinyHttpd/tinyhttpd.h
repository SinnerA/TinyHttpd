#ifndef _TINY_HTTDP_H_
#define	_TINY_HTTPD_H_

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
#include <sys/sendfile.h>

using namespace std;

typedef struct _epollfd_connfd{
	int epollfd;
	int connfd;
}_epollfd_connfd;

#define MAX_EVENTS   1024 //epoll��������
#deinfe MAX_BACKLOG  100  //�������������

//�����ļ����ֵ
string domain("");
string docroot("");

//--------------------------------MIME����------------------------------
typedef struct mime_node{
	const char* type;
	const char* value;
}mime_node;

mime_node mime[]=
{
	{".html", "text/html"},
	{".xml", "text/xml"},
	{".xhtml", "application/xhtml+xml"},
	{".txt", "text/plain"},
	{".rtf", "application/rtf"},
	{".pdf", "application/pdf"},
	{".word", "application/msword"},
	{".png", "image/png"},
	{".gif", "image/gif"},
	{".jpg", "image/jpeg"},
	{".jpeg", "image/jpeg"},
	{".au", "audio/basic"},
	{".mpeg", "video/mpeg"},
	{".mpg", "video/mpeg"},
	{".avi", "video/x-msvideo"},
	{".gz", "application/x-gzip"},
	{".tar", "application/x-tar"},
	{NULL ,NULL}
};

const char* mime_type2value(const char* type){
	for(int i = 0; mime[i].type != NULL; ++i){
		if(strcmp(type, mime[i].type) == 0)
			return mime[i].value;
	}
	return NULL;
}

//--------------------------------HTTP״̬��-------------------------------
#define CONTINUE 		100	//�յ����������ʼ���֣��ͻ���Ӧ�ü�������

#define OK				200	//�������Ѿ��ɹ���������
#define ACCEPTED		202	//�����ѽ��ܣ���������δ����

#define MOVED			301	//�����URL�����ߣ���ӦӦ�ð���Location URL
#define	FOUND			302	//�����URL��ʱ���ߣ���ӦӦ�ð���Location URL
#define SEEOTHER		303	//���߿ͻ���Ӧ������һ��URL��ȡ��Դ����ӦӦ�ð���Location URL
#define NOTMODIFIED	    304	//��Դδ�����仯

#define BADREQUEST		400	//�ͻ��˷�����һ���쳣����
#define FORBIDDEN		403	//�������ܾ�����
#define NOTFOUND		404	//URLδ�ҵ�


#define ERROR			500	//����������
#define NOIMPLEMENTED	501 //��������֧�ֵ�ǰ��������Ҫ��ĳ������
#define BADGATEWAY		502	//��Ϊ���������ʹ�õķ�����������������Ӧ�������ε���Ч��Ӧ
#define SRVUNAVAILABLE	503 //������Ŀǰ�޷��ṩ������񣬹�һ��ʱ�����Իָ�

char ok[] = "OK";
char badrequest[] = "Bad Request";
char forbidden[] = "Forbidden";
char notfound[] = "Not Found";
char noimplemented[] = "No Implemented";

char *get_state_by_codes(int http_codes); //ͨ��http״̬����״̬

//---------------------------------HTTP��Ӧ�ײ�-----------------------------
#define ACCEPTRANGE_HEAD			"Accpet-Range"
#define	AGE_HEAD 					"Age"
#define	ALLOW_HEAD					"Allow"
#define	CONTENTBASE_HEAD			"Content-Base"
#define	CONTENTLENGTH_HEAD			"Content-Length"
#define	CONTENTLOCATION_HEAD		"Content-Location"
#define	CONTENTRANGE_HEAD			"Content-Range"
#define	CONTENTTYPE_HEAD			"Content-Type"
#define	DATE_HEAD					"Date"
#define	EXPIRES_HEAD				"Expires"
#define	LAST_MODIFIED_HEAD			"Last-Modified"
#define	LOCATION_HEAD 				"Location"
#define	PUBLIC_HEAD				    "Public"
#define RANGE_HEAD 				    "Range"
#define	SERVER_HEAD				    "Server"

#endif