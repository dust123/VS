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
	//����false������ʧ�ܣ�����true�����ӳɹ�  
	mysql_options(&m_sqlCon, MYSQL_OPT_COMPRESS, 0);
	if ( !mysql_real_connect(&m_sqlCon, host, user, psw, DB, port, NULL, 0))
		//�м�ֱ����������û��������룬���ݿ������˿ںţ�����дĬ��0����3306�ȣ���������д�ɲ����ٴ���ȥ  
	{
		printf("Error connecting to database:%s\n", mysql_error(&m_sqlCon));
		return false;
	}
	else
	{
		printf("���ӳɹ�...\n");
		return true;
	}
}

bool MySqlConn::info_query()// ��Ϊ��Ƶ�һЩ����
{

 
	mysql_query(&m_sqlCon, "SET NAMES GB2312"); 
	if (mysql_query(&m_sqlCon, "select * from BehavioralAudit")) {
		std::cout << "��ѯBehavioralAuditʧ��" << std::endl;
		::MessageBox(NULL, "��ѯBehavioralAuditʧ��", "qq", 0);
		return false;
	}
	else
	{
		std::cout << "��ѯBehavioralAudit�ɹ�" << std::endl;
		//::MessageBox(NULL, "��ѯBehavioralAudit�ɹ�", "qq", 0);
	}
	MYSQL_RES *result;//��ý����
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
			//::MessageBox(NULL, sqlba_row[1], "��ѯsqlba_row�ɹ�", 0);
			strTempROW = sqlba_row[1];
			if (strTempROW == "BAip")
			{
				//::MessageBox(NULL, sqlba_row[2], "��ѯBAip�ɹ�", 0);
				mysqlBAip = sqlba_row[2];
				//::MessageBox(NULL, mysqlBAip.c_str(), "��ѯBAip�ɹ�", 0);
			}
			else if (strTempROW == "BALogoutdDirectory") {
				//::MessageBox(NULL, sqlba_row[2], "��ѯmysqlBALogoutdDirectory�ɹ�", 0);
				mysqlBALogoutdDirectory = sqlba_row[2];
				//::MessageBox(NULL, mysqlBALogoutdDirectory.c_str(), "��ѯmysqlBALogoutdDirectory�ɹ�", 0);
			}
			else if (strTempROW == "BALogoutT") {
				//::MessageBox(NULL, sqlba_row[2], "��ѯmysqlBALogoutT�ɹ�", 0);
				mysqlBALogoutT = sqlba_row[2];
				//::MessageBox(NULL, mysqlBALogoutT.c_str(), "��ѯmysqlBALogoutT�ɹ�", 0);
			}
			else if (strTempROW == "BALOginDirectory") {
				//::MessageBox(NULL, sqlba_row[2], "��ѯmysqlBALOginDirectory�ɹ�", 0);
				mysqlBALOginDirectory = sqlba_row[2];
				//::MessageBox(NULL, mysqlBALOginDirectory.c_str(), "��ѯmysqlBALOginDirectory�ɹ�", 0);
			}
			else if (strTempROW == "BALLoginDt") {
				//::MessageBox(NULL, sqlba_row[2], "��ѯmysqlBALLoginDt�ɹ�", 0);
				mysqlBALLoginDt = sqlba_row[2];
				//::MessageBox(NULL, mysqlBALLoginDt.c_str(), "��ѯmysqlBALLoginDt�ɹ�", 0);
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
	//mysql_query(&m_sqlCon, "SET NAMES GB2312");//�����
											   //mysql_query(&m_sqlCon, "SET NAMES 'Latin1'");
	if (mysql_query(&m_sqlCon, query)) {
		std::cout << "��ѯʧ��" << std::endl;
		::MessageBox(NULL, "��ѯʧ��", "qq", 0);
	}
	else
	{
		std::cout << "��ѯ�ɹ�" << std::endl;
		//::MessageBox(NULL, "��ѯ�ɹ�", "qq", 0);
	}

	MYSQL_RES *result;//��ý����
	result = mysql_store_result(&m_sqlCon);
	if (result) {
		my_ulonglong  row_num;
		int col_num;
		row_num = mysql_num_rows(result);
		col_num = mysql_num_fields(result);
		//std::cout << "����" << row_num << "�����ݣ�����Ϊ����ϸ���ݣ�" << std::endl;
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
			cout << "�û����� " << sql_row[2] << endl;

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
			//return "δ�鵽�˿���Ϣ";
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
	//	std::cout << "�����ѯʧ��" << std::endl; 
	//}
	//if (mysql_num_rows(result)) {
	//	std::cout << "�û��Ѵ���" << std::endl; 
	//} 
	mysql_query(&m_sqlCon, "SET NAMES GB2312");//�����

	sprintf_s(query, sizeof(query), chSQL);
	cout << "SQLinSert:" << query << endl;
	if (mysql_query(&m_sqlCon, query)) {
		std::cout << "��������ʧ��" << std::endl;
		return false;
	}
	else 
	{
	//std::cout << "�������ݳɹ�,�����룺" << mysql_affected_rows(&m_sqlCon) << "��" << std::endl;
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
	strcpy_s(chGetMachineInfo, strGetMachineInfo.c_str()); //string ��char[]
	sqlTime = strtok(chGetMachineInfo, "^");
	sqlMachineName = strtok(NULL, "^");
	sqlIP = strtok(NULL, "^");
	sqlMAC = strtok(NULL, "^");
	mysql_query(&m_sqlCon, "SET NAMES GB2312");
	//::MessageBox(NULL, "����userPrint", "insert", 0);
	//::MessageBox(NULL, strGetMachineInfo.c_str(), "insert", 0);
	char select_user[500];
	MYSQL_RES *result;
	sprintf_s(select_user, sizeof(select_user), "select * from PrintUser where MachineMAC='%s'", sqlMAC);
	if (mysql_query(&m_sqlCon, select_user) || !(result = mysql_store_result(&m_sqlCon))  ) {
		std::cout << "�޸Ĳ�ѯʧ��" << std::endl; 
		::MessageBox(NULL, "�޸Ĳ�ѯʧ��", "insert", 0);
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
			std::cout << "����ʧ��" << std::endl;
			::MessageBox(NULL, "����ʧ��", "insert", 0);
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
			std::cout << "��������ʧ��" << std::endl;
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
	//	std::cout << "�޸Ĳ�ѯʧ��" << std::endl; 
	//}
	//if (mysql_num_rows(result) == 0) {
	//	std::cout << "Ҫ�޸ĵ��û�������" << std::endl; 
	//}
	mysql_query(&m_sqlCon, "SET NAMES GB2312");//�����
	//::MessageBox(NULL, chpw, TEXT("321"), 0);
	sprintf_s(query, sizeof(query), "update CarTable set UserPW=AES_ENCRYPT('%s','Dust') where CarNumber='%s'", chpw, GetCarNumber.c_str() );
	//::MessageBox(NULL, query, TEXT("321"), 0);
	if (mysql_query(&m_sqlCon, query)) {
		std::cout << "�޸�ʧ��" << std::endl; 
		return false;
	}
	else
	{
		return true;
	}
	//std::cout << "�޸ĳɹ������޸ģ�" << mysql_affected_rows(&m_sqlCon) << "��" << std::endl;
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
		std::cout << "�޸Ĳ�ѯʧ��" << std::endl;
	}
	if (mysql_num_rows(result) == 0) {
		std::cout << "Ҫ�޸ĵ��û�������" << std::endl;
	} 

	sprintf_s(query, sizeof(query), "delete from CarTable where UserName='%s'", "test2");
	if (mysql_query(&m_sqlCon, query)) {
		std::cout << "�޸�ʧ��" << std::endl;
	}
	std::cout << "�޸ĳɹ������޸ģ�" << mysql_affected_rows(&m_sqlCon) << "��" << std::endl;

	if (result != NULL)
		mysql_free_result(result);
}

//�ͷ���Դ  
void MySqlConn::FreeConnect()
{
	//mysql_free_result(res);  //�ͷ�һ���������ʹ�õ��ڴ档
	mysql_close(&m_sqlCon);	 //�ر�һ�����������ӡ�
}