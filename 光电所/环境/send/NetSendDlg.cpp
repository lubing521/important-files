// NetSendDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NetSend.h"
#include "NetSendDlg.h"

#include <winsock2.h>
#include <stdio.h>
#include <afx.h>

#pragma comment(lib,  "Ws2_32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int netflag;
int alnetflag ;
int ret;

int bmpEnd;
int bDataEnd;

HANDLE hFind1;


HRESULT   hFind2;

char * filName;


PBYTE sBuf;

/////////////////////////////////////////////////////////////////////////////
// CNetSendDlg dialog

CNetSendDlg::CNetSendDlg(CWnd* pParent /*=NULL*/) : CDialog(CNetSendDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNetSendDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNetSendDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNetSendDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

}

BEGIN_MESSAGE_MAP(CNetSendDlg, CDialog)
	//{{AFX_MSG_MAP(CNetSendDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_SEND, OnBtnSend)
	ON_BN_CLICKED(IDC_BTN_STOP, OnBtnStop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetSendDlg message handlers

BOOL CNetSendDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
    hFind1=NULL;
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CNetSendDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CNetSendDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

UINT NetSendThread(LPVOID param)
{
	CDC* netdc;
	netdc = (CDC *) param;
	int index;
	index = 0;

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		return 0;
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		return 0;
	}
	SOCKET sockClient = socket(AF_INET, SOCK_DGRAM, 0);
	SOCKADDR_IN addrClient;
	addrClient.sin_addr.S_un.S_addr = inet_addr("192.168.200.200");
	addrClient.sin_family = AF_INET;
	addrClient.sin_port = htons(11020);
	SOCKADDR_IN addrSrv;
    
	int bmpsend_flag=0;
	int datasend_flag=0;
    int count1=0;
	int count2=0;

    int countd=0;//数据data的num

	int rt;
	bmpEnd=0;
	bDataEnd=0;
	HANDLE hBmp;
	HANDLE hBData;
	alnetflag =1;
//**********************bmp****************************************************//
	hBmp=NULL;
	hBData=NULL;
	char newbmpPath[]="E:\\VC_ICE\\mcinewdata\\7700\\*.BMP";
	char newdataPath[]="E:\\VC_ICE\\mcinewdata\\7400\\*.dat";
	char * fullPathName;

	DWORD sizeBmp;
	DWORD sizeBData;
    SYSTEMTIME now; 
	int newtick;
    int tick1 = GetTickCount();
	int tick2 = GetTickCount();
	     char tempbstr[50];
  


	while (alnetflag )
	{
		if ((bmpEnd==1)&&(bDataEnd==1)) //两个都发送完成
		{ break;
		}

		newtick = GetTickCount();
		if ((newtick - tick1)> 50)
		{
			tick1 = newtick;
			bmpsend_flag = 0;	
			datasend_flag = 1;
		}
// 		if (newtick - tick2 > 1000)
// 		{
// 			tick2 = newtick;
// 			
//		}
//*******************************开始发送bmp***************************

	if ((bmpsend_flag == 1)&&(bmpEnd==0))
		{
			GetLocalTime(&now);
	
		
			
//	        filName=new char [255];   //文件名字
		

       
			ret=0;

			if (hBmp==NULL)
			{hFind1=NULL;
			filName=NULL;
			}
			else
				hFind1=hBmp;

            sizeBmp=FindFile(newbmpPath);
  //          bmpEnd=0;
//文件数据发送完毕
            if (sizeBmp==0)
            {
				bmpEnd=1;
            }
	//找到文件就执行发送
		if (bmpEnd==0)
			{
			fullPathName=new char [200];
			char drive[5];
			char dir[100];
			char exp[6];
			
		    hBmp=hFind1; 
			memset(fullPathName,0,200);	
			_splitpath(newbmpPath,drive,dir,NULL,exp);
			strcpy(fullPathName,drive);
			strcat(fullPathName,dir);
		    strcat(fullPathName,filName);
            
//			filName=NULL;
			delete [] filName;
 //           strcat(fullPathName,exp);
//*******************************读入大小和bmp*****************************
			CFile   m_file; 
	
			  int   filelen   =   0;   
			  m_file.Open(fullPathName,CFile::modeRead);   
			  sizeBmp= m_file.GetLength();   
              m_file.Close();

            char * bmpData;
			bmpData=(char *)malloc(sizeBmp);
			memset(bmpData,0,sizeBmp);
			FILE * fp1=fopen((char *)fullPathName,"r+");
			if (fp1!=NULL)
			{	
				fread(bmpData,100,1,fp1);//128*128
			    //* (bmpData+sizeBmp)='\0';
		    	fclose(fp1);
			}

//			fullPathName=NULL;
			delete [] fullPathName;


//******************得到bmpdata  开始循环****************************************          //

		   	char bmpstr[95];

		  
		   int sublen;
		   short temp;
           double tempsecont;
		   long templ;
    
		   int length;
		   length=sizeBmp;
//		   length=33846;//22538;//33846;

           PBYTE sBmpBuf;
		   sBmpBuf = (PBYTE) malloc(length);
		   INT sBmpBufAddr;
	       sBmpBufAddr = (INT) sBmpBuf;
           

		   PBYTE UseBuf;
		   UseBuf = (PBYTE)malloc(4096 + 94);
		   INT UseBufAddr;
	       UseBufAddr = (INT) UseBuf;

		   memcpy(sBmpBuf,bmpData,sizeBmp);

          sublen=sizeBmp;
			bmpEnd=0;
			bDataEnd=0;
			netflag=1;
			bmpsend_flag = 1;
	//////////////////////////一个bmp的循环发送
	
			count1++;
			sprintf(tempbstr, "bmp send:%8d", count1);
			netdc->TextOut(10, 40, tempbstr);			
			// get image from card buf

			// ImageBuf
             count2=0;
			for (int i = 0; ; i++)
			{
                 Sleep(120);
                count2++;
				sBmpBuf = (PBYTE) (sBmpBufAddr);
				UseBuf = (PBYTE) (UseBufAddr);

				if (sublen > 4096)
				{
					temp = 0x0202;
					memcpy(bmpstr,&temp,2);
					
					temp = 0x0102;
					memcpy(bmpstr+2,&temp,2);
					
					temp = 0x0000;
					memcpy(bmpstr+4,&temp,2);
					
					temp = 0x2177;
					memcpy(bmpstr+6,&temp,2);
					
					temp = 4096+84;
					memcpy(bmpstr+8,&temp,2);
					
					temp=now.wHour;
					memcpy(bmpstr+10,&temp,2);
					
					temp=now.wMinute;
					memcpy(bmpstr+12,&temp,2);
					
					tempsecont=(double)(now.wSecond + 1.0*now.wMilliseconds/1000);
					memcpy(bmpstr+14,&tempsecont,8);
					
					char filename[65];

					for(int k=0;k<64;k++) filename[k]=' ';


					sprintf(filename,"%04d%02d%02d%02d%02d%02d_999999_0202_0201_%04d.BMP                        ",
						now.wYear,now.wMonth,now.wDay,now.wHour,now.wMinute,now.wSecond,count2);
					memcpy(bmpstr+22,&filename,64);


					templ=17;
					memcpy(bmpstr+86,&templ,4);
					templ=i+1;
					memcpy(bmpstr+90,&templ,4);	
					
					CopyMemory(UseBuf, bmpstr, 94);
					UseBuf = UseBuf + 94;
					sBmpBuf = sBmpBuf + 4096 * i;
					
		
					CopyMemory(UseBuf, sBmpBuf, 4096);
					UseBuf = (PBYTE) UseBufAddr;
					
					rt=0;
					rt=sendto(sockClient, (char *) UseBuf,4096+94, 0,
						(SOCKADDR *) &addrClient, sizeof(SOCKADDR));
				}
				if (sublen < 4096)
				{
					temp = 0x0202;
					memcpy(bmpstr,&temp,2);
					
					temp = 0x0102;
					memcpy(bmpstr+2,&temp,2);
					
					temp = 0x0000;
					memcpy(bmpstr+4,&temp,2);
					
					temp = 0x2177;
					memcpy(bmpstr+6,&temp,2);
					
					temp = sublen+84;
					memcpy(bmpstr+8,&temp,2);
					
					temp=now.wHour;
					memcpy(bmpstr+10,&temp,2);
					
					temp=now.wMinute;
					memcpy(bmpstr+12,&temp,2);
					
					tempsecont=(double)(now.wSecond + 1.0*now.wMilliseconds/1000);
					memcpy(bmpstr+14,&tempsecont,8);
					char filename[65];
					for(int k=0;k<65;k++) filename[k]=' ';
					sprintf(filename,"%04d%02d%02d%02d%02d%02d_999999_0202_0201_%04d.BMP                        ",
						now.wYear,now.wMonth,now.wDay,now.wHour,now.wMinute,now.wSecond,count2);
					memcpy(bmpstr+22,&filename,64);
					
					
					templ=17;
					memcpy(bmpstr+86,&templ,4);
					templ=i+1;
					memcpy(bmpstr+90,&templ,4);	

					CopyMemory(UseBuf, bmpstr, 94);
					UseBuf = UseBuf + 94;
					sBmpBuf = sBmpBuf + 4096 * i;
//					netdc->TextOut(10, 160, bmpstr);
					
					CopyMemory(UseBuf, sBmpBuf, sublen);
					UseBuf = (PBYTE) UseBufAddr;
					rt=sendto(sockClient, (char *) UseBuf, sublen + 94, 0,
						(SOCKADDR *) &addrClient, sizeof(SOCKADDR));
					break;
				}//循环发送bmp里面的数据
				sublen -= 4096;


			}    //for循环结束 1个bmp中的1个数据发送完成

			bmpsend_flag=0;

			sBmpBuf=NULL;
			UseBuf=NULL;
			bmpData=NULL;
			
			free(sBmpBuf);
			free(UseBuf);
			free(bmpData);
			
			delete [] sBmpBuf;
			delete [] UseBuf;
		} //可以发送bmp 
   } //发送bmp程序结束




				
//******************************************发送data*******************************************************
// 		   	//调用并发送data数据 发送1个bmp同时发送data
	if ((datasend_flag==1)&&(bDataEnd==0))
	{

	

		short temp;
		double tempd;
		long templ;
		double tempsecont;
	
		 GetLocalTime(&now);


 
// 		 //数据开始		 
// 
// //**************找到data文件存放路径********************		 
		

// 
 			 if (hBData==NULL)
 			 {
				 hFind1=NULL;
			     filName=NULL;
			 }
			 else
				 hFind1=hBData;

         
		 sizeBData=FindFile(newdataPath);
// 
// //文件数据发送完毕
			 if (sizeBData==0)
			 {
				 bDataEnd=1;
			 }	 

//************************读取数据******************
		if (bmpEnd==0)
			{

				char tempdstr [90];

    

				char dataStr[94];
				
				
				char drive[5];
				char dir[100];
				char exp[6];
				int bDlength;
			
				fullPathName=new char [200];
				
				memset(fullPathName,0,200);
				hBData=hFind1; 
					
				_splitpath(newdataPath,drive,dir,NULL,exp);
				strcpy(fullPathName,drive);
				strcat(fullPathName,dir);
				strcat(fullPathName,filName);
            
			    filName=NULL;
		    	delete [] filName;

// //*******************************读入大小和bmp*****************************
			  CFile   bdatafile; 
			  int   filelen   =   0;   
			  bdatafile.Open(fullPathName,CFile::modeRead);   
			  //sizeBData=bdatafile.GetLength();   
			  sizeBData = 100;
              bdatafile.Close();

		    char * bDataBuf;
			bDataBuf=(char *)malloc(sizeBData);
			memset(bDataBuf,0,sizeBData);

			FILE * fp1=fopen((char *)fullPathName,"rb+");
			if (fp1!=NULL)
			{	
				fread(bDataBuf,sizeBData,1,fp1);
			    //* (bDataBuf+sizeBData)='\0';
		    	fclose(fp1);
			}

			fullPathName=NULL;
			delete [] fullPathName;

			bDlength=sizeBData+22;


			PBYTE sBDataBuf;
			sBDataBuf = (PBYTE)malloc(sizeBData);
			INT sBDataBufAddr;
			sBDataBufAddr = (INT) sBDataBuf;
			
			PBYTE TMBuf;
			TMBuf = (PBYTE)malloc(bDlength+23);
			INT TMBufAddr;
			TMBufAddr = (INT) TMBuf;

			memset(sBDataBuf,0,sizeBData+1);
			memset(TMBuf,0,bDlength+1);
			memcpy(sBDataBuf,bDataBuf,sizeBData);
  


			countd++;
			sprintf(tempdstr, "data send:%8d",countd);
			netdc->TextOut(10,100, tempdstr);
// 			// send track and messure data
// 			// 10 times/s
// 			// time 12 bytes
// 
		 temp = 0x0202;
		 memcpy(dataStr,&temp,2);
		 
		 temp = 0x0102;
		 memcpy(dataStr+2,&temp,2);
		 
		 temp = 0x0000;
		 memcpy(dataStr+4,&temp,2);
		 
		 temp = 0x1177;
		 memcpy(dataStr+6,&temp,2);
		 
		 temp = 88;
		 memcpy(dataStr+8,&temp,2);
		 
		 temp=now.wHour;
		 memcpy(dataStr+10,&temp,2);
		 
		 temp=now.wMinute;
		 memcpy(dataStr+12,&temp,2);
		 
		 tempsecont=(double)(now.wSecond + 1.0*now.wMilliseconds/1000);
		 memcpy(dataStr+14,&tempsecont,8);
				
						 
		 memcpy(TMBuf, dataStr, 22);
		 TMBuf=TMBuf+22;
		 memcpy(TMBuf,sBDataBuf,sizeBData);
         TMBuf = (PBYTE) TMBufAddr;

	     rt=0;
	   	 rt=sendto(sockClient, (char *)TMBuf,bDlength,0,
				(SOCKADDR *) &addrClient, sizeof(SOCKADDR));
         
		 Sleep(2);

		    bDataBuf=NULL;
			sBDataBuf=NULL;
			TMBuf=NULL;
// 
// 			
			free(sBDataBuf);
			free(TMBuf);
			 free(bDataBuf);
// 			 
 			delete [] sBDataBuf;
//			delete [] TMBuf;
 		 
		}//data数据发送完成
		datasend_flag=0;
				 
	
		Sleep(20);
	 }  //1个data发送完成

	}  //所有循环结束	

	closesocket(sockClient);
	WSACleanup();
	return 1;
}

void CNetSendDlg::OnBtnSend()
{
	netflag = 1;
	CDC* dc;
	
	dc = GetDC();
	AfxBeginThread(NetSendThread, (LPVOID) dc);
//	AfxBeginThread(NetGetThread, (LPVOID) dc);
}

void CNetSendDlg::OnBtnStop()
{
	alnetflag = 0;
}

//1返回文件大小  0为没有找到文件
DWORD  FindFile(char * dataPath)
{

//    filName=new char [255];

	char * newPath;
    newPath=dataPath;
    filName=new char[255] ;
	memset(filName,0,255);
	WIN32_FIND_DATA   fd;
	DWORD fileSize;
	int rt;
	fileSize=1;
	if (hFind1==NULL)
	{
		hFind1=FindFirstFile(newPath,&fd);	
		strcpy(filName,fd.cFileName);//,strlen(fd.cFileName)+1);
//		filName=NULL;
//        delete [] filName;
	}

	else	
	{   
        hFind2=FindNextFile(hFind1,&fd);	
	    if (hFind2==0)
	    {
			filName=NULL;
			delete [] filName;
			return 0;
	    }
		strcpy(filName,fd.cFileName);
  
		return (fileSize);
	}

	return (fileSize);

}

 //************************************************得到文件数据并发送************************************************//	
void SendData(char * dataPath)
{
	     
// 		 char dataStr[14];//文件头
// 
// 		 sBuf=new byte[35];
// 	     char tempstr[10];
// 		 short temp;
// 		 SYSTEMTIME now;
// 		 GetLocalTime(&now);
// 		 double tempsecont;
// 
// 		 //数据开始
// 		 char * fullPathName;
// 		 fullPathName=new char [60];
// 		 fileName=new char [30];   //文件名字
// 		 char drive[5],dir[40],exp[6];
// 		 char newPath[]="E:\\yang\\资料\\mcinewdata\\mcinewdata\\7701d\\*.dat";
// 		 FindFile(newPath);
// 		 
// 		 HANDLE hBData=NULL;
// 		 unsigned int sizeBData;
// 		 if (hBData==NULL)
// 		 {hFind1=NULL;
// 		 fileName=NULL;
// 		 }
// 		 else
// 			 hFind1=hBData;
// 		 
// 		 sizeBData=FindFile(newPath);
// 		 bDataEnd=0;
// 
// //文件数据发送完毕
// 		 if (sizeBData==0)
// 		 {
// 			 bDataEnd=1;
// 		 }	 
// 
// 		 if (bDataEnd==1)
// 		 {
// 			 //				datasend_flag=0;
// 			 delete [] sBuf;
// 			 delete [] fileName;
// 			 
// 			 return;
// 		 };
// 
// 
// 		 hBData=hFind1; 
// 
// 				
// 		 temp = 0x0202;
// 		 memcpy(dataStr,&temp,2);
// 		 
// 		 temp = 0x0102;
// 		 memcpy(dataStr+2,&temp,2);
// 		 
// 		 temp = 0x0000;
// 		 memcpy(dataStr+4,&temp,2);
// 		 
// 		 temp = 0x1177;
// 		 memcpy(dataStr+6,&temp,2);
// 		 
// 		 temp = 88;
// 		 memcpy(dataStr+8,&temp,2);
// 		 
// 		 temp=now.wHour;
// 		 memcpy(dataStr+10,&temp,2);
// 		 
// 		 temp=now.wMinute;
// 		 memcpy(dataStr+12,&temp,2);
// 		 
// 		 tempsecont=(double)(now.wSecond + 1.0*now.wMilliseconds/1000);
// 		 memcpy(dataStr+14,&tempsecont,8);
// 			
// 				
// 	
// 				
// 		 _splitpath(newPath,drive,dir,NULL,exp);
// 		 strcpy(fullPathName,drive);
// 		 strcat(fullPathName,dir);
// 		 strcat(fullPathName,fileName);
// 		 strcat(fullPathName,exp);
// 		 
// 		 char * data;
// 		 data=new char[21];
// 		 FILE * fp1=fopen((char *)fullPathName,"rb+");
// 
// 		 if (fp1!=NULL)
// 		 {	
// 			 fread(data,20,1,fp1);
// 			 * (data+20)='\0';
// 			 fclose(fp1);
// 		 }
// 		 
// 		 memcpy(sBuf, dataStr, 14);
// 		 sBuf=sBuf+14;
// 		 memcpy(sBuf, data, 20);
// 		 
// 		 delete [] sBuf;
// 		 //                delete [] dataStr;
// 		 //				delete [] data;
// 		 delete [] fileName;
// 			//发送数据循环完成

}