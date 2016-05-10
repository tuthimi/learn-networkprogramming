// TCP21.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TCP21.h"
#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#include "T-SQL.h"
#include <afxmt.h>  //�̺߳���
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object
CWinApp theApp;
using namespace std;
	CCriticalSection FTP_pass;  //�̺߳���
	CString IP1; //��ʼIP
	CString IP2; //����IP
	CString port1; //�˿ں�

				//��ȡ������Ϣ
char CurPath1[MAX_PATH]; //MAX_PATH��󳤶�260   INI�ļ�
//���ݽṹ
typedef struct tagHyperLinkTag{
char *host;  //IP��ַ
}HyperLink;
UINT IPport(LPVOID   lp) //�鿴�˿��Ƿ񿪷�
{
	HyperLink *pdata=(HyperLink*)lp;
	char *host=pdata->host;
	printf("����ɨ��%s:21 \r",host);
	FD_SET    mask;
	TIMEVAL timeout;
    SOCKET sockfd;
    struct sockaddr_in addr;//�׽���
	timeout.tv_sec=5;
    timeout.tv_usec=0;  /* ����select�ȴ������ʱ��Ϊ5�� */
    int port=atoi(port1);
	WSADATA               WSAData;  //����ṹ�������洢 ��WSAStartup�������ú󷵻ص� Windows Sockets ����
	WSAStartup(MAKEWORD(2,2) ,&WSAData);  //ȷ��SOCKET�汾

    addr.sin_family =AF_INET;    //sin_family ��ַ���壨������AF_INET��
	addr.sin_addr.s_addr =inet_addr(host);//inet_addr��ip��ַת���������ַ

	sockfd = socket(AF_INET, SOCK_STREAM, 0);  //inet_addr��ip��ַת���������ַ  IP��ַ����ȷ����INADDR_NONE
    if(sockfd==INVALID_SOCKET)//IP����
    {   
		printf("Socket ����!\n");
        return 0;
    }
	
	u_long value;
 	value=1;
    ioctlsocket(sockfd,FIONBIO,&value);  //�����׽ӿڵ�ģʽ
	addr.sin_port = htons(port);//�洢�˿ںţ�ʹ�������ֽ�˳��
   
	
    connect(sockfd,(struct sockaddr *) &addr, sizeof(addr));//���Ӳ�  �鿴�Ƿ��������
	

    FD_ZERO(&mask);//��ʼ���׽���
    FD_SET(sockfd,&mask); //��sockfd����mask����
    value=select(0,NULL,&mask,NULL,&timeout); /* ��������������õ�����mask��ľ���Ƿ��пɶ���Ϣ */

    if(value==0 || value==-1)
    {    
		//closesocket(sockfd);
		//printf("���ӳ�ʱ\n");
		//printf("%s:%d���ӳ�ʱ\n",host,port); /* ˵���������趨��ʱ��ֵ1���500�����ʱ���ڣ�socket��״̬û�з����仯 */
		Sleep(50);
		return 0; 
    }
    else 
	{
		FTP_pass.Lock(); //�ڶԶ��н��в���ǰ�����ٽ����
				char *host2=host;
		printf("\t%s:���Ŷ˿�%d ��.\r\n",host2,port);
		shutdown(sockfd, 0);    //�ͷ�socket

				CString tim;
				CTime tm;
				tm=CTime::GetCurrentTime();    
				tim=tm.Format(_T("%Y-%m-%d  %X"));
		
		CString   sql;   
		sql.Format(_T("insert into FTPURL(URL,TIME1)VALUES('%s','%s')"),host2,tim);
		if (add_SQL(sql.GetBuffer(sql.GetLength())))
		{
			printf("\t���IP���ݳɹ�.\r\n");
			Sleep(50);
		}
		else
		{
			printf("\t���IP����ʧ��.\r\n");
			Sleep(50);
		}
		FTP_pass.Unlock();//�뿪�ٽ���
		closesocket(sockfd);  //�ر�socket
		Sleep(50);
	}
     
	return 0;
}

//////////////////////////////////////////////////////////////////////////
unsigned   long   __fastcall   InvertIp(unsigned   long   NormalIp) 
{ 
    unsigned   char b1,b2,b3,b4; 
b1=NormalIp&0x00FF;
b2=(NormalIp>>8)&0x00FF;
b3=(NormalIp>>16)&0x00FF;
b4=(NormalIp>>24)&0x00FF;

return  (b1<<24)|(b2<<16)|(b3<<8)|b4; 
} 


UINT IPSC(LPVOID pParam)  //IP��ַ����
{
	//�ٺ٣���һ�θ���һ�����Ժ��õĴ��룬�����^_^ 
//----------------------------------------- 
 int ID2=0;  //����INI�����´���������ظ�ɨ��
//IP��ַ��ɨ����룺 
    in_addr   ia; 
    unsigned   long   FirstIp,SecondIp; 
    int   Delta; 
	//AnsiString Addr; 

    FirstIp   =   inet_addr(IP1);       //����Ŀ�ʼ��ַ 
    SecondIp   =  inet_addr(IP2);   //����Ľ�����ַ 
    
    //ת������ֱ�ӵ����͵ݼ��ĵ�ַ 
    FirstIp   =   InvertIp(FirstIp); 
    SecondIp   =   InvertIp(SecondIp); 

    Delta   =   SecondIp   -   FirstIp; 
	printf("��ʼIP:%s ����IP:%s  ɨ��˿�:%s\r\n",IP1,IP2,port1);
	printf("��%d ��IP��Ҫɨ�� \r\n",Delta);
    for(int  i=0;i<=Delta;i++) 
    { 
        ia.S_un.S_addr=InvertIp(FirstIp++);

		 	HyperLink m_thisURL;
 	m_thisURL.host=inet_ntoa(ia);
 	AfxBeginThread(IPport,&m_thisURL);
        ID2++;
		if (ID2>=200)
		{
			ID2=0;
			//printf("��������ID����\r\n");
			WritePrivateProfileString("DATA","IP1",inet_ntoa(ia),CurPath1); //Ŀ��
		}
		Sleep(50);
    } 
printf("ɨ����� \r\n");
	return 0;
}

///////////////////////////////////////////////////////////////////////////
UINT  SQ_INI()   //��ȡINI�ļ�
{
	GetCurrentDirectory(MAX_PATH,CurPath1);  //GetCurrentDirectory��õ�ǰĿ¼  MAX_PATH��󳤶�260 CurPath����Ŀ¼��Ϣ
	strcat(CurPath1,"\\IP.ini");   //��λINI�ļ�

	if (!GetPrivateProfileString("DATA","IP1",NULL,IP1.GetBuffer(MAX_PATH),MAX_PATH,".\\IP.ini"))  //��ʼIP
		IP1 = _T("1.0.0.1"); 

	if (!GetPrivateProfileString("DATA","IP2",NULL,IP2.GetBuffer(MAX_PATH),MAX_PATH,".\\IP.ini"))  //����IP
		IP2 = _T("255.255.255.254"); 

	if (!GetPrivateProfileString("DATA","port",NULL,port1.GetBuffer(MAX_PATH),MAX_PATH,".\\IP.ini"))  //�˿ں�
		port1 = _T("21"); 



	return FALSE;
}
///////////////////////////////

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
SetConsoleTitle("IP�˿�ɨ����-www.20cn.net--BY:�������"); // ���ô��ڱ��� 
	HANDLE m_hMutex=CreateMutex(NULL,FALSE,"TCP21 ������� http://hi.baidu.com/alalmn");
if(GetLastError()==ERROR_ALREADY_EXISTS)
{	//������л������������ͷž������λ������
	CloseHandle(m_hMutex);
	m_hMutex=NULL;
	//�˳�����
	ExitProcess(0);
}
// CWinApp app((LPCTSTR)argv[0]);
// 	app.InitApplication();
// 	AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0);
// 	HyperLink m_thisURL;
// 	m_thisURL.host="127.0.0.1";
// 	AfxBeginThread(IPport,&m_thisURL);
// 	//IPport(host,port);//�鿴�˿��Ƿ񿪷�
SQ_INI();   //��ȡINI�ļ�
LOG_MYSQL();  //����SQL
Sleep(1000);
	AfxBeginThread(::IPSC,NULL);
	   while (TRUE)
   {
		   Sleep(10000);
	   }
	return 0;
}
//cout << (LPCTSTR)strHello << endl;

