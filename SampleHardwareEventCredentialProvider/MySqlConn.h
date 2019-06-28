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
//��Ϊ��Ƶ�������Ϣ
	string mysqlBAip;
	string mysqlBALogoutdDirectory;
	string mysqlBALogoutT;
	string mysqlBALOginDirectory;
	string mysqlBALLoginDt;

	//��ʼ��mysql  
	MYSQL m_sqlCon;  //����mysql�����ݿ�  

	bool initConnection();
	bool info_query(); // ��Ϊ��Ƶ�һЩ����
	bool user_query(string strSQL);
	bool user_insert(char *chSQL);
	bool user_update(char *chpw);
	void user_delete();
	void FreeConnect();
};

