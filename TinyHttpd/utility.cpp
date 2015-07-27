#include "utility.h"

//�洢�����ļ��ļ�ֵ�ԣ�����key=docroot value=/home/sinnera/desktop/code/webserver
map<string, int> config_keyword_map;

//---------------------------------------��Ŀʵ�ù��ߺ���-----------------------------------
//�õ�ϵͳʱ��
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
	snprintf(buf, sizeof(buf), "%d ", time_now->tm_mday); //��
	str_time += string(buf);
	switch(time_now->tm_mon){                             //��
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

//����http������е�url�������ļ��е�docroot����ѡ���������url
string make_real_url(const string& url){
	string real_url, url2;

	int n = 0;
	
	if((n = url.find(domain, 0)) != string::npos)//url�а���������Ҫ����ɾȥ
		url2 = url.substr(domain.size(), url.size() - domain.size());
	else
		url2 = url;

	if(docroot[docroot.size() - 1] == '/')//������docrootĩβ��'/'
	{
		if(url2[0] == '/')
			real_url = docroot + url2.erase(0, 1);
		else
			real_url = docroot + url2;
	}
	else//������docrootĩβû��'/'
	{
		if(url2[0] == '/')
			real_url = docroot + url2;
		else
			real_url = docroot + '/' + url2;
	}

	return real_url; //real_url = docroot + (url - domain)
}

//����ļ�����
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

//����ļ�����޸�ʱ��
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

//��ʼ��ȫ�ֱ���config_keyword_map��������ʹ��config_keyword_mapǰ���ô˺���
void init_config_keyword_map(){
	config_keyword_map.insert(make_pair("docroot", DOCROOT));
	config_keyword_map.insert(make_pair("domain", DOMAIN));
}

//���������ļ�
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

//�����ļ�������Ϊ������ģʽ
int set_nonblocking(int fd){
	int old_option = fcntl(fd, F_GETFL);
	int new_option = old_option | O_NONBLOCK;
	fcntl(fd, F_SETFL, new_option);
	return old_option;
}

//�����׽���SO_REUSEADDRѡ��
void set_reuse_addr(int sockfd){
	int on = 1;
	int ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if(ret == -1){
		perror("setsockopt: SO_REUSEADDR");
		exit(-1);
	}
}

//�����׽���SO_RCVTIMEOѡ����ճ�ʱ
void set_recv_timeo(int sockfd, int sec, int usec){
 	struct timeval time = {sec, usec};
 	int ret = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &time, sizeof(time));
 	if(ret == -1){
 		perror("setsockopt: SO_RCVTIMEO");
		exit(-1);
 	}
}

//�����׽���SO_SNDTIMEOѡ����ͳ�ʱ
void set_snd_timeo(int sockfd, int sec, int usec){
 	struct timeval time= {sec, usec};
 	int ret = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &time, sizeof(time));
 	if(ret == -1){
 		perror("setsockopt: SO_SNDTIMEO");
		exit(-1);
 	}
}

//----------------------------------ϵͳ�����İ�������-----------------------------------
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
			//�������ǰѼ����׽�������Ϊ�˷�����ģʽ
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
