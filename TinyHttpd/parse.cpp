#include "parse.h"


//���header_map���value
string get_value_from_http_header(const string& key, const header_map& header);


//��ӡhttp_header_t�����header
void print_http_header_header(const header_map& header){
	if(!header.empty()){
		map<string, string>::const_iterator it = header.begin();
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
	cout << httphdr->method << " " << httphdr->url << " " << httphdr->version << endl;
	print_http_header_header(httphdr->header);
	cout << endl;
	cout << httphdr->body << endl;
}

//Ϊhttp_header_t�����ڴ�
http_header_t *alloc_http_header(){
	http_header_t *httphdr = (http_header_t*)new http_header_t();
	if(httphdr == NULL){
		printf("alloc http_header falied\n");
		exit(-1);
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
	if((next = http_request.find(crlf, prev)) != string::npos){
		string first_line(http_request.substr(prev, next - prev));
		prev = next;
		stringstream sstream(first_line);
		sstream >> (httphdr->method);
		sstream >> (httphdr->url);
		sstream >> (httphdr->version);
	} else {
		perror("parse_http_request: http_request has not a \\r\\n");
		return false;
	}
	
	//����"\r\n\r\n"��λ��
	size_t pos_crlfcrlf = http_request.find(crlfcrlf, prev);
	if(pos_crlfcrlf == string::npos){
		perror("parse_http_request: http_request has not a \"\r\n\r\n\"");
		return false;
	}
	
	//�����ײ���
	string buf, key, value;
	size_t begin = prev + 2, end = next;
	size_t i = 0, j = 0;
	while(1){
		end = http_request.find(crlf, begin);
		
		//ֱ������"\r\n\r\n"
		if(end <= pos_crlfcrlf){
			buf = http_request.substr(begin, end - begin); //����һ��
			
			for(; isblank(buf[i]); ++i);
			for(j = i; buf[j] != ':' && !isblank(buf[j]); ++j);
			key = buf.substr(i, j - i);
			
			for(i = j; !isalpha(buf[i]) && !isdigit(buf[i]); ++i);
			for(j = i; j != end; ++j);
			value = buf.substr(i, j - i);
			
			httphdr->header.insert(make_header(key, value));
			begin = end + 2;
		} else {
			break;
		}
	}
	
	//��ȡhttp�������ʵ��ֵ
	httphdr->body = http_request.substr(pos_crlfcrlf + 4, http_request.size() - pos_crlfcrlf - 4);
	return true;
}

//����key���õ�value
string get_value_from_http_header(const string& key, const header_map& header){
	if(header.empty())
		return "";
	header_map::const_iterator cit = header.find(key);
	if(cit == header.end())
		return "";
	return (*cit).second;
}
