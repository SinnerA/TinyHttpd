#include "tinyhttpd.h"

#define ONE_KB		1024
#define ONE_MB	    1024*ONE_KB
#define ONE_GB		1024*ONE_MB

int32_t thread_num = 0;
pthread_mutex_t thread_num_mutex = PTHREAD_MUTEX_INITIALIZER;

void* thread_func(void* args);                            //处理客户端连接的线程函数
void thread_add();                                        //线程数加1
void thread_reduce();                                     //线程数减1
int do_http_header(http_header_t *httphdr, string& reply);//根据解析的http_header_t信息，处理客户请求
char *get_state_by_codes(int http_codes);                 //根据http状态码返回状态

//-----------------------------------------主函数-----------------------------------------
int main(int argc, char* argv[]){
	int                     listenfd;
	int                     sockfd;
	int                     nfds;
	int                     epollfd;
	uint16_t                listen_port;
	struct servent          *pservent;
	struct epoll_event      event;
	struct epoll_event      events[MAX_EVENTS];
	struct sockaddr_in      server_addr;
	struct sockaddr_in      client_addr;
	socklen_t               addrlen;
	pthread_attr_t          pthread_attr_detach;
	_epollfd_connfd         epollfd_connfd;
	pthread_t               pid;
	
	if(argc < 2){
		printf("usage: %s config_path\n", basename(argv[0]));
		return 1;
	}
	
	//配置文件是否存在
	if(!is_file_existed(argv[1])){
		perror("file is not existed\n");
		return 1;
	}
	
	//解析配置文件
	if(parse_config(argv[1])){
		printf("parse config failed\n");
		return 1;
	}
	
	pservent = Getservbyname("http", "tcp");
	listen_port = pservent->s_port;
	
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = listen_port;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);//监听所有地址
	
	Bind(listenfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	Listen(listenfd, MAX_BACKLOG);
	
	set_nonblocking(listenfd); //非阻塞模式
	set_reuse_addr(listenfd);  //设置SO_REUSEADDR选项
	
	epollfd = Epoll_create(MAX_EVENTS);
	
	event.events = EPOLLIN;
	event.data.fd = listenfd;
	Epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &event);
	
	//设置线程属性为detach
	pthread_attr_init(&pthread_attr_detach);
	pthread_attr_setdetachstate(&pthread_attr_detach, PTHREAD_CREATE_DETACHED);
	
	while(1){
		nfds = Epoll_wait(epollfd, events, MAX_EVENTS, -1);
		if(nfds == -1 && errno == EINTR)
			continue;
		
		for(int i = 0; i < nfds; i++){
			if((events[i].data.fd == listenfd) && (events[i].events & EPOLLIN)){
				sockfd = Accept(listenfd, (struct sockaddr*)&client_addr, &addrlen);
				set_nonblocking(sockfd);          //非阻塞模式
				event.events = EPOLLIN | EPOLLET; //ET模式
				event.data.fd = sockfd;
				Epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &event);
			} else {
				epollfd_connfd.epollfd = epollfd;
				epollfd_connfd.connfd = events[i].data.fd;
				
				//epoll不再监听此套接字
				event.data.fd = sockfd;
				Epoll_ctl(epollfd, EPOLL_CTL_DEL, sockfd, &event);
				
				//创建线程处理连接，并设置线程属性为detach
				pthread_create(&pid, &pthread_attr_detach, &thread_func, (void*)&epollfd_connfd);
			}
		}
	}
	pthread_attr_destroy(&pthread_attr_detach);
	close(listenfd);
	return 0;
}

//------------------------------线程处理连接-----------------------------------------
void* thread_func(void* args){
	thread_add();//线程数加1
	
	_epollfd_connfd *p_epollfd_connfd = (_epollfd_connfd*)args;
	int sockfd = p_epollfd_connfd->connfd; //客户连接sockfd
	
	struct epoll_event event, events[2];
	event.events = EPOLLIN | EPOLLET;  //ET模式
	event.data.fd = sockfd;
	
	int epollfd = Epoll_create(2);
	Epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &event);
	
	pthread_t pid = pthread_self();
	printf("thread %u runing......\n", (unsigned int)pid);
	
	char *request_buf = (char*)Malloc(ONE_MB); //1MB缓存存放http请求包
	bzero(request_buf, sizeof(request_buf));
	
	set_recv_timeo(sockfd, 60, 0);     //设置接受超时时间为60秒
	
	int32_t nread = 0, n = 0;
	while(1){
		if((n = read(sockfd, request_buf + nread, ONE_MB - 1)) > 0){
			nread += n;
		} else if (n < 0){
			if(errno == EINTR){
				continue;
			} else if(errno == EAGAIN || errno == EWOULDBLOCK){
				break;
			} else {
				printf("read http request failed\n");
				Free(request_buf);
				break;
			}
		} else {
			break;
		}
	}
	
	if(nread > 0){
		string http_request(request_buf, request_buf + nread);
		http_header_t *httphdr = alloc_http_header();
		
		if(!parse_http_request(http_request, httphdr)){
			printf("parse http request falied\n");
		}
		printf("解析出来的http请求包：\n");
		print_http_header(httphdr);
		
		string http_reply;
		int http_codes = do_http_header(httphdr, http_reply);	
		printf("http响应包：\n%s\n", http_reply.c_str());
		
		char *reply_buf = (char*)Malloc(http_reply.size());
		int i = 0;
		for(; i < http_reply.size(); i++){
			reply_buf[i] = http_reply[i];
		}
		reply_buf[i] = '\0';
		
		int nwrite = 0, n = 0;
		if(http_codes == BADREQUEST || 
		   http_codes == NOIMPLEMENTED || 
		   http_codes == NOTFOUND || 
		   (http_codes == OK && httphdr->method == "HEAD") || 
		   (http_codes == OK && httphdr->method == "GET")
		   )
		{
			while(nwrite < i){
				if((n = write(sockfd, reply_buf + nwrite, i)) > 0){
					nwrite += n;
				} else if(n == 0){
					break;
				} else if(n < 0){
					if(errno == EINTR){
						continue;
					} else {
						printf("write http reply falied\n");
						Free(reply_buf);
						break;
					}
				}
			}
			if(http_codes == OK && httphdr->method == "GET"){
				string real_url = make_real_url(httphdr->url);
				int in_fd = open(real_url.c_str(), O_RDONLY);
				int file_size = get_file_length(real_url.c_str());
				printf("GET the file size is %d\n", file_size);
				printf("send file: %s\n", real_url.c_str());
				nwrite = 0;
				while(sendfile(sockfd, in_fd, (off_t*)&nwrite, file_size) > 0);
				printf("send file success\n");
			}
		}
		free(reply_buf);
		free_http_header(httphdr);
	}
	close(sockfd);
	thread_reduce();
	printf("thread %u stop......\n", (unsigned int)pid);
}

//线程数加1
void thread_add(){
	pthread_mutex_lock(&thread_num_mutex);
	++thread_num;
	pthread_mutex_unlock(&thread_num_mutex);
}

//线程数减1
void thread_reduce(){
	pthread_mutex_lock(&thread_num_mutex);
	--thread_num;
	pthread_mutex_unlock(&thread_num_mutex);
}

//根据解析得到的http_header_t处理客户请求，reply中存放响应包
int do_http_header(http_header_t *httphdr, string& reply){
	char status_line[256] = {0};
	string crlf("\r\n");
	string server("Server: tinyhttpd\r\n");
	string Public("Public: GET, HEAD\r\n");
	string content_base = "Content-Base: " + domain + crlf;
	string date = "Date: " + time_gmt() + crlf;
	string content_length("Content-Length: ");
	string content_location("Content-Location: ");
	string last_modified("Last-Modified: ");
	
	if(httphdr == NULL){
		snprintf(status_line, sizeof(status_line), "HTTP/1.1 %d %s\\r\n",
		        BADREQUEST, get_state_by_codes(BADREQUEST));
		reply = status_line + crlf;
		return BADREQUEST;
	}
	
	string method = httphdr->method;
	string real_url = make_real_url(httphdr->url);
	string version = httphdr->version;
	if(method == "GET" || method == "HEAD"){
		if(is_file_existed(real_url.c_str()) == -1){
			snprintf(status_line, sizeof(status_line), "HTTP/1.1 %d %s\r\n",
			         NOTFOUND, get_state_by_codes(NOTFOUND));
			reply += (status_line + server + date + crlf);
			return NOTFOUND;
		} else {
			int len = get_file_length(real_url.c_str());
			snprintf(status_line, sizeof(status_line), "HTTP/1.1 %d %s\r\n",
			         OK, get_state_by_codes(OK));
			reply += status_line;
			snprintf(status_line, sizeof(status_line), "%d\r\n", len);
			reply += content_length + status_line;
			reply += server + content_base + date;
			reply += last_modified + get_file_modified_time(real_url.c_str());
		}
	} else if(method == "PUT" || method == "DELETE" || method == "POST"){
		snprintf(status_line, sizeof(status_line), "HTTP/1.1 %d %s\r\n",
		         NOIMPLEMENTED, get_state_by_codes(NOIMPLEMENTED));
		reply += status_line + server + Public + date + crlf;
		return NOIMPLEMENTED;
	} else {
		snprintf(status_line, sizeof(status_line), "HTTP/1.1 %d %s\r\n",
				 BADREQUEST, get_state_by_codes(BADREQUEST));
		reply = status_line + crlf;
		return BADREQUEST;
	}
	return OK;
}

//通过http状态码获得状态
char *get_state_by_codes(int http_codes){
	switch(http_codes){
		case OK:
			return ok;
		case BADREQUEST:
			return badrequest;
		case FORBIDDEN:
			return forbidden;
		case NOTFOUND:
			return notfound;
		case NOIMPLEMENTED:
			return noimplemented;
		default:
			break;
	}
	return NULL;
}