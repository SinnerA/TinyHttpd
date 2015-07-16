#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <map>
#include <utility>
#include <sstream>
#include <ctype.h>
#include <iostream>

using namespace std;

typedef map<string, string> header_map;

#define make_header(key, value) \
                    make_pair((key), (value))

//保存从http request中解析得到的内容
typedef struct http_header_t{
	string      method;
	string      url;
	string      version;
	header_map  header;
	string      body;
}http_header_t;

//打印http_header_t里面的header
void print_http_header_header(const header_map& header);

//打印http_header_t
void print_http_header(http_header_t* httphdr);

//为http_header_t分配内存
http_header_t *alloc_http_header();

//回收为http_header_t分配的内存
void free_http_header(http_header_t* httphdr);

//解析http request，并将其存放在httphdr中
bool parse_http_request(const string& http_request, http_header_t* httphdr);

//获得header_map里的value
string get_value_from_http_header(const string& key, const header_map& header);