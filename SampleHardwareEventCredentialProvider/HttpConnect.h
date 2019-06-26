#pragma once

#include <string>

class HttpConnect
{
public:
	HttpConnect();
	~HttpConnect();
public:
	//bool httpSockBack = false;
	bool BoolHttpback = false;
	bool HttpConnect::socketHttp(std::string host, std::string request,bool boolGET);
	bool HttpConnect::postData(std::string host, std::string path, std::string post_content);
	void HttpConnect::getData(std::string host, std::string path, std::string get_content);
};