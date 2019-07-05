//#include "stdafx.h"
#include "MySqlConn.h" 

MySqlConn::MySqlConn()
{
	//MySqlConn::a = 0;
	//this->a = 0;
}


MySqlConn::~MySqlConn()
{
 
}

bool MySqlConn::initConnection()
{

	mysql_init(&m_sqlCon); 
	//返回false则连接失败，返回true则连接成功  
	mysql_options(&m_sqlCon, MYSQL_OPT_COMPRESS, 0);
	if ( !mysql_real_connect(&m_sqlCon, host, user, psw, DB, port, NULL, 0))
		//中间分别是主机，用户名，密码，数据库名，端口号（可以写默认0或者3306等），可以先写成参数再传进去  
	{
		printf("Error connecting to database:%s\n", mysql_error(&m_sqlCon));
		return false;
	}
	else
	{
		printf("连接成功...\n");
		return true;
	}
}

bool MySqlConn::info_query()// 行为审计的一些配置
{

 
	mysql_query(&m_sqlCon, "SET NAMES GB2312"); 
	if (mysql_query(&m_sqlCon, "select * from BehavioralAudit")) {
		std::cout << "查询BehavioralAudit失败" << std::endl;
		::MessageBox(NULL, "查询BehavioralAudit失败", "qq", 0);
		return false;
	}
	else
	{
		std::cout << "查询BehavioralAudit成功" << std::endl;
		//::MessageBox(NULL, "查询BehavioralAudit成功", "qq", 0);
	}
	MYSQL_RES *result;//获得结果集
	result = mysql_store_result(&m_sqlCon);
	if (result)
	{
		my_ulonglong  row_num;
		int col_num;
		row_num = mysql_num_rows(result);
		col_num = mysql_num_fields(result);
		MYSQL_ROW sqlba_row;
		string strTempROW;
		//sqlba_row = mysql_fetch_row(result);
		while (sqlba_row = mysql_fetch_row(result))
		{
			//::MessageBox(NULL, sqlba_row[1], "查询sqlba_row成功", 0);
			strTempROW = sqlba_row[1];
			if (strTempROW == "BAip")
			{
				//::MessageBox(NULL, sqlba_row[2], "查询BAip成功", 0);
				mysqlBAip = sqlba_row[2];
				//::MessageBox(NULL, mysqlBAip.c_str(), "查询BAip成功", 0);
			}
			else if (strTempROW == "BALogoutdDirectory") {
				//::MessageBox(NULL, sqlba_row[2], "查询mysqlBALogoutdDirectory成功", 0);
				mysqlBALogoutdDirectory = sqlba_row[2];
				//::MessageBox(NULL, mysqlBALogoutdDirectory.c_str(), "查询mysqlBALogoutdDirectory成功", 0);
			}
			else if (strTempROW == "BALogoutT") {
				//::MessageBox(NULL, sqlba_row[2], "查询mysqlBALogoutT成功", 0);
				mysqlBALogoutT = sqlba_row[2];
				//::MessageBox(NULL, mysqlBALogoutT.c_str(), "查询mysqlBALogoutT成功", 0);
			}
			else if (strTempROW == "BALOginDirectory") {
				//::MessageBox(NULL, sqlba_row[2], "查询mysqlBALOginDirectory成功", 0);
				mysqlBALOginDirectory = sqlba_row[2];
				//::MessageBox(NULL, mysqlBALOginDirectory.c_str(), "查询mysqlBALOginDirectory成功", 0);
			}
			else if (strTempROW == "BALLoginDt") {
				//::MessageBox(NULL, sqlba_row[2], "查询mysqlBALLoginDt成功", 0);
				mysqlBALLoginDt = sqlba_row[2];
				//::MessageBox(NULL, mysqlBALLoginDt.c_str(), "查询mysqlBALLoginDt成功", 0);
			}

		} //endwhile
		return true;
	}//endif 
	else
	{
		return false;
	}

}

bool MySqlConn::user_query(string strSQL)
{
	 
	char query[255];
	//sprintf_s(query, sizeof(query), "select * from CarTable where CarNumber=md5(\"_Z&%sl^_\")", strSQL.c_str()); 
	sprintf_s(query, sizeof(query), "select id,CarNumber,UserName,nickname,AES_DECRYPT(UserPW ,'Dust')as UserPW  from CarTable where CarNumber=\"%s\" ", strSQL.c_str());
	//sprintf_s(query, sizeof(query), "select * from CarTable where CarNumber='%s'", strSQL.c_str());
	//::MessageBox(NULL, query, "query", 0);
	mysql_query(&m_sqlCon, "SET NAMES GB2312"); 
	//mysql_query(&m_sqlCon, "SET NAMES GB2312");//用这个
											   //mysql_query(&m_sqlCon, "SET NAMES 'Latin1'");
	if (mysql_query(&m_sqlCon, query)) {
		std::cout << "查询失败" << std::endl;
		::MessageBox(NULL, "查询失败", "qq", 0);
	}
	else
	{
		std::cout << "查询成功" << std::endl;
		//::MessageBox(NULL, "查询成功", "qq", 0);
	}

	MYSQL_RES *result;//获得结果集
	result = mysql_store_result(&m_sqlCon);
	if (result) {
		my_ulonglong  row_num;
		int col_num;
		row_num = mysql_num_rows(result);
		col_num = mysql_num_fields(result);
		//std::cout << "共有" << row_num << "条数据，以下为其详细内容：" << std::endl;
		//MYSQL_FIELD *fd;
		//while (fd = mysql_fetch_field(result)) {
		//	std::cout << fd->name << "\t";
		//}
		//std::cout << std::endl;
		MYSQL_ROW sql_row;
		//while (sql_row = mysql_fetch_row(result)) {
		//	for (int i = 0; i < col_num; i++) {
		//		if (sql_row[i] == NULL) std::cout << "NULL\t";
		//		else std::cout << sql_row[i] << "\t";
		//	}
		//	std::cout << std::endl;
		//}
		if (1 == row_num)
		{
			sql_row = mysql_fetch_row(result);
			cout << "用户名： " << sql_row[2] << endl;

			GetCarNumber  = sql_row[1];
			GetUserNames = sql_row[2];
			GetNickNume   = sql_row[3];
			//strlogUser    = sql_row[2];
			//strlogUserPW  = sql_row[4];
			//::MessageBox(NULL, GetCarNumber.c_str(), "query", 0);
			//::MessageBox(NULL, sql_row[1], "query", 0); 
			//::MessageBox(NULL, sql_row[2], "query", 0);
			//::MessageBox(NULL, GetNickNume.c_str(), "query", 0);
			//::MessageBox(NULL, sql_row[3], "query", 0);

			if ((sql_row[4] == NULL) || (sql_row[4] == "")) 
			{ 
				GetUserPW = ""; 
			}else{
				GetUserPW = sql_row[4];
			}
			//::MessageBox(NULL, sql_row[2], sql_row[2], 0);
			if (result != NULL)
				mysql_free_result(result);

			return true;
		}
		else 
		{
			GetCarNumber = strSQL.c_str();
			//return "未查到此卡信息";
			return false;
		}
		 
	}
	else
	{
		return false;
	}


	
}

bool MySqlConn::user_insert(char *chSQL)
{
	//char select_user[255];
	char query[255];
	//MYSQL_RES *result;
	//sprintf(select_user, "select * from user where UserName='%s'", body.userName);

	//sprintf_s(select_user,sizeof(select_user), "select * from CarTable where UserName='%s'","test");

	//cout << "SQLQUERY:" << select_user << endl;

	//if (mysql_query(&m_sqlCon, select_user) || !(result = mysql_store_result(&m_sqlCon))) {
	//	std::cout << "插入查询失败" << std::endl; 
	//}
	//if (mysql_num_rows(result)) {
	//	std::cout << "用户已存在" << std::endl; 
	//} 
	mysql_query(&m_sqlCon, "SET NAMES GB2312");//用这个

	sprintf_s(query, sizeof(query), chSQL);
	cout << "SQLinSert:" << query << endl;
	if (mysql_query(&m_sqlCon, query)) {
		std::cout << "插入数据失败" << std::endl;
		return false;
	}
	else 
	{
	//std::cout << "插入数据成功,共插入：" << mysql_affected_rows(&m_sqlCon) << "行" << std::endl;
	//mysql_free_result(result);
	/*	if (result != NULL)
		{
			mysql_free_result(result);
		}*/
		return true;
	}
}

bool MySqlConn::user_Print(string strGetMachineInfo)
{
	
	char chGetMachineInfo[250];
	char *sqlTime, *sqlMachineName, *sqlIP, *sqlMAC;

	memset(chGetMachineInfo, 0, sizeof(chGetMachineInfo));
	strcpy_s(chGetMachineInfo, strGetMachineInfo.c_str()); //string 到char[]
	sqlTime = strtok(chGetMachineInfo, "^");
	sqlMachineName = strtok(NULL, "^");
	sqlIP = strtok(NULL, "^");
	sqlMAC = strtok(NULL, "^");
	mysql_query(&m_sqlCon, "SET NAMES GB2312");
	//::MessageBox(NULL, "进入userPrint", "insert", 0);
	//::MessageBox(NULL, strGetMachineInfo.c_str(), "insert", 0);
	char select_user[500];
	MYSQL_RES *result;
	sprintf_s(select_user, sizeof(select_user), "select * from PrintUser where MachineMAC='%s'", sqlMAC);
	if (mysql_query(&m_sqlCon, select_user) || !(result = mysql_store_result(&m_sqlCon))  ) {
		std::cout << "修改查询失败" << std::endl; 
		::MessageBox(NULL, "修改查询失败", "insert", 0);
	}
	if (mysql_num_rows(result) == 1) {
		memset(select_user, 0, sizeof(select_user));
		if ("" == GetCarNumber) 
		{
			sprintf_s(select_user, sizeof(select_user), "update PrintUser set PuTime=\"%s\",PrintMark=\"%s\"  where MachineMAC=\"%s\"", sqlTime, "off", sqlMAC);
			 
		}
		else
		{
			sprintf_s(select_user, sizeof(select_user), "update PrintUser set MachineName=\"%s\",MachineIP=\"%s\",CarNumber=\"%s\",PrintUserNickName=\"%s\",PrintUserName=\"%s\",PuTime=\"%s\",PrintMark=\"%s\"  where MachineMAC=\"%s\"", sqlMachineName, sqlIP, GetCarNumber.c_str(), GetNickNume.c_str(), GetUserNames.c_str(), sqlTime, "on", sqlMAC);
			 
		}
		

		//::MessageBox(NULL, select_user, TEXT("update"), 0);
		if (mysql_query(&m_sqlCon, select_user)) {
			std::cout << "更新失败" << std::endl;
			::MessageBox(NULL, "更新失败", "insert", 0);
			return false;
		}
		else
		{
			return true;
		}

	}
	else {

		//::MessageBox(NULL, "insert", "insert", 0);
		//::MessageBox(NULL, sqlMachineName, "insert", 0);
		//::MessageBox(NULL, sqlMAC, "insert", 0);
		//::MessageBox(NULL, sqlIP, "insert", 0);
		//::MessageBox(NULL, GetCarNumber.c_str(), "insert", 0);
		//::MessageBox(NULL, GetNickNume.c_str(), "insert", 0);
		//::MessageBox(NULL, GetUserNames.c_str(), "insert", 0);
		//::MessageBox(NULL, sqlTime, "insert", 0);
		memset(select_user, 0, sizeof(select_user));

		if ("" == GetCarNumber)
		{
			sprintf_s(select_user, sizeof(select_user), "insert into PrintUser(MachineName,MachineMAC,MachineIP,PuTime,PrintMark)values( \"%s\",\"%s\",\"%s\",\"%s\",\"%s\");", sqlMachineName, sqlMAC, sqlIP,  sqlTime, "off");
		 
		}
		else
		{
			sprintf_s(select_user, sizeof(select_user), "insert into PrintUser(MachineName,MachineMAC,MachineIP,CarNumber,PrintUserNickName,PrintUserName,PuTime,PrintMark)values( \"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\");", sqlMachineName, sqlMAC, sqlIP, GetCarNumber.c_str(), GetNickNume.c_str(), GetUserNames.c_str(), sqlTime, "on");
		 
		}

		//::MessageBox(NULL, select_user, TEXT("user_Printinsert"), 0);
		if (mysql_query(&m_sqlCon, select_user)) {
			std::cout << "插入数据失败" << std::endl;
			return false;
		}
		else
		{
			return true;
		}
	}
	//return true;


}

bool MySqlConn::user_update(char *chpw)
{
	char query[255];
	//char select_user[255];
	//MYSQL_RES *result;
	//sprintf_s(select_user, sizeof(select_user), "select * from CarTable where UserName='%s'","test2");
	//if (mysql_query(&m_sqlCon, select_user) || !(result = mysql_store_result(&m_sqlCon))) {
	//	std::cout << "修改查询失败" << std::endl; 
	//}
	//if (mysql_num_rows(result) == 0) {
	//	std::cout << "要修改的用户不存在" << std::endl; 
	//}
	mysql_query(&m_sqlCon, "SET NAMES GB2312");//用这个
	//::MessageBox(NULL, chpw, TEXT("321"), 0);
	sprintf_s(query, sizeof(query), "update CarTable set UserPW=AES_ENCRYPT('%s','Dust') where CarNumber='%s'", chpw, GetCarNumber.c_str() );
	//::MessageBox(NULL, query, TEXT("321"), 0);
	if (mysql_query(&m_sqlCon, query)) {
		std::cout << "修改失败" << std::endl; 
		return false;
	}
	else
	{
		return true;
	}
	//std::cout << "修改成功，共修改：" << mysql_affected_rows(&m_sqlCon) << "行" << std::endl;
	//if (result != NULL)
	//	mysql_free_result(result);
	
}

void MySqlConn::user_delete()
{
	char query[255];
	char select_user[255];
	MYSQL_RES *result;
	sprintf_s(select_user, sizeof(select_user), "select * from CarTable where UserName='%s'", "test2");
	if (mysql_query(&m_sqlCon, select_user) || !(result = mysql_store_result(&m_sqlCon))) {
		std::cout << "修改查询失败" << std::endl;
	}
	if (mysql_num_rows(result) == 0) {
		std::cout << "要修改的用户不存在" << std::endl;
	} 

	sprintf_s(query, sizeof(query), "delete from CarTable where UserName='%s'", "test2");
	if (mysql_query(&m_sqlCon, query)) {
		std::cout << "修改失败" << std::endl;
	}
	std::cout << "修改成功，共修改：" << mysql_affected_rows(&m_sqlCon) << "行" << std::endl;

	if (result != NULL)
		mysql_free_result(result);
}

//释放资源  
void MySqlConn::FreeConnect()
{
	//mysql_free_result(res);  //释放一个结果集合使用的内存。
	mysql_close(&m_sqlCon);	 //关闭一个服务器连接。
}