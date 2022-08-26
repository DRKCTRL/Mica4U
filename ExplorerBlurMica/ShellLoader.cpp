/*
* BHO���������
*
* Author: Maple
* date: 2022-1-31 Create
* Copyright winmoes.com
*/

#include "ShellLoader.h"

long g_cDllRef = 0;

/*������޸��˴��� ��ʹ��VS��GUID���������µ�GUID��*/
const std::wstring CLSID_SHELL_BHO_STR = L"{B44BD3C8-E597-4E08-AE43-246CE24698E7}";
const CLSID CLSID_SHELL_BHO = { 0xb44bd3c8, 0xe597, 0x4e08, { 0xae, 0x43, 0x24, 0x6c, 0xe2, 0x46, 0x98, 0xe7 } };

#pragma region CObjectWithSite

CObjectWithSite::CObjectWithSite()
{
	OnWindowLoad();
}

CObjectWithSite::~CObjectWithSite()
{
	ReleaseRes();
}

STDMETHODIMP CObjectWithSite::QueryInterface(REFIID riid, void** ppv)
{
	if (riid == IID_IUnknown)
		*ppv = static_cast<CObjectWithSite*>(this);
	else if (riid == IID_IObjectWithSite)
		*ppv = static_cast<IObjectWithSite*>(this);
	else
		return E_NOINTERFACE;
	AddRef();
	return S_OK;
}

ULONG __stdcall CObjectWithSite::AddRef()
{
	InterlockedIncrement(&g_cDllRef);
	return InterlockedIncrement(&m_ref);
}

ULONG __stdcall CObjectWithSite::Release()
{
	int tmp = InterlockedDecrement(&m_ref);
	if (tmp == 0)
		delete this;
	InterlockedDecrement(&g_cDllRef);
	return tmp;
}


STDMETHODIMP CObjectWithSite::SetSite(IUnknown* pUnkSite)
{
	ReleaseRes();
	HRESULT hr = pUnkSite->QueryInterface(IID_IWebBrowser2, (void**)&m_web);
	if (FAILED(hr))
		ReleaseRes();
	return hr;
}

void CObjectWithSite::ReleaseRes()
{
	if (m_web) m_web->Release();
	m_web = 0;
}

#pragma endregion

#pragma region ClassFactory

STDMETHODIMP ClassFactory::QueryInterface(REFIID riid, void** ppv)
{
	if (riid == IID_IUnknown || riid == IID_IClassFactory)
	{
		*ppv = this;
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

ULONG __stdcall ClassFactory::AddRef()
{
	InterlockedIncrement(&g_cDllRef);
	return InterlockedIncrement(&g_cDllRef);
}

ULONG __stdcall ClassFactory::Release()
{
	int tmp = InterlockedDecrement(&m_ref);
	if (tmp == 0)
		delete this;
	InterlockedDecrement(&g_cDllRef);
	return tmp;
}

IFACEMETHODIMP ClassFactory::LockServer(BOOL fLock)
{
	if (fLock)
		InterlockedIncrement(&g_cDllRef);
	else
		InterlockedDecrement(&g_cDllRef);
	return S_OK;
}

IFACEMETHODIMP ClassFactory::CreateInstance(IUnknown* pUnkOuter, REFIID riid, LPVOID* ppvObj)
{
	*ppvObj = NULL;
	if (pUnkOuter)
		return CLASS_E_NOAGGREGATION;
	CObjectWithSite* bho = new CObjectWithSite();
	bho->AddRef();
	HRESULT hr = bho->QueryInterface(riid, ppvObj);
	bho->Release();
	return hr;
}

#pragma endregion

#pragma region Reg

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppvOut)
{
	static ClassFactory factory;
	*ppvOut = NULL;
	if (rclsid == CLSID_SHELL_BHO) {
		return factory.QueryInterface(riid, ppvOut);
	}
	return CLASS_E_CLASSNOTAVAILABLE;
}

STDAPI DllCanUnloadNow(void)
{
	return (g_cDllRef > 0) ? S_FALSE : S_OK;
}

STDAPI DllRegisterServer()
{
	HKEY hkey = 0;
	TCHAR dllpath[MAX_PATH];
	GetModuleFileNameW(g_hModule, dllpath, MAX_PATH);

	//����CLSID
	std::wstring regpath = L"CLSID\\" + CLSID_SHELL_BHO_STR;
	if (RegCreateKeyExW(HKEY_CLASSES_ROOT, regpath.c_str(), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hkey, NULL) != ERROR_SUCCESS)
		return SELFREG_E_CLASS;

	//����COM�������
	RegSetValueExW(hkey, NULL, 0, REG_SZ, (const BYTE*)L"ExplorerBlurMica BHO", 24 * sizeof(TCHAR));
	RegCloseKey(hkey);

	//����InProcServer32
	if (RegCreateKeyExW(HKEY_CLASSES_ROOT, (regpath + L"\\InProcServer32").c_str(), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hkey, NULL) != ERROR_SUCCESS)
		return SELFREG_E_CLASS;

	//����dllλ��
	RegSetValueExW(hkey, NULL, 0, REG_SZ, (const BYTE*)dllpath, DWORD(wcslen(dllpath) + 1) * sizeof(wchar_t));
	//�����߳�ģ��
	RegSetValueExW(hkey, L"ThreadingModel", 0, REG_SZ, (const BYTE*)L"Apartment", 10 * sizeof(wchar_t));
	RegCloseKey(hkey);

	//ע��BHO���
	if (RegCreateKeyExW(HKEY_LOCAL_MACHINE, (LR"(Software\Microsoft\Windows\CurrentVersion\Explorer\Browser Helper Objects\)"
		+ CLSID_SHELL_BHO_STR).c_str(), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hkey, NULL) != ERROR_SUCCESS)
		return SELFREG_E_CLASS;

	//��ֹIE��������ر����
	DWORD value = 1;
	RegSetValueExW(hkey, L"NoInternetExplorer", 0, REG_DWORD, (const BYTE*)&value, sizeof(DWORD));
	RegCloseKey(hkey);
	return S_OK;
}

STDAPI DllUnregisterServer()
{
	//ɾ��BHO���ע��
	RegDeleteKeyW(HKEY_LOCAL_MACHINE, (LR"(Software\Microsoft\Windows\CurrentVersion\Explorer\Browser Helper Objects\\)"
		+ CLSID_SHELL_BHO_STR).c_str());
	//ɾ��COM���ע��
	RegDeleteKeyW(HKEY_CLASSES_ROOT, (L"CLSID\\" + CLSID_SHELL_BHO_STR + L"\\InProcServer32").c_str());
	RegDeleteKeyW(HKEY_CLASSES_ROOT, (L"CLSID\\" + CLSID_SHELL_BHO_STR).c_str());
	return S_OK;
}

#pragma endregion
