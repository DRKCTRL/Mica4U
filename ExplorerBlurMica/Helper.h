/*
* WinAPI����
*
* Author: Maple
* date: 2021-7-13 Create
* Copyright winmoes.com
*/

#pragma once
#include "framework.h"
#include <Uxtheme.h>

#include <comdef.h>
#include <gdiplus.h>

#include <intrin.h>

#pragma comment(lib, "GdiPlus.lib")
#pragma comment(lib, "uxtheme.lib")

//��ȡ��ǰdll����Ŀ¼
extern std::wstring GetCurDllDir();

//�ж��ļ��Ƿ����
extern bool FileIsExist(std::wstring FilePath);

//��ȡ���ڱ���
extern std::wstring GetWindowTitle(HWND hWnd);

//��ȡ��������
extern std::wstring GetWindowClassName(HWND hWnd);

//��ȡ�����ļ�����
extern std::wstring GetIniString(std::wstring FilePath, std::wstring AppName, std::wstring KeyName);

//�Ƚ���ɫ
extern bool CompareColor(COLORREF color1, COLORREF color2);

//ת����Сд
extern std::wstring ConvertTolower(std::wstring str);

/*��ָ�����ڸ���AeroЧ��
* @param hwnd	 - ���ھ��
* @param win10	 - �Ƿ�ʹ��win10����
* @param Acrylic - �Ƿ������ǿ���Ч��(win10 1803���Ͽ���)
* @param color	 - ���ڱ������ɫ
*/
extern void StartAero(HWND hwnd, bool Acrylic = false, COLORREF color = 0, bool blend = false);

//��ȡ��������
extern std::wstring GetThemeClassName(HTHEME hTheme);

//��麯��������Դ
bool CheckCaller(HMODULE caller, void* address = _ReturnAddress());
bool CheckCaller(LPCWSTR caller, void* address = _ReturnAddress());

//���� Windows 10 Ribbon DPI�߶�
UINT CalcRibbonHeightForDPI(HWND hWnd, UINT src, bool normal = true);

//��ȡע���SZֵ
extern std::wstring RegGetSZ(HKEY hKey, LPCWSTR SubKey, LPCWSTR KeyName);

//ˢ��Windows10����Blur���ڱ߿�����
void RefreshWin10BlurFrame(bool blurType);