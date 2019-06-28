#pragma once
#include <winsock.h>
#include "include\mysql.h"

#include<iostream>    //cout system("pause");
 
#pragma comment(lib, "libmysql.lib")



using namespace std;


class MySqlConn
{
public:
	MySqlConn();
	~MySqlConn();
private:
	//const char host[20] = "192.168.0.7";
	//static const char user[] = "root";
	//static const char psw[] = "789456963-a";
	//static const char table[] = "db";
	//static const int port = 3306;
	const char *host = "192.168.0.7";
	const char *user = "root";
	const char *psw = "789456963-a";
	const char *DB = "db";
	const int port = 3306;
	
 
public:
	string GetCarNumber;
	string GetUserNames;
	string GetNickNume;
	string GetUserPW;
//行为审计的配置信息
	string mysqlBAip;
	string mysqlBALogoutdDirectory;
	string mysqlBALogoutT;
	string mysqlBALOginDirectory;
	string mysqlBALLoginDt;

	//初始化mysql  
	MYSQL m_sqlCon;  //连接mysql，数据库  

	bool initConnection();
	bool info_query(); // 行为审计的一些配置
	bool user_query(string strSQL);
	bool user_insert(char *chSQL);
	bool user_update(char *chpw);
	void user_delete();
	void FreeConnect();
};

