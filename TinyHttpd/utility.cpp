#include "utility.h"

//存储解析文件的键值对，例如key=docroot value=/home/sinnera/desktop/code/webserver
map<string, int> config_keyword_map;

//---------------------------------------项目实用工具函数-----------------------------------
//得到系统时间
string time_gmt(){
	time_t now;
	struct tm *time_now;
	string str_time;

	time(&now);
	time_now = localtime(&now);

	switch(time_now->tm_wday){
		case 0:
			str_time += "Sun, ";
			break;
		case 1:
			str_time += "Mon, ";
			break;
		case 2:
			str_time += "Tue, ";
			break;
		case 3:
			str_time += "Wed, ";
			break;
		case 4:
			str_time += "Thu, ";
			break;
		case 5:
			str_time += "Fri, ";
			break;
		case 6:
			str_time += "Sat, ";
			break;
	}
	char buf[16];
	snprintf(buf, sizeof(buf), "%d ", time_now->tm_mday); //日
	str_time += string(buf);
	switch(time_now->tm_mon){                             //月
		case 0:
			str_time += "Jan ";
			break;
		case 1:
			str_time += "Feb ";
			break;
		case 2:
			str_time += "Mar ";
			break;
		case 3:
			str_time += "Apr ";
			break;
		case 4:
			str_time += "May ";
			break;
		case 5:
			str_time += "Jun ";
			break;
		case 6:
			str_time += "Jul ";
			break;
		case 7:
			str_time += "Aug ";
			break;
		case 8:
			str_time += "Sep ";
			break;
		case 9:
			str_time += "Oct ";
			break;
		case 10:
			str_time += "Nov ";
			break;
		case 11:
			str_time += "Dec ";
			break;
	}
	snprintf(buf, sizeof(buf), "%d", time_now->tm_year + 1900);
	str_time += string(buf);
	snprintf(buf, sizeof(buf), " %d:%d:%d ", time_now->tm_hour, time_now->tm_min, time_now->tm_sec);
	str_time += string(buf);

	str_time += "GMT";

	return str_time;
}

//根据http请求包中的url和配置文件中的docroot配置选项构造真正的url
string make_real_url(const string& url){
	string real_url, url2;

	int n = 0;
	
	if((n = url.find(domain, 0)) != string::npos)//url中包含域名，要将其删去
		url2 = url.substr(domain.size(), url.size() - domain.size());
	else
		url2 = url;

	if(docroot[docroot.size() - 1] == '/')//配置项docroot末尾有'/'
	{
		if(url2[0] == '/')
			real_url = docroot + url2.erase(0, 1);
		else
			real_url = docroot + url2;
	}
	else//配置项docroot末尾没有'/'
	{
		if(url2[0] == '/')
			real_url = docroot + url2;
		else
			real_url = docroot + '/' + url2;
	}

	return real_url; //real_url = docroot + (url - domain)
}

//获得文件长度
int get_file_length(const char *path){
 	struct stat buf;
 	int ret = stat(path, &buf);
 	if(ret == -1)
 	{
 		perror("get_file_length");
 		exit(-1);
 	}
 	return (int)buf.st_size;
}

//获得文件最后修改时间
string get_file_modified_time(const char *path){
	struct stat buf;
	int ret = stat(path, &buf);
	if(ret == -1)
	{
		perror("get_file_length");
		exit(-1);
	}
	char array[32] = {0};
	snprintf(array, sizeof(array), "%s", ctime(&buf.st_mtime));
	return string(array, array + strlen(array));
}

//初始化全局变量config_keyword_map，必须在使用config_keyword_map前调用此函数
void init_config_keyword_map(){
	config_keyword_map.insert(make_pair("docroot", DOCROOT));
	config_keyword_map.insert(make_pair("domain", DOMAIN));
}

//解析配置文件
int parse_config(const char *path){
	init_config_keyword_map();
	int ret = 0;
	fstream infile(path, fstream::in);
	string line, word;
	if(!infile){
		printf("%s can't open\n", path);
		infile.close();
		return -1;
	}
	
	while(getline(infile, line)){
		stringstream stream(line);
		stream >> word;            //key
		map<string, int>::const_iterator cit = config_keyword_map.find(word);
		if(cit == config_keyword_map.end()){
			printf("can't find keyword\n");
			infile.close();
			return -1;
		}
		switch (cit->second){
			case DOCROOT:
				stream >> docroot; //value
				break;
			case DOMAIN:
				stream >> domain;  //value
				break;
			default :
				infile.close();
				return -1;
		}
	}
	infile.close();
	return 0;
}

//设置文件描述符为非阻塞模式
int set_nonblocking(int fd){
	int old_option = fcntl(fd, F_GETFL);
	int new_option = old_option | O_NONBLOCK;
	fcntl(fd, F_SETFL, new_option);
	return old_option;
}

//设置套接字SO_REUSEADDR选项
void set_reuse_addr(int sockfd){
	int on = 1;
	int ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if(ret == -1){
		perror("setsockopt: SO_REUSEADDR");
		exit(-1);
	}
}

//设置套接字SO_RCVTIMEO选项，接收超时
void set_recv_timeo(int sockfd, int sec, int usec){
 	struct timeval time = {sec, usec};
 	int ret = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &time, sizeof(time));
 	if(ret == -1){
 		perror("setsockopt: SO_RCVTIMEO");
		exit(-1);
 	}
}

//设置套接字SO_SNDTIMEO选项，发送超时
void set_snd_timeo(int sockfd, int sec, int usec){
 	struct timeval time= {sec, usec};
 	int ret = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &time, sizeof(time));
 	if(ret == -1){
 		perror("setsockopt: SO_SNDTIMEO");
		exit(-1);
 	}
}

//----------------------------------系统函数的包裹函数-----------------------------------
int Socket(int domain, int type, int protocol){
	int listen_fd;
	if((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("socket");
		exit(-1);
	}
	return listen_fd;
}

void Listen(int sockfd, int backlog){
	if(listen(sockfd, backlog) == -1){
		perror("listen");
		exit(-1);
	}
}

void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen){
	if(bind(sockfd, addr, addrlen) == -1){
		perror("bind");
		exit(-1);
	}
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen){
	int ret = 0;
	while(1){	
		ret = accept(sockfd, addr, addrlen);
		if(ret > 0){
			break;
		} else if(ret == -1) {
			//由于我们把监听套接字设置为了非阻塞模式
			if(errno != EAGAIN && errno != EPROTO && errno != EINTR && errno != ECONNABORTED){
				perror("accept");
				exit(-1);
			}
		} else {
			continue;
		}
	}
	return ret;
}
struct servent* Getservbyname(const char *name, const char *proto){
	struct servent *pservent;
	if((pservent = getservbyname(name, proto)) == NULL){
		perror("getservbyname");
		exit(-1);
	}
	return pservent;
}

int Epoll_create(int size){
	int epollfd;
	epollfd = epoll_create(size);
	if(-1 == epollfd){
		perror("epoll_create");
		exit(-1);
	}
	return epollfd;
}

void Epoll_ctl(int epfd, int op, int fd, struct epoll_event *event){
	if(epoll_ctl(epfd, op, fd, event) == -1){
		perror("epoll_ctl");
		exit(-1);
	}
}

int Epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout){
	int nfds = epoll_wait(epfd, events, maxevents, timeout);
	if(nfds < 0 && errno != EINTR){
		perror("epoll_wait");
		exit(-1);
	}
	return nfds;
}

void *Calloc(size_t n, size_t size){
	void *ptr = calloc(n, size);
	if(NULL == ptr){
		perror("calloc");
		exit(-1);
	}
	return ptr;
}

void *Malloc(size_t size){
	void *ptr = malloc(size);
	if(NULL == ptr){
		perror("malloc");
		exit(-1);
	}
	return ptr;
}

void Free(void *ptr){
	free(ptr);
}
