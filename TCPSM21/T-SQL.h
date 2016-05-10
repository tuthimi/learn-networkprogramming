#include "StdAfx.h"

_ConnectionPtr m_MYSQL; // ADO���Ӷ���  �������ݿ�ʹ��  
//*****************************************************************************
UINT  LOG_MYSQL()  //����SQL
{
//��ȡ������Ϣ
	char CurPath1[MAX_PATH]; //MAX_PATH��󳤶�260
 	GetCurrentDirectory(MAX_PATH,CurPath1);  //GetCurrentDirectory��õ�ǰĿ¼  MAX_PATH��󳤶�260 CurPath����Ŀ¼��Ϣ
 	strcat(CurPath1,"\\Server.ini");   //��λINI�ļ�

 CString Server;
 CString Username;
CString password;
if (!GetPrivateProfileString("DATA","SQL Server",NULL,Server.GetBuffer(MAX_PATH),MAX_PATH,".\\Server.ini"))  //��������ַ
	Server = _T("127.0.0.1,1433"); 

if (!GetPrivateProfileString("DATA","Username",NULL,Username.GetBuffer(MAX_PATH),MAX_PATH,".\\Server.ini"))  //�û���
Username = _T("sa"); 

if (!GetPrivateProfileString("DATA","password",NULL,password.GetBuffer(MAX_PATH),MAX_PATH,".\\Server.ini"))  //����
password = _T("password"); 

CString   sql;   
sql.Format(_T("Provider=SQLOLEDB;Server=%s;Database=FTP-data;uid=%s;pwd=%s;"),Server,Username,password);
///////////////////////////////////////////////////////////////////////////////////////////
	printf("�������ݿ��ַ���Ϊ��%s\n",sql);
	try
	{
		::CoInitialize(NULL);
		if(!AfxOleInit())   //��ʼ��COM��  ֻ�ܳ�ʼ��  һ��
		{
			//cout<<"���ݿ��ʼ��COM����"<<endl;
			printf("���ݿ��ʼ��COM����\r\n");
		}
		HRESULT hr;

		if (FAILED(hr = m_MYSQL.CreateInstance("ADODB.Connection")))  ///����Connection����
		{
			//			AfxMessageBox("����ʵ��ʧ��!");
			//		return FALSE;
		}

		if(SUCCEEDED(hr))
		{
			m_MYSQL->ConnectionTimeout=3;///���ó�ʱʱ��Ϊ3��
			//hr =m_MYSQL->Open("Provider=SQLOLEDB;Server=127.0.0.1,1433;Database=eml;uid=sa;pwd=admin;","","",adModeUnknown);///�������ݿ�
			hr =m_MYSQL->Open((LPCTSTR)(_bstr_t)sql,"","",adModeUnknown);///�������ݿ�
			///����һ���������ִ��е�Provider�����ACCESS2000�����ģ�����ACCESS97,��Ҫ��Ϊ:Provider=Microsoft.Jet.OLEDB.3.51;  }
		//cout<<"�������ݿ�ɹ�����"<<endl;
			printf("�������ݿ�ɹ�����\r\n");
		}
	}
	catch(_com_error e)///��׽�쳣
	{
		//cout<<"�������ݿ����:"<<e.ErrorMessage()<<endl;
		printf("�������ݿ����:%s\r\n",e.ErrorMessage());
		return false;
	}
	///////////////////////////////////////////
 	_RecordsetPtr  m_pRecordset;
 	if (FAILED(m_pRecordset.CreateInstance("ADODB.Recordset")))
 	{
 		//cout<<"������:"<<a2<<endl;
 		//cout<<"����ʵ��ʧ��1111!"<<endl;
		printf("����ʵ��ʧ��1111!\r\n");
 		//AfxMessageBox("����ʵ��ʧ��!");
 		return FALSE;
 	}
	////////////////////////////////////
	//////////////////////////////////////////////////
	return FALSE;
}  
//*****************************************************************************
bool add_SQL(string data)  //�������
{
	try{
		CString   sql;   
		sql.Format(_T("%s"),data.c_str());
//cout<<"SQL���"<<(LPCSTR)sql<<endl;
		_bstr_t sqlSelect;
		sqlSelect=sql.AllocSysString();   //���ַ�������ת��
		m_MYSQL->Execute(sqlSelect,NULL,adCmdText);  //ִ������
		//cout<<"������䣺"<<emil.c_str()<<"�ɹ���"<<endl;
		return TRUE;
	}
	catch ( _com_error &e)
	{
		//MessageBox("���Ĳ���ʧ�ܣ�");
		//cout<<"������䣺"<<emil.c_str()<<"ʧ��!"<<endl;
		//cout << "�����쳣��" << e.ErrorMessage() << endl;
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