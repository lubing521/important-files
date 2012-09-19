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

double A = 100.004;
double E = 0.456;
double DA = 3.05;
double DE = 3.07;


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
	int i;
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
	addrClient.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addrClient.sin_family = AF_INET;
	addrClient.sin_port = htons(11020);
	SOCKADDR_IN addrSrv;

	BITMAPFILEHEADER SaveFileHeader;
	BITMAPINFOHEADER SaveInfoHeader;
	CFile SaveFile;
	CString FileName;
	FileName = "c:\\temp.bmp";
	if (SaveFile.Open(FileName, CFile::modeRead))
	{
		SaveFile.Close();
		SaveFile.Open(FileName, CFile::modeWrite);
	}
	else
	{
		SaveFile.Open(FileName, CFile::modeCreate | CFile::modeWrite);
	}

	// set bmp file header
	SaveFileHeader.bfType = 0x4d42;
	SaveFileHeader.bfSize = sizeof(BITMAPFILEHEADER) +
		sizeof(BITMAPINFOHEADER) +
		256 * 256 +
		sizeof(RGBQUAD) * 256;
	SaveFileHeader.bfReserved1 = 0;
	SaveFileHeader.bfReserved2 = 0;
	SaveFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) +
		sizeof(BITMAPINFOHEADER) +
		sizeof(RGBQUAD) * 256;
	// set bmp info header
	SaveInfoHeader.biSize = 40L;			
	SaveInfoHeader.biWidth = 256;
	SaveInfoHeader.biHeight = 256;
	SaveInfoHeader.biPlanes = 1L;
	SaveInfoHeader.biBitCount = 8L;
	SaveInfoHeader.biCompression = 0L;
	SaveInfoHeader.biSizeImage = 0;//(long)iWid*iHei;
	SaveInfoHeader.biXPelsPerMeter = 0;//0xbc;
	SaveInfoHeader.biYPelsPerMeter = 0;//0xbc;.
	SaveInfoHeader.biClrUsed = 0;//256;	
	SaveInfoHeader.biClrImportant = 0;
	// set color index
	RGBQUAD argbq[256];
	for (int i = 0; i <= 255; i++)
	{
		argbq[i].rgbRed = i;
		argbq[i].rgbGreen = i;
		argbq[i].rgbBlue = i;
		argbq[i].rgbReserved = 0;
	}
	PBYTE ImageBuf;
	ImageBuf = (PBYTE) LocalAlloc(LPTR, 256 * 256);
	for (int k = 0; k < 256; k++)
		for (int j = 255; j >= 0; j--)
		{
			*(ImageBuf + k * 256 + j) = k;
		}
	// save data
	SaveFile.Write(&SaveFileHeader, sizeof(BITMAPFILEHEADER));
	SaveFile.Write(&SaveInfoHeader, sizeof(BITMAPINFOHEADER));
	SaveFile.Write(argbq, sizeof(RGBQUAD) * 256);	
	SaveFile.Write(ImageBuf, 256 * 256);
	SaveFile.Close();

	INT length;
	length = sizeof(BITMAPFILEHEADER) +
		sizeof(BITMAPINFOHEADER) +
		sizeof(RGBQUAD) * 256 +
		256 * 256;

	PBYTE SendBuf;
	SendBuf = (PBYTE) LocalAlloc(LPTR, length);
	INT SendBufAddr;
	SendBufAddr = (INT) SendBuf;

	INT headlen;
	headlen = sizeof(BITMAPFILEHEADER) +
		sizeof(BITMAPINFOHEADER) +
		sizeof(RGBQUAD) * 256;
	PBYTE HeadBuf;
	HeadBuf = (PBYTE) LocalAlloc(LPTR, headlen);


	PBYTE UseBuf;
	UseBuf = (PBYTE) LocalAlloc(LPTR, 4096 + 94);
	INT UseBufAddr;
	UseBufAddr = (INT) UseBuf;

	PBYTE TMBuf;
	TMBuf = (PBYTE) LocalAlloc(LPTR, 98);
	INT TMBufAddr;
	TMBufAddr = (INT) TMBuf;

	SaveFile.Open(FileName, CFile::modeRead);
	SaveFile.Read(HeadBuf, headlen);
	SaveFile.Close();
	
	SYSTEMTIME now;
	short temp;
	double tempd;
	long templ;
	double tempsecont;

	int tick1 = GetTickCount();
	int tick2 = tick1;
	int newtick;

	int bmpsend_flag = 0;
	int datasend_flag = 0;

	int count1, count2;
	count1 = 0;
	count2 = 0;

	char tempstr[90];
	char bmpstr[94];
	char str[98];	
	for(i=0;i<90;i++) tempstr[i]=0;
	for(i=0;i<94;i++) bmpstr[i]=0;
	for(i=0;i<98;i++) str[i]=0;


	while (netflag)
	{
		// tick judge
		newtick = GetTickCount();
		if (newtick - tick1 > 100)
		{
			tick1 = newtick;
			datasend_flag = 1;
		}
		if (newtick - tick2 > 2000)
		{
			tick2 = newtick;
			bmpsend_flag = 1;
		}

		if (bmpsend_flag == 1)
		{
			GetLocalTime(&now);
			
			count1++;
			sprintf(tempstr, "bmp send:%8d", count1);
			netdc->TextOut(10, 40, tempstr);			
			// get image from card buf

			// ImageBuf
			for (int k = 0; k < 256; k++)
			{
				for (int j = 255; j >= 0; j--)
				{
					*(ImageBuf+k*256+j)=(UCHAR)(k*j);
					//*(ImageBuf + k * 256 + j) = (UCHAR) rand();
				}
			}
			//

			SendBuf = PBYTE(SendBufAddr);
			//CopyMemory(SendBuf,SaveFileHeader,sizeof(BITMAPFILEHEADER));
			//CopyMemory(SendBuf,SaveInfoHeader,sizeof(BITMAPINFOHEADER));
			CopyMemory(SendBuf, HeadBuf, headlen);
			//CopyMemory(SendBuf,argbq,256*sizeof(RGBQUAD));
			SendBuf = SendBuf + headlen;
			CopyMemory(SendBuf, ImageBuf, 256 * 256);


			INT sublen;			
			sublen = length - 0;
			int sendlength;

			for (int i = 0; ; i++)
			{

//				sprintf(bmpstr, "%02d%02d%02d%03d000%060d.bmp%04d%04d", thour,
//					tminute, tsecond, count1,tMilliseconds,17, i + 1);

				SendBuf = (PBYTE) (SendBufAddr);
				UseBuf = (PBYTE) (UseBufAddr);
				if (sublen > 4096)
				{
					temp = 0x0202;
					memcpy(bmpstr,&temp,2);
					
					temp = 0x0102;
					memcpy(bmpstr+2,&temp,2);
					
					temp = 0x0000;
					memcpy(bmpstr+4,&temp,2);
					
					temp = 0x0077;
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
/*
					templ=now.wYear;
					memcpy(filename,&templ,4);
					
					temp=now.wMonth;
					memcpy(filename+4,&temp,2);
					
					temp=now.wDay;
					memcpy(filename+6,&temp,2);
					
					temp=now.wHour;
					memcpy(filename+8,&temp,2);
					
					temp=now.wMinute;
					memcpy(filename+10,&temp,2);
					
					temp=now.wSecond;
					memcpy(filename+12,&temp,2);
					
					filename[14]='_';
					
					templ=9999;
					temp=99;
					memcpy(filename+15,&temp,2);
					memcpy(filename+17,&templ,4);
					
					templ=0x0202;
					memcpy(filename+21,&templ,4);
					
					filename[25]='_';
					
					templ=0x0102;
					memcpy(filename+26,&templ,4);
					
					filename[30]='_';
					
					templ=count1;
					memcpy(filename+31,&templ,4);
					
					filename[35]='.';
					filename[36]='B';
					filename[37]='M';
					filename[38]='P';
					for(k=39;k<64;k++) filename[k]=' ';
*/
					memcpy(bmpstr+90,&templ,4);
					sprintf(filename,"%04d%02d%02d%02d%02d%02d_999999_0202_0201_%04d.BMP                        ",
						now.wYear,now.wMonth,now.wDay,now.wHour,now.wMinute,now.wSecond,count1);
					memcpy(bmpstr+22,&filename,64);

/*
					templ=count1;
					memcpy(bmpstr+82,&templ,4);
*/

					templ=17;
					memcpy(bmpstr+86,&templ,4);
					templ=i+1;
					memcpy(bmpstr+90,&templ,4);	
					
					CopyMemory(UseBuf, bmpstr, 94);
					UseBuf = UseBuf + 94;
					SendBuf = SendBuf + 4096 * i;
					netdc->TextOut(10, 160, bmpstr);
					
					CopyMemory(UseBuf, SendBuf, 4096);
					UseBuf = (PBYTE) UseBufAddr;
					sendlength=sendto(sockClient, (char *) UseBuf, 4096 + 94, 0,
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
					
					temp = 0x0077;
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
						now.wYear,now.wMonth,now.wDay,now.wHour,now.wMinute,now.wSecond,count1);
					memcpy(bmpstr+22,&filename,64);
					
//					templ=count1;
//					memcpy(bmpstr+82,&templ,4);
					
					templ=17;
					memcpy(bmpstr+86,&templ,4);
					templ=i+1;
					memcpy(bmpstr+90,&templ,4);	

					CopyMemory(UseBuf, bmpstr, 94);
					UseBuf = UseBuf + 94;
					SendBuf = SendBuf + 4096 * i;
					netdc->TextOut(10, 160, bmpstr);

					CopyMemory(UseBuf, SendBuf, sublen);
					UseBuf = (PBYTE) UseBufAddr;
					sendlength=sendto(sockClient, (char *) UseBuf, sublen + 94, 0,
						(SOCKADDR *) &addrClient, sizeof(SOCKADDR));
					break;
				}
				sublen -= 4096;
			}

			bmpsend_flag = 0;
		}

		if (datasend_flag == 1)
		{
			count2++;
			sprintf(tempstr, "data send:%8d", count2);
			netdc->TextOut(10, 80, tempstr);
			// send track and messure data
			// 10 times/s
			// time 12 bytes
			temp = 0x0202;
			memcpy(str,&temp,2);
			
			temp = 0x0102;
			memcpy(str+2,&temp,2);
			
			temp = 0x0000;
			memcpy(str+4,&temp,2);
			
			temp = 0x0074;
			memcpy(str+6,&temp,2);
			
			temp = 88;
			memcpy(str+8,&temp,2);
			
			temp=now.wHour;
			memcpy(str+10,&temp,2);
			
			temp=now.wMinute;
			memcpy(str+12,&temp,2);
			
			tempsecont=(double)(now.wSecond + 1.0*now.wMilliseconds/1000);
			memcpy(str+14,&tempsecont,8);
			
			// A E 8bytes float
			A = 15.132112;//1.234;
//			A += 1.234;
			if (A >= 360.00)
				A -= 360.00;
//			E += 2.345;
			E = 15.132112;//2.345;
			if (E >= 180.00)
				E -= 180.00;
			// A. E. 8bytes fill 9

			// DA DE 8bytes float
			DA = (double)0.132112;//5;// rand() / 10000;
			DE = (double)0.132112;// rand() / 10000;
			memcpy(str+22,&A,8);
			memcpy(str+30,&E,8);

			tempd=99999999;
			memcpy(str+38,&tempd,8);
			memcpy(str+46,&tempd,8);
			
			memcpy(str+54,&DA,8);
			memcpy(str+62,&DE,8);
			
			templ=9999;
			memcpy(str+70,&templ,4);
			memcpy(str+74,&tempd,8);

			memcpy(str+82,&tempd,8);
			memcpy(str+90,&tempd,8);

			
			// NOUSE 12+8+8 fill 9

			// Fill
			/*
							int temp1=(int)E;
							int temp2=(E-(int)E)*10000;
							int temp3=(int)A;
							int temp4=(A-(int)A)*10000;
							int temp5=(int)DE;
							int temp6=(DE-(int)DE)*10000;
							int temp7=(int)DA;
							int temp8=(DA-(int)DA)*10000;
							*/

			//sprintf(str,"%02d%02d%02d.00000%03d.%04d%03d.%04d9999999999999999%03d.%04d%03d.%04d9999999999999999999999999999",
			//	hour,minute,second,temp1,temp2,temp3,temp4,temp5,temp6,temp7,temp8);
//			sprintf(str,
//				"%02d%02d%02d.00000%08.4f%08.4f9999999999999999%08.4f%08.4f9999999999999999999999999999",
//				hour, minute, second, E, A, DE, DA);
			memcpy(TMBuf, str, 98);
			// send
			sendto(sockClient, (char *) TMBuf, 98, 0,
				(SOCKADDR *) &addrClient, sizeof(SOCKADDR));

			//
			datasend_flag = 0;
			netdc->TextOut(10, 120, str);
		}
	}
	closesocket(sockClient);
	WSACleanup();
	if (ImageBuf != NULL)
		LocalFree(ImageBuf);
	if (SendBuf != NULL)
		LocalFree(SendBuf);
	if (HeadBuf != NULL)
		LocalFree(HeadBuf);
	if (UseBuf != NULL)
		LocalFree(UseBuf);
	if (TMBuf != NULL)
		LocalFree(TMBuf);
	return 1;
}

UINT NetGetThread(LPVOID param)
{
	CDC* netdc;
	netdc = (CDC *) param;
	int index;
	index = 0;

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(1, 1);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		return 0;
	}
	if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
	{
		WSACleanup();
		return 0;
	}

	SOCKET sockSrv = socket(AF_INET, SOCK_DGRAM, 0);
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(15703);

	bind(sockSrv, (SOCKADDR *) &addrSrv, sizeof(SOCKADDR));

	int saveflag = 0;
	CFile savefile;
	int all, piece;
	int i;
	char str[4];
	while (netflag)
	{
		char recvBuf[4180];
		char saveBuf[4096];
		SOCKADDR addrClient;
		int len = sizeof(SOCKADDR);
		recvfrom(sockSrv, recvBuf, 4096 + 84, 0, (SOCKADDR *) &addrClient,
			&len);
		if (recvBuf[84] == 'B' && recvBuf[85] == 'M')
		{
			char filename[12];
			for (i = 64; i < 76; i++)
			{
				filename[i - 64] = recvBuf[i];
			}
			filename[0] = 'e';
			filename[1] = ':';
			filename[2] = '\\';
			filename[3] = '\\';
			filename[12] = '\0';
			savefile.Open(filename, CFile::modeWrite | CFile::modeCreate);

			//savefile.Open("c:\\tttt.bmp",CFile::modeWrite|CFile::modeCreate);
			saveflag = 1;
		}
		if (saveflag == 1)
		{
			for (i = 84; i < 4180; i++)
			{
				saveBuf[i - 84] = recvBuf[i];
			}
			savefile.Write(saveBuf, sizeof(saveBuf));
			for (i = 76; i < 80; i++)
			{
				str[i - 76] = recvBuf[i];
			}
			all = atoi(str);
			for (i = 80; i < 84; i++)
			{
				str[i - 80] = recvBuf[i];
			}
			piece = atoi(str);
			if (all == piece)
			{
				savefile.Close();
				saveflag = 0;
				//netflag=0;
			}
		}
	}
	closesocket(sockSrv);
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
	netflag = 0;
}































