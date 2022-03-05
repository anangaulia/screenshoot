#include "stdafx.h"
#define _WIN32_WINNT 0x0500

//#include <cstdlib>
//#include <iostream>
//#include <sstream>
//#include <fstream>
#include <ctime>
#include <windows.h>
//#include <Winuser.h>
#include <GdiPlus.h>
#pragma comment( lib, "GDIPLUS" )

using namespace std;

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	using namespace Gdiplus;
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}    
	}

	free(pImageCodecInfo);
	return 0;
}

void screencap()
{
     SYSTEMTIME SysTime;
     GetLocalTime(&SysTime);
     wchar_t filename[200];
     memset(filename,0,sizeof(filename));
     wsprintfW(filename,L"Screenshot_%02d_%02d_%02d_-_%02d_%02d_%02d.png",SysTime.wHour,SysTime.wMinute,SysTime.wSecond,SysTime.wDay,SysTime.wMonth,SysTime.wYear);


	using namespace Gdiplus;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	{
		HDC scrdc, memdc;
		HBITMAP membit;
		scrdc = ::GetDC(0);
		int Height = GetSystemMetrics(SM_CYSCREEN);
		int Width = GetSystemMetrics(SM_CXSCREEN);
		memdc = CreateCompatibleDC(scrdc);
		membit = CreateCompatibleBitmap(scrdc, Width, Height);
		HBITMAP hOldBitmap =(HBITMAP) SelectObject(memdc, membit);
		BitBlt(memdc, 0, 0, Width, Height, scrdc, 0, 0, SRCCOPY);

		
		Gdiplus::Bitmap bitmap(membit, NULL);
		CLSID jpgclsid;
		GetEncoderClsid(L"image/png", &jpgclsid);
		bitmap.Save(filename, &jpgclsid, NULL);

		SelectObject(memdc, hOldBitmap);

		DeleteObject(memdc);

		DeleteObject(membit);

		::ReleaseDC(0,scrdc);
	}

	GdiplusShutdown(gdiplusToken);
}


/*GLOBALS*/
int counter;

int RegStartup()
{
	
    long result;
    HKEY key;

    // Write HKCU Registry
    result = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_READ|KEY_WRITE|KEY_QUERY_VALUE, &key);
    if (result != ERROR_SUCCESS) return 1;

    // Get Current location of Keylogger
    char filepath[MAX_PATH];
    GetModuleFileName(NULL, filepath, MAX_PATH);

    // Register the values
    LPCTSTR data = filepath;
    result = RegSetValueEx(key, "Print Screen Service", 0, REG_SZ, (LPBYTE)data, strlen(data)+1);
    if (result != ERROR_SUCCESS) return 1;

    // Close Registry
    RegCloseKey(key);
    return 0;
}


/*MAIN*/
int main(int argc, char *argv[])

//int APIENTRY WinMain(HINSTANCE hInstance,
//                     HINSTANCE hPrevInstance,
//                     LPTSTR    lpCmdLine,
//                     int       nCmdShow)
{
	unsigned char c;
	
	counter = 0;

	RegStartup();

	while (1){
		Sleep(10);
		for (c = 1; c < 255; c++){
			SHORT rv = GetAsyncKeyState(c);
			if (rv & 0x0001) {
    			switch (c) {
				case 0x2C:
					screencap();
                    break;
				};

			}
		}
	}

	system("PAUSE");
	return 0;
}

