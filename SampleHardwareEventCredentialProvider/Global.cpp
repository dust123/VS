//#include "afx.h"
#include "Global.h"

char G_Readinfochs[40] = { 0 };  //字符方式存储
string Readinfo ="";			 //字符串方式存储

//配置文件相关变量
string strCom = "";			     //字符串方式存储COM口
string strUser = "";		     //字符串方式存储登录用户 

int isopenCOM = 0;               //串口的线程是否有错误，用于检测串口设备是不是掉线了