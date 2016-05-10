// TCP21.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TCP21.h"
#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#include "T-SQL.h"
#include <afxmt.h>  //线程呼哧
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object
CWinApp theApp;
using namespace std;
	CCriticalSection FTP_pass;  //线程呼哧
	CString IP1; //开始IP
	CString IP2; //结束IP
	CString port1; //端口号

				//读取配置信息
char CurPath1[MAX_PATH]; //MAX_PATH最大长度260   INI文件
//数据结构
typedef struct tagHyperLinkTag{
char *host;  //IP地址
}HyperLink;
UINT IPport(LPVOID   lp) //查看端口是否开放
{
	HyperLink *pdata=(HyperLink*)lp;
	char *host=pdata->host;
	printf("正在扫描%s:21 \r",host);
	FD_SET    mask;
	TIMEVAL timeout;
    SOCKET sockfd;
    struct sockaddr_in addr;//套接字
	timeout.tv_sec=5;
    timeout.tv_usec=0;  /* 设置select等待的最大时间为5秒 */
    int port=atoi(port1);
	WSADATA               WSAData;  //这个结构被用来存储 被WSAStartup函数调用后返回的 Windows Sockets 数据
	WSAStartup(MAKEWORD(2,2) ,&WSAData);  //确定SOCKET版本

    addr.sin_family =AF_INET;    //sin_family 地址家族（必须是AF_INET）
	addr.sin_addr.s_addr =inet_addr(host);//inet_addr将ip地址转换成网络地址

	sockfd = socket(AF_INET, SOCK_STREAM, 0);  //inet_addr将ip地址转换成网络地址  IP地址不正确返回INADDR_NONE
    if(sockfd==INVALID_SOCKET)//IP不对
    {   
		printf("Socket 错误!\n");
        return 0;
    }
	
	u_long value;
 	value=1;
    ioctlsocket(sockfd,FIONBIO,&value);  //控制套接口的模式
	addr.sin_port = htons(port);//存储端口号（使用网络字节顺序）
   
	
    connect(sockfd,(struct sockaddr *) &addr, sizeof(addr));//连接并  查看是否可以连接
	

    FD_ZERO(&mask);//初始化套接字
    FD_SET(sockfd,&mask); //将sockfd加入mask集合
    value=select(0,NULL,&mask,NULL,&timeout); /* 检测我们上面设置到集合mask里的句柄是否有可读信息 */

    if(value==0 || value==-1)
    {    
		//closesocket(sockfd);
		//printf("连接超时\n");
		//printf("%s:%d连接超时\n",host,port); /* 说明在我们设定的时间值1秒加500毫秒的时间内，socket的状态没有发生变化 */
		Sleep(50);
		return 0; 
    }
    else 
	{
		FTP_pass.Lock(); //在对队列进行操作前申请临界对象
				char *host2=host;
		printf("\t%s:开放端口%d 打开.\r\n",host2,port);
		shutdown(sockfd, 0);    //释放socket

				CString tim;
				CTime tm;
				tm=CTime::GetCurrentTime();    
				tim=tm.Format(_T("%Y-%m-%d  %X"));
		
		CString   sql;   
		sql.Format(_T("insert into FTPURL(URL,TIME1)VALUES('%s','%s')"),host2,tim);
		if (add_SQL(sql.GetBuffer(sql.GetLength())))
		{
			printf("\t添加IP数据成功.\r\n");
			Sleep(50);
		}
		else
		{
			printf("\t添加IP数据失败.\r\n");
			Sleep(50);
		}
		FTP_pass.Unlock();//离开临界区
		closesocket(sockfd);  //关闭socket
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


UINT IPSC(LPVOID pParam)  //IP地址生成
{
	//嘿嘿，这一次给你一个绝对好用的代码，共享吧^_^ 
//----------------------------------------- 
 int ID2=0;  //保存INI方便下次软件启动重复扫描
//IP地址段扫描代码： 
    in_addr   ia; 
    unsigned   long   FirstIp,SecondIp; 
    int   Delta; 
	//AnsiString Addr; 

    FirstIp   =   inet_addr(IP1);       //任意的开始地址 
    SecondIp   =  inet_addr(IP2);   //任意的结束地址 
    
    //转换成能直接递增和递减的地址 
    FirstIp   =   InvertIp(FirstIp); 
    SecondIp   =   InvertIp(SecondIp); 

    Delta   =   SecondIp   -   FirstIp; 
	printf("开始IP:%s 结束IP:%s  扫描端口:%s\r\n",IP1,IP2,port1);
	printf("共%d 个IP需要扫描 \r\n",Delta);
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
			//printf("保存数据ID清零\r\n");
			WritePrivateProfileString("DATA","IP1",inet_ntoa(ia),CurPath1); //目标
		}
		Sleep(50);
    } 
printf("扫描结束 \r\n");
	return 0;
}

///////////////////////////////////////////////////////////////////////////
UINT  SQ_INI()   //读取INI文件
{
	GetCurrentDirectory(MAX_PATH,CurPath1);  //GetCurrentDirectory获得当前目录  MAX_PATH最大长度260 CurPath保存目录信息
	strcat(CurPath1,"\\IP.ini");   //定位INI文件

	if (!GetPrivateProfileString("DATA","IP1",NULL,IP1.GetBuffer(MAX_PATH),MAX_PATH,".\\IP.ini"))  //开始IP
		IP1 = _T("1.0.0.1"); 

	if (!GetPrivateProfileString("DATA","IP2",NULL,IP2.GetBuffer(MAX_PATH),MAX_PATH,".\\IP.ini"))  //结束IP
		IP2 = _T("255.255.255.254"); 

	if (!GetPrivateProfileString("DATA","port",NULL,port1.GetBuffer(MAX_PATH),MAX_PATH,".\\IP.ini"))  //端口号
		port1 = _T("21"); 



	return FALSE;
}
///////////////////////////////

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
SetConsoleTitle("IP端口扫描器-www.20cn.net--BY:玉面飞龙"); // 设置窗口标题 
	HANDLE m_hMutex=CreateMutex(NULL,FALSE,"TCP21 玉面飞龙 http://hi.baidu.com/alalmn");
if(GetLastError()==ERROR_ALREADY_EXISTS)
{	//如果已有互斥量存在则释放句柄并复位互斥量
	CloseHandle(m_hMutex);
	m_hMutex=NULL;
	//退出程序
	ExitProcess(0);
}
// CWinApp app((LPCTSTR)argv[0]);
// 	app.InitApplication();
// 	AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0);
// 	HyperLink m_thisURL;
// 	m_thisURL.host="127.0.0.1";
// 	AfxBeginThread(IPport,&m_thisURL);
// 	//IPport(host,port);//查看端口是否开放
SQ_INI();   //读取INI文件
LOG_MYSQL();  //连接SQL
Sleep(1000);
	AfxBeginThread(::IPSC,NULL);
	   while (TRUE)
   {
		   Sleep(10000);
	   }
	return 0;
}
//cout << (LPCTSTR)strHello << endl;

