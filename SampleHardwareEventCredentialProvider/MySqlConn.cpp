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

bool MySqlConn::user_query(char *chSQL, string strSQL)
{
	 
	char query[255];
	sprintf_s(query, sizeof(query), "select * from CarTable2 where CarNumber=md5(\"_Z&%sl^_\")", strSQL.c_str() );
	//sprintf_s(query, sizeof(query), "select * from CarTable where CarNumber='%s'", strSQL.c_str());
	//::MessageBox(NULL, query, query, 0);
	//mysql_query(&m_sqlCon, "SET NAMES UTF8"); 
	mysql_query(&m_sqlCon, "SET NAMES GB2312");//�����
	if (mysql_query(&m_sqlCon, query )) {
		std::cout << "��ѯʧ��" << std::endl;
		//return;
	}
	else
	{
		std::cout << "��ѯ�ɹ�" << std::endl;
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
			GetCarNumber = sql_row[1];
			GetUserNumber = sql_row[2];
			//::MessageBox(NULL, sql_row[2], sql_row[2], 0);
			if (result != NULL)
				mysql_free_result(result);

			return true;
		}
		else 
		{
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

void MySqlConn::user_update()
{
	char query[255];
	char select_user[255];
	MYSQL_RES *result;
	sprintf_s(select_user, sizeof(select_user), "select * from CarTable where UserName='%s'","test2");
	if (mysql_query(&m_sqlCon, select_user) || !(result = mysql_store_result(&m_sqlCon))) {
		std::cout << "�޸Ĳ�ѯʧ��" << std::endl; 
	}
	if (mysql_num_rows(result) == 0) {
		std::cout << "Ҫ�޸ĵ��û�������" << std::endl; 
	}
	 
	sprintf_s(query, sizeof(query), "update CarTable set CarNumber='%s' where UserName='%s'","55555", "test2");
	if (mysql_query(&m_sqlCon, query)) {
		std::cout << "�޸�ʧ��" << std::endl; 
	}
	std::cout << "�޸ĳɹ������޸ģ�" << mysql_affected_rows(&m_sqlCon) << "��" << std::endl;
	if (result != NULL)
		mysql_free_result(result);
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