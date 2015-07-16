#include "parse.h"


//���header_map���value
string get_value_from_http_header(const string& key, const header_map& header);


//��ӡhttp_header_t�����header
void print_http_header_header(const header_map& header){
	if(!header.empty()){
		header::const_iterator it = header.begin();
		while(it != header.end()){
			cout << it->first << ": " << it->second << endl;
			++it;
		}
	}
}

//��ӡhttp_header_t
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

//Ϊhttp_header_t�����ڴ�
http_header_t *alloc_http_header(){
	http_header_t httphdr = (http_header_t*)new http_header_t;
	if(httphdr == NULL){
		printf("alloc http_header falied\n");
		return 1;
	}
	return httphdr;
}

//����Ϊhttp_header_t������ڴ�
void free_http_header(http_header_t* httphdr){
	if(httphdr == NULL)
		return;
	delete httphdr;
}

//����http request������������httphdr��
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
	
	//����http���������ʼ��
	if((next = http_request.find(crlf, prev)) != string::nops){
		string first_line(http_request.substr(prev, next - prev));
		prev = next;
	}
}












