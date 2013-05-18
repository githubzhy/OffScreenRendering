// GDISaveAsBMP.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <GL/glut.h>
#include <afx.h>
#include <afxwin.h>
#include <atlimage.h>
#include <iostream>

using namespace ATL;
using namespace std;

#define BMPWidth 640
#define BMPHeight 640

HBITMAP hBitmap,hOldBitmap;
HDC hScrDC, hMemDC,olddc;
HGLRC oldrc,newrc;

int _tmain(int argc, _TCHAR* argv[])
{
  void InitializeDC();
	void InitializeGL();
	void DrawPolygon();
	void SaveBMP(char*);

	InitializeDC();
	InitializeGL();
	DrawPolygon();
	SaveBMP("D://SaveAsBmp.bmp");
	return 0;
}

void InitializeDC()
{
	// 创建绘图DC及内存DC
	hScrDC = CreateDC(_T("BITMAP"), NULL, NULL, NULL);
	hMemDC = CreateCompatibleDC(hScrDC);
	//设置位图信息
	int iPixel = 32;

	LPBITMAPINFO lpbmih = new BITMAPINFO;
	lpbmih->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	lpbmih->bmiHeader.biWidth = BMPWidth;
	lpbmih->bmiHeader.biHeight = BMPHeight;
	lpbmih->bmiHeader.biPlanes= 1;
	lpbmih->bmiHeader.biBitCount = iPixel;
	lpbmih->bmiHeader.biCompression = BI_RGB;
	lpbmih->bmiHeader.biSizeImage = 0;
	lpbmih->bmiHeader.biXPelsPerMeter = 0;
	lpbmih->bmiHeader.biYPelsPerMeter = 0;
	lpbmih->bmiHeader.biClrUsed= 0;
	lpbmih->bmiHeader.biClrImportant= 0;

	//创建HBITMAP
	BYTE *pBits;
	hBitmap = CreateDIBSection(hMemDC,lpbmih,DIB_PAL_COLORS,(void **)&pBits,NULL,0);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
	CRect rc(0,0,BMPWidth,BMPHeight);
	// 由内存DC创建新的RC，将图形绘制到该内存DC中
	// 创建新的像素格式，用来将图形绘制到位图
	PIXELFORMATDESCRIPTOR pfd= {
		sizeof(PIXELFORMATDESCRIPTOR), // size of this pfd
		1,
		// version number
		PFD_DRAW_TO_WINDOW | // support window
		PFD_SUPPORT_OPENGL | // support OpenGL
		PFD_DRAW_TO_BITMAP | // *** ***此处与绘制到窗口的rc设置不同，无double buffered *** ***
		PFD_SUPPORT_GDI,
		PFD_TYPE_RGBA, // RGBA type
		24,// 24-bit color depth
		0, 0, 0, 0, 0, 0,// color bits ignored
		0,// no alpha buffer
		0,// shift bit ignored
		0,// no accumulation buffer
		0, 0, 0, 0,// accum bits ignored
		32,// 32-bit z-buffer
		0,// no stencil buffer
		0,// no auxiliary buffer
		PFD_MAIN_PLANE, // main layer
		0,// reserved
		0, 0, 0// layer masks ignored
	};

	int pixelFormat=ChoosePixelFormat(hMemDC,&pfd);
	SetPixelFormat(hMemDC,pixelFormat,&pfd);
	// 保存之前绘图的RC及DC
	oldrc = wglGetCurrentContext();
	olddc = wglGetCurrentDC();
	// 创建新的RC
	newrc = wglCreateContext(hMemDC);
	// 设置新的RC及DC
	wglMakeCurrent(hMemDC,newrc);
}


void InitializeGL()
{
	glViewport(0,0,BMPWidth,BMPHeight);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	// 设置投影矩阵，模型视图矩阵，视口	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrtho(-1.0,1.0,-1.0,1.0,-1.0,1.0);
	gluPerspective(45.0,BMPWidth/BMPHeight,0.1f,100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0,0.0,0.0, 0.0,0.0,-1.0, 0.0,1.0,0.0);
}

void DrawPolygon()
{
	glTranslatef(0.0f,0.0f,-20.0f);
	// 清除深度及颜色缓存
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	// 开始绘制
	glShadeModel(GL_SMOOTH);
	glBegin(GL_POLYGON);
	glColor3f(1.0,0.0,0.0);
	glVertex3f(-1.0,1.0,0.0);
	glColor3f(0.0,1.0,0.0);
	glVertex3f(1.0,1.0,0.0);
	glColor3f(0.0,0.0,1.0);
	glVertex3f(1.0,-1.0,0.0);
	glColor3f(1.0,1.0,0.0);
	glVertex3f(-1.0,-1.0,0.0);
	glEnd();
	//结束绘制
	glFinish();

	SwapBuffers(wglGetCurrentDC());
	//恢复之前的RC及DC，释放新创建的RC
	wglMakeCurrent(olddc,oldrc);
	wglDeleteContext(newrc);
	// 完成OPENGL绘制
	hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);
	// 释放DC
	DeleteDC(hScrDC);
	DeleteDC(hMemDC);
}

//保存HBITMAP为文件
void SaveBMP(char* fileName)
{
	CImage image;
	image.Attach(hBitmap);
	HRESULT hResult = image.Save(fileName);
	DeleteObject(hBitmap);
}
