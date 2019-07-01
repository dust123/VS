#include "HttpConnect.h"
 
#include <stdio.h>
#include<iostream>
#include <Windows.h>   
#include <fstream>
#include<sstream>
//#include <stdlib.h>


#ifdef WIN32
#pragma comment(lib,"ws2_32.lib")
#endif
using namespace std;

HttpConnect::HttpConnect()
{
#ifdef WIN32
	//此处一定要初始化一下，否则gethostbyname返回一直为空
	WSADATA wsa = { 0 };
	WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
}


HttpConnect::~HttpConnect()
{
}


bool HttpConnect::socketHttp(std::string host, std::string request,bool boolGET)
{
	SOCKET sockfd;
	struct sockaddr_in address;
	struct hostent *server;

	//string  strData;
	//int FindOne;/*字符查找*/
	//int FindTow ;/*字符查找*/

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	address.sin_family = AF_INET;
	address.sin_port = htons(80);
	server = gethostbyname(host.c_str());
	memcpy((char *)&address.sin_addr.s_addr, (char*)server->h_addr, server->h_length);

	if (-1 == connect(sockfd, (struct sockaddr *)&address, sizeof(address))) {
		cout << "connection error!" << std::endl;
		return false;
	}

	//cout << request << std::endl;
#ifdef WIN32
	send(sockfd, request.c_str(), (int)request.size(), 0);
#else
	write(sockfd, request.c_str(), request.size());
#endif
	//char buf[1024 * 1024] = { 0 };
	char *buf;
	buf = new char[1024 * 1024];

	int offset = 0;
	int rc;

#ifdef WIN32
	while (rc = recv(sockfd, buf + offset, 1024, 0))
#else
	while (rc = read(sockfd, buf + offset, 1024))
#endif
	{
		offset += rc;
	}

#ifdef WIN32
	closesocket(sockfd);
#else
	close(sockfd);
#endif
	buf[offset] = 0;
	//cout << buf << std::endl;
	int intFindOne=0, intFindTow=0;
	string  strDatabuf; 
	if (boolGET) {
		strDatabuf = buf;
		//strData = (string)buf;
		intFindOne = (int)strDatabuf.find("%E7", 0);/*字符查找*/
		intFindTow = (int)strDatabuf.find("authfailed", 0);/*字符查找*/
		//cout << "aaaaa: " << offisetpre << endl;
		//isfind = memchr(buf, '%E7', sizeof(buf));
		//isfindstr = memchr(buf, 'fail', sizeof(buf));
	}


	delete[]buf;

	if (boolGET) 
	{

		if ((-1 == intFindOne) && ( -1 == intFindTow))
		{
			//cout << "用户登录OK" << std::endl;
			boolGET = true;
			//::MessageBox(NULL, "boolGET is true", TEXT("boolGET"), 0);
		}
		else
		{
			boolGET = false;
			//::MessageBox(NULL, "用户登录互联网行为审计失败", TEXT("boolGET"), 0);
		}
		
	}
	
    return boolGET;

}

bool HttpConnect::postData(std::string host, std::string path, std::string post_content)
{
	//POST请求方式
	std::stringstream stream;
	stream << "POST " << path;
	stream << " HTTP/1.0\r\n";
	stream << "Host: " << host << "\r\n";
	stream << "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN; rv:1.9.2.3) Gecko/20100401 Firefox/3.6.3\r\n";
	stream << "Content-Type:application/x-www-form-urlencoded\r\n";
	stream << "Content-Length:" << post_content.length() << "\r\n";
	stream << "Connection:close\r\n\r\n";
	stream << post_content.c_str();
	bool temp = false;
	temp = socketHttp(host, stream.str(), true);
	if (temp)
	{
		//::MessageBox(NULL, "return true", TEXT("postData"), 0);
		return true;
	}
	else
	{
		//::MessageBox(NULL, "用户登录互联网行为审计失败", TEXT("postData"), 0);
		return false;
	}
}

void HttpConnect::getData(std::string host, std::string path, std::string get_content)
{
	//GET请求方式
	std::stringstream stream;
	stream << "GET " << path << "?" << get_content;
	stream << " HTTP/1.0\r\n";
	stream << "Host: " << host << "\r\n";
	stream << "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN; rv:1.9.2.3) Gecko/20100401 Firefox/3.6.3\r\n";
	stream << "Connection:close\r\n\r\n";

	socketHttp(host, stream.str(),false);
}