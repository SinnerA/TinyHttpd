#include "parse.h"


//获得header_map里的value
string get_value_from_http_header(const string& key, const header_map& header);


//打印http_header_t里面的header
void print_http_header_header(const header_map& header){
	if(!header.empty()){
		header::const_iterator it = header.begin();
		while(it != header.end()){
			cout << it->first << ": " << it->second << endl;
			++it;
		}
	}
}

//打印http_header_t
void print_http_header(http_header_t* httphdr){
	if(httphdr == NULL){
		printf("print http_header failed\n");
		return;
	}
	cout << httphdr->method << " " << httphdr->url << httphdr->version << endl;
	print_http_header_header(httphdr->header);
	cout << endl;
	cout << httphdr->body << endl;
}

//为http_header_t分配内存
http_header_t *alloc_http_header(){
	http_header_t httphdr = (http_header_t*)new http_header_t;
	if(httphdr == NULL){
		printf("alloc http_header falied\n");
		return 1;
	}
	return httphdr;
}

//回收为http_header_t分配的内存
void free_http_header(http_header_t* httphdr){
	if(httphdr == NULL)
		return;
	delete httphdr;
}

//解析http request，并将其存放在httphdr中
bool parse_http_request(const string& http_request, http_header_t* httphdr){
	if(http_request.empty()){
		printf("parse_http_request: http_request is empty\n");
		return false;
	}
	if(httphdr == NULL){
		printf("parse_http_request: phttphdr is NULL");
		return false;
	}
	
	string crlf("\r\n"), crlfcrlf("\r\n\r\n");
	size_t prev = 0, next = 0;
	
	//解析http请求包的起始行
	if((next = http_request.find(crlf, prev)) != string::nops){
		string first_line(http_request.substr(prev, next - prev));
		prev = next;
	}
}












