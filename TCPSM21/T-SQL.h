#include "StdAfx.h"

_ConnectionPtr m_MYSQL; // ADO连接对象  连接数据库使用  
//*****************************************************************************
UINT  LOG_MYSQL()  //连接SQL
{
//读取配置信息
	char CurPath1[MAX_PATH]; //MAX_PATH最大长度260
 	GetCurrentDirectory(MAX_PATH,CurPath1);  //GetCurrentDirectory获得当前目录  MAX_PATH最大长度260 CurPath保存目录信息
 	strcat(CurPath1,"\\Server.ini");   //定位INI文件

 CString Server;
 CString Username;
CString password;
if (!GetPrivateProfileString("DATA","SQL Server",NULL,Server.GetBuffer(MAX_PATH),MAX_PATH,".\\Server.ini"))  //服务器地址
	Server = _T("127.0.0.1,1433"); 

if (!GetPrivateProfileString("DATA","Username",NULL,Username.GetBuffer(MAX_PATH),MAX_PATH,".\\Server.ini"))  //用户名
Username = _T("sa"); 

if (!GetPrivateProfileString("DATA","password",NULL,password.GetBuffer(MAX_PATH),MAX_PATH,".\\Server.ini"))  //密码
password = _T("password"); 

CString   sql;   
sql.Format(_T("Provider=SQLOLEDB;Server=%s;Database=FTP-data;uid=%s;pwd=%s;"),Server,Username,password);
///////////////////////////////////////////////////////////////////////////////////////////
	printf("连接数据库字符串为：%s\n",sql);
	try
	{
		::CoInitialize(NULL);
		if(!AfxOleInit())   //初始化COM库  只能初始化  一次
		{
			//cout<<"数据库初始化COM错误"<<endl;
			printf("数据库初始化COM错误\r\n");
		}
		HRESULT hr;

		if (FAILED(hr = m_MYSQL.CreateInstance("ADODB.Connection")))  ///创建Connection对象
		{
			//			AfxMessageBox("创建实例失败!");
			//		return FALSE;
		}

		if(SUCCEEDED(hr))
		{
			m_MYSQL->ConnectionTimeout=3;///设置超时时间为3秒
			//hr =m_MYSQL->Open("Provider=SQLOLEDB;Server=127.0.0.1,1433;Database=eml;uid=sa;pwd=admin;","","",adModeUnknown);///连接数据库
			hr =m_MYSQL->Open((LPCTSTR)(_bstr_t)sql,"","",adModeUnknown);///连接数据库
			///上面一句中连接字串中的Provider是针对ACCESS2000环境的，对于ACCESS97,需要改为:Provider=Microsoft.Jet.OLEDB.3.51;  }
		//cout<<"连接数据库成功！！"<<endl;
			printf("连接数据库成功！！\r\n");
		}
	}
	catch(_com_error e)///捕捉异常
	{
		//cout<<"连接数据库错误:"<<e.ErrorMessage()<<endl;
		printf("连接数据库错误:%s\r\n",e.ErrorMessage());
		return false;
	}
	///////////////////////////////////////////
 	_RecordsetPtr  m_pRecordset;
 	if (FAILED(m_pRecordset.CreateInstance("ADODB.Recordset")))
 	{
 		//cout<<"标题名:"<<a2<<endl;
 		//cout<<"创建实例失败1111!"<<endl;
		printf("创建实例失败1111!\r\n");
 		//AfxMessageBox("创建实例失败!");
 		return FALSE;
 	}
	////////////////////////////////////
	//////////////////////////////////////////////////
	return FALSE;
}  
//*****************************************************************************
bool add_SQL(string data)  //添加数据
{
	try{
		CString   sql;   
		sql.Format(_T("%s"),data.c_str());
//cout<<"SQL语句"<<(LPCSTR)sql<<endl;
		_bstr_t sqlSelect;
		sqlSelect=sql.AllocSysString();   //对字符串进行转换
		m_MYSQL->Execute(sqlSelect,NULL,adCmdText);  //执行连接
		//cout<<"添加邮箱："<<emil.c_str()<<"成功。"<<endl;
		return TRUE;
	}
	catch ( _com_error &e)
	{
		//MessageBox("您的操作失败！");
		//cout<<"添加邮箱："<<emil.c_str()<<"失败!"<<endl;
		//cout << "捕获到异常：" << e.ErrorMessage() << endl;
		return FALSE;
	}	
	return FALSE;
}
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************