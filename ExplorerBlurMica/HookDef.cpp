/*
* Hook������
*
* Author: Maple
* date: 2022-7-3 Create
* Copyright winmoes.com
*/
#include "HookDef.h"
#include <unordered_map>
#include <functional>
#include <vssym32.h>

struct DUIData
{
    HWND hWnd = 0;      //DirectUIHWND
    HWND mainWnd = 0;   //Explorer Window
    HWND TreeWnd = 0;   //TreeView

    //DirectUIHWND
    HDC hDC = 0;
    HDC srcDC = 0;

    SIZE size = { 0,0 };
    bool treeDraw = false;
    bool refresh = true;
};
struct Config
{
    int effType = 0;        //Ч������ 0=Blur 1=Acrylic(Mica)
    COLORREF blendColor = 0;//�����ɫ
    bool smallborder = true;
};

std::unordered_map<DWORD, DUIData> m_DUIList;                  //dui����б�
std::unordered_map<DWORD, std::pair<HWND, HDC>> m_ribbonPaint; //������¼win10 Ribbon�����Ƿ����
std::unordered_map<DWORD, bool> m_drawtextState;               //������¼drawText Alpha�޸�״̬
HBRUSH m_clearBrush = 0;                                       //�������DC�Ļ�ˢ
Config m_config;                                               //�����ļ�


inline COLORREF M_RGBA(BYTE r, BYTE g, BYTE b, BYTE a)
{
    return RGB(r, g, b) | (a << 24);
}

void RefreshConfig()
{
    if (!m_clearBrush)
        m_clearBrush = CreateSolidBrush(0x00000000);
    
    std::wstring path = GetCurDllDir() + L"\\config.ini";
    auto cfg = GetIniString(path, L"config", L"effect");

    //Ч������
    m_config.effType = _wtoi(cfg.c_str());
    if (m_config.effType != 0 && m_config.effType != 1)
        m_config.effType = 0;

    //��ɫRGBA
    auto color = [&path](std::wstring s) -> BYTE
    {
        int c = _wtoi(GetIniString(path, L"blend", s).c_str());
        if (c > 255) c = 255;
        else if (c < 0) c = 0;

        if (m_config.effType && c > 249)
            c = 249;

        return BYTE(c);
    };
    m_config.blendColor = M_RGBA(color(L"r"), color(L"g"), color(L"b"), color(L"a"));

    m_config.smallborder = GetIniString(path, L"config", L"smallBorder") == L"false" ? false : true;

    RefreshWin10BlurFrame(m_config.smallborder);
}

namespace Hook
{
    int hookIndex = 0;

    auto _CreateWindowExW_          = HookDef(CreateWindowExW, MyCreateWindowExW);
    auto _DestroyWindow_            = HookDef(DestroyWindow, MyDestroyWindow);
    auto _BeginPaint_               = HookDef(BeginPaint, MyBeginPaint);
    auto _EndPaint_                 = HookDef(EndPaint, MyEndPaint);
    auto _FillRect_                 = HookDef(FillRect, MyFillRect);
    auto _DrawTextW_                = HookDef(DrawTextW, MyDrawTextW);
    auto _DrawTextExW_              = HookDef(DrawTextExW, MyDrawTextExW);
    auto _ExtTextOutW_              = HookDef(ExtTextOutW, MyExtTextOutW);
    auto _CreateCompatibleDC_       = HookDef(CreateCompatibleDC, MyCreateCompatibleDC);
    auto _GetThemeColor_            = HookDef(GetThemeColor, MyGetThemeColor);
    auto _DrawThemeText_            = HookDef(DrawThemeText, MyDrawThemeText);
    auto _DrawThemeTextEx_          = HookDef(DrawThemeTextEx, MyDrawThemeTextEx);
    auto _DrawThemeBackgroundEx_    = HookDef(DrawThemeBackgroundEx, MyDrawThemeBackgroundEx);

    auto _PatBlt_                   = HookDef(PatBlt, MyPatBlt);

    void HookAttachAll()
    {
        _CreateWindowExW_.Attach();
        _DestroyWindow_.Attach();
        _BeginPaint_.Attach();
        _EndPaint_.Attach();
        _FillRect_.Attach();
        _DrawTextW_.Attach();
        _DrawTextExW_.Attach();
        _ExtTextOutW_.Attach();
        _CreateCompatibleDC_.Attach();
        _GetThemeColor_.Attach();
        _DrawThemeText_.Attach();
        _DrawThemeTextEx_.Attach();
        _DrawThemeBackgroundEx_.Attach();
        _PatBlt_.Attach();
    }

    void HookDetachAll()
    {
        MH_DisableHook(MH_ALL_HOOKS);
        MH_Uninitialize();
    }

    //�Ƿ�Ϊdui�߳�
    bool IsDUIThread()
    {
        auto iter = m_DUIList.find(GetCurrentThreadId());
        return iter != m_DUIList.end();
    }

    HWND MyCreateWindowExW(
        DWORD     dwExStyle,
        LPCWSTR   lpClassName,
        LPCWSTR   lpWindowName,
        DWORD     dwStyle,
        int       X,
        int       Y,
        int       nWidth,
        int       nHeight,
        HWND      hWndParent,
        HMENU     hMenu,
        HINSTANCE hInstance,
        LPVOID    lpParam
    )
    {

        HWND hWnd = _CreateWindowExW_.Org(dwExStyle, lpClassName, lpWindowName, dwStyle,
            X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

        std::wstring ClassName;
        if (hWnd)
        {
            ClassName = GetWindowClassName(hWnd);
        }

        //�޸�Blur��Edit��Alpha
        if (IsDUIThread()) {
            if (ConvertTolower(ClassName) == L"edit")
            {
                SetWindowLongW(hWnd, GWL_EXSTYLE, GetWindowLongW(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
                SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 255, LWA_ALPHA);
            }
        }

        //explorer window 
        if (ClassName == L"DirectUIHWND" && GetWindowClassName(hWndParent) == L"SHELLDLL_DefView")
        {
            //�������Ҹ���
            HWND parent = GetParent(hWndParent);
            if (GetWindowClassName(parent) == L"ShellTabWindowClass")
            {
                parent = GetParent(parent);

                //����Blur
                StartAero(parent, m_config.effType == 1, m_config.blendColor, m_config.blendColor != 0);

                //��¼���б��� Add to list
                DWORD tid = GetCurrentThreadId();

                DUIData data;
                data.hWnd = hWnd;
                data.mainWnd = parent;

                m_DUIList[tid] = data;
            }
        }
        //��������ͼ
        else if (ClassName == L"SysTreeView32")
        {
            HWND parent = GetParent(hWndParent);//NamespaceTreeControl
            parent = GetParent(parent);         //CtrlNotifySink
            parent = GetParent(parent);         //DirectUIHWND
            parent = GetParent(parent);         //DUIViewWndClassName
            std::wstring mainWndCls = GetWindowClassName(parent);

            if (mainWndCls == L"ShellTabWindowClass")
                m_DUIList[GetCurrentThreadId()].TreeWnd = hWnd;
        }
        return hWnd;
    }

    BOOL MyDestroyWindow(HWND hWnd)
    {
        //���Ҳ�ɾ���б��еļ�¼
        auto iter = m_DUIList.find(GetCurrentThreadId());
        if (iter != m_DUIList.end())
        {
            if (iter->second.hWnd == hWnd) {
                m_DUIList.erase(iter);
            }
        }
        return _DestroyWindow_.Org(hWnd);
    }

    HDC MyBeginPaint(HWND hWnd, LPPAINTSTRUCT lpPaint)
    {
        //��ʼ����DUI����
        HDC hDC = _BeginPaint_.Org(hWnd, lpPaint);

        auto iter = m_DUIList.find(GetCurrentThreadId());

        if (iter != m_DUIList.end()) {
            if (iter->second.hWnd == hWnd)
            {
                iter->second.srcDC = hDC;
                iter->second.hDC = hDC;
                iter->second.treeDraw = false;
            }
            else if (iter->second.TreeWnd == hWnd)
            {
                iter->second.srcDC = hDC;
                iter->second.hDC = hDC;
                iter->second.treeDraw = true;
            }
        }
        return hDC;
    }

    BOOL MyEndPaint(HWND hWnd, const PAINTSTRUCT* lpPaint)
    {
        DWORD curThread = GetCurrentThreadId();
        auto iter = m_DUIList.find(curThread);

        if (iter != m_DUIList.end()) {
            if (iter->second.hWnd == hWnd
                || iter->second.TreeWnd == hWnd)
            {
                iter->second.srcDC = 0;
                iter->second.hDC = 0;
                iter->second.treeDraw = false;
            }
        }

        auto ribiter = m_ribbonPaint.find(curThread);
        if (ribiter != m_ribbonPaint.end())
            m_ribbonPaint.erase(ribiter);

        BOOL ret = _EndPaint_.Org(hWnd, lpPaint);
        return ret;
    }

    int MyFillRect(HDC hDC, const RECT* lprc, HBRUSH hbr)
    {
        int ret = S_OK;

        DWORD curThread = GetCurrentThreadId();

        auto iter = m_DUIList.find(curThread);
        if (iter != m_DUIList.end()) {
            if (iter->second.hDC == hDC)
            {
                ret = _FillRect_.Org(hDC, lprc, hbr);

                //ˢ����ɫֵ
                if (iter->second.refresh)
                {
                    SendMessageW(iter->second.hWnd, WM_THEMECHANGED, 0, 0);
                    iter->second.refresh = false;
                }
                //�ж����б���ɫ�Ƿ񱻸ı�
                if (iter->second.treeDraw)
                {
                    COLORREF color = RGB(0, 0, 0);
                    if (!CompareColor(TreeView_GetBkColor(iter->second.TreeWnd), color))
                        TreeView_SetBkColor(iter->second.TreeWnd, color);
                }
                return ret;
            }
            else
                goto Next;
        }
    Next:
        //͸���� Windows 10 Ribbon ��Lightģʽ
        auto ribiter = m_ribbonPaint.find(curThread);
        if (ribiter != m_ribbonPaint.end())
        {
            if (ribiter->second.second == hDC) {
                RECT rcWnd;
                GetWindowRect(ribiter->second.first, &rcWnd);

                if (lprc->bottom == CalcRibbonHeightForDPI(ribiter->second.first, 26, false)
                    || (lprc->bottom - lprc->top == CalcRibbonHeightForDPI(ribiter->second.first, 1)
                        && (rcWnd.right - rcWnd.left) == lprc->right))
                {
                    hbr = (HBRUSH)GetStockObject(BLACK_BRUSH);
                }
            }
        }

        return  _FillRect_.Org(hDC, lprc, hbr);
    }

    BOOL MyPatBlt(HDC hdc, int x, int y, int w, int h, DWORD rop)
    {
        //�޸�ѡ����ο��Alpha
        if (IsDUIThread() && rop == PATCOPY) {
            //�������߳�
            static std::unordered_map<DWORD, bool> thList;

            DWORD curThread = GetCurrentThreadId();
            if (thList.find(curThread) != thList.end())
                return _PatBlt_.Org(hdc, x, y, w, h, rop);

            thList.insert(std::make_pair(curThread, true));
            //��ȡDC��ɫ��ˢ
            HBRUSH hbr = (HBRUSH)SelectObject(hdc, GetSysColorBrush(COLOR_WINDOW));
            LOGBRUSH lbr;
            GetObjectW(hbr, sizeof(lbr), &lbr);

            //����GDIP��ˢ��ɫ
            Gdiplus::SolidBrush brush(Gdiplus::Color::Transparent);
            brush.SetColor(Gdiplus::Color::MakeARGB(200, GetRValue(lbr.lbColor),
                GetGValue(lbr.lbColor), GetBValue(lbr.lbColor)));

            //ʹ��GDIP����������
            Gdiplus::Graphics gp(hdc);
            gp.FillRectangle(&brush, Gdiplus::Rect(x, y, w, h));

            //���û�ԭ��ˢ
            SelectObject(hdc, hbr);

            thList.erase(curThread);
            return TRUE;
        }
        return _PatBlt_.Org(hdc, x, y, w, h, rop);
    }

    bool AlphaBuffer(HDC hdc, LPRECT pRc, std::function<void(HDC)> fun)
    {
        BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

        BP_PAINTPARAMS bpParam;
        bpParam.cbSize = sizeof(BP_PAINTPARAMS);
        bpParam.dwFlags = BPPF_ERASE;
        bpParam.prcExclude = nullptr;
        bpParam.pBlendFunction = &bf;
        HDC hDC = 0;
        HPAINTBUFFER pbuffer = BeginBufferedPaint(hdc, pRc, BPBF_TOPDOWNDIB, &bpParam, &hDC);
        if (pbuffer && hDC && fun)
        {
            //����ԭDC��Ϣ
            SelectObject(hDC, GetCurrentObject(hdc, OBJ_FONT));
            SetBkMode(hDC, GetBkMode(hdc));
            SetBkColor(hDC, GetBkColor(hdc));
            SetTextAlign(hDC, GetTextAlign(hdc));
            SetTextCharacterExtra(hDC, GetTextCharacterExtra(hdc));

            fun(hDC);

            EndBufferedPaint(pbuffer, TRUE);

            return true;
        }
        return false;
    }

    //�޸�DrawTextW��Alpha
    int MyDrawTextW(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format)
    {
        if ((format & DT_CALCRECT) || m_drawtextState.find(GetCurrentThreadId()) != m_drawtextState.end())
            return _DrawTextW_.Org(hdc, lpchText, cchText, lprc, format);

        HRESULT hr = S_OK;
        HDC hDC = 0;
        DTTOPTS dtop = { 0 };
        dtop.dwSize = sizeof(DTTOPTS);
        dtop.dwFlags = DTT_COMPOSITED | DTT_TEXTCOLOR | DTT_CALLBACK;
        dtop.crText = GetTextColor(hdc);
        dtop.pfnDrawTextCallback = [](HDC hdc, LPWSTR lpchText, int cchText, LPRECT lprc, UINT format, LPARAM lParam)
        {
            return _DrawTextW_.Org(hdc, lpchText, cchText, lprc, format);
        };

        auto fun = [&](HDC hDC) {
            HTHEME hTheme = OpenThemeData((HWND)0, L"Menu");
            hr = MyDrawThemeTextEx(hTheme, hDC, 0, 0, lpchText, cchText, format, lprc, &dtop);
            CloseThemeData(hTheme);
        };
        if (!AlphaBuffer(hdc, lprc, fun))
        {
            hr = _DrawTextW_.Org(hdc, lpchText, cchText, lprc, format);
        }

        return hr;
    }

    //�޸�DrawTextExW��Alpha
    int MyDrawTextExW(HDC hdc, LPWSTR lpchText, int cchText, LPRECT lprc, UINT format, LPDRAWTEXTPARAMS lpdtp)
    {
        static std::unordered_map<DWORD, bool> thList;
        DWORD curTh = GetCurrentThreadId();

        //TreeView���Ƶ�ʱ�����һ��������NULL ��˿���ֱ����DrawText
        if (!lpdtp && !(format & DT_CALCRECT) && thList.find(curTh) == thList.end()
            && m_drawtextState.find(GetCurrentThreadId()) == m_drawtextState.end()) {

            thList.insert(std::make_pair(curTh, true));
            auto ret = MyDrawTextW(hdc, lpchText, cchText, lprc, format);
            thList.erase(curTh);
            return ret;
        }
        return _DrawTextExW_.Org(hdc, lpchText, cchText, lprc, format, lpdtp);
    }

    //�޸�ExtTextOutW��Alpha
    BOOL MyExtTextOutW(HDC hdc, int x, int y, UINT option, const RECT* lprect, LPCWSTR lpString, UINT c, const INT* lpDx)
    {
        std::wstring str;
        if (lpString) str = lpString;

        static std::unordered_map<DWORD, bool> thList;
        DWORD curTh = GetCurrentThreadId();

        if (IsDUIThread() && !(option & ETO_GLYPH_INDEX) && !(option & ETO_IGNORELANGUAGE)
            && thList.find(curTh) == thList.end() && str != L"" && m_drawtextState.find(curTh) == m_drawtextState.end())
        {
            thList.insert(std::make_pair(curTh, true));

            RECT rect = { 0 };
            if ((option & ETO_OPAQUE || option & ETO_CLIPPED) && lprect)
                rect = *lprect;
            else
                _DrawTextW_.Org(hdc, lpString, c, &rect, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_CALCRECT);

            if (!lpDx) {
                MyDrawTextW(hdc, lpString, c, &rect, DT_LEFT | DT_TOP | DT_SINGLELINE);
            }
            else
            {
                RECT rc = { x, y, x + (rect.right - rect.left), y + (rect.bottom - rect.top) };

                if (option & ETO_CLIPPED)
                {
                    SaveDC(hdc);
                    IntersectClipRect(hdc, rect.left, rect.top, rect.right, rect.bottom);
                }

                HRESULT hr = S_OK;
                HDC hDC = 0;
                DTTOPTS dtop = { 0 };
                dtop.dwSize = sizeof(DTTOPTS);
                dtop.dwFlags = DTT_COMPOSITED | DTT_TEXTCOLOR | DTT_CALLBACK;
                dtop.crText = GetTextColor(hdc);
                dtop.pfnDrawTextCallback = [](HDC hdc, LPWSTR lpchText, int cchText, LPRECT lprc, UINT format, LPARAM lParam)
                {
                    return _DrawTextW_.Org(hdc, lpchText, cchText, lprc, format);
                };

                //���������ı�
                auto fun = [&](HDC hDC) {
                    HTHEME hTheme = OpenThemeData((HWND)0, L"Menu");

                    std::wstring batchStr;
                    bool batch = true;

                    batchStr += lpString[0];

                    int srcExtra = GetTextCharacterExtra(hdc);
                    SetTextCharacterExtra(hDC, lpDx[0]);

                    RECT batchRc = rc;
                    for (int i = 0; i < c; i++)
                    {
                        if (i != 0) {
                            if (lpDx[i] == lpDx[i - 1]) {
                                if (!batch)
                                {
                                    batch = true;
                                    SetTextCharacterExtra(hDC, lpDx[i]);
                                }
                                batchStr += lpString[i];
                            }
                            else
                            {
                                //�Ȼ�����һ��
                                hr = _DrawThemeTextEx_.Org(hTheme, hDC, 0, 0, batchStr.c_str(), batchStr.length(), DT_LEFT | DT_TOP | DT_SINGLELINE, &batchRc, &dtop);

                                batch = false;
                                batchStr = lpString[i];
                                SetTextCharacterExtra(hDC, lpDx[i]);
                                batchRc.left = rc.left;
                            }
                        }

                        //���һ��
                        if (i == c - 1)
                        {
                            hr = _DrawThemeTextEx_.Org(hTheme, hDC, 0, 0, batchStr.c_str(), batchStr.length(), DT_LEFT | DT_TOP | DT_SINGLELINE, &batchRc, &dtop);
                        }

                        rc.left += lpDx[i];
                    }
                    SetTextCharacterExtra(hDC, srcExtra);
                    CloseThemeData(hTheme);
                };
                if (!AlphaBuffer(hdc, &rc, fun))
                {
                    hr = _ExtTextOutW_.Org(hdc, x, y, option, lprect, lpString, c, lpDx);
                }

                if (option & ETO_CLIPPED)
                    RestoreDC(hdc, -1);
            }

            thList.erase(curTh);
            return 1;
        }
        return _ExtTextOutW_.Org(hdc, x, y, option, lprect, lpString, c, lpDx);
    }

    HDC MyCreateCompatibleDC(HDC hDC)
    {
        //�ڻ���DUI֮ǰ �����CreateCompatibleDC �ҵ���
        HDC retDC = _CreateCompatibleDC_.Org(hDC);
        auto iter = m_DUIList.find(GetCurrentThreadId());
        if (iter != m_DUIList.end()) {
            if (iter->second.hDC == hDC) {
                iter->second.hDC = retDC;
                return retDC;
            }
        }
        //Win10 Ribbon
        if (g_sysBuildNumber < 22000) {
            HWND wnd = WindowFromDC(hDC);
            if (wnd && GetWindowClassName(wnd) == L"NetUIHWND")
                m_ribbonPaint[GetCurrentThreadId()] = std::make_pair(wnd, retDC);
        }
        return retDC;
    }

    HRESULT MyGetThemeColor(
        HTHEME   hTheme,
        int      iPartId,
        int      iStateId,
        int      iPropId,
        COLORREF* pColor
    )
    {
        HRESULT ret = _GetThemeColor_.Org(hTheme, iPartId, iStateId, iPropId, pColor);
        std::wstring kname = GetThemeClassName(hTheme);

        //����Ҫ�ؼ��ı���ɫ����Ϊ��ɫ �Թ�API͸����BlurЧ��
        if (iPropId == TMT_FILLCOLOR && IsDUIThread())
        {
            //DUI��ͼ���ײ�״̬����������
            if (((kname == L"ItemsView" || kname == L"ExplorerStatusBar" || kname == L"ExplorerNavPane")
                && (iPartId == 0 && iStateId == 0))
                //ReadingPane��Ԥ�����ı���ɫ
                || (kname == L"ReadingPane" && iPartId == 1 && iStateId == 0))//FILL Color
            {
                *pColor = RGB(0, 0, 0);
            }
        }
        return ret;
    }

    //ת��ΪDrawThemeTextEx����
    HRESULT MyDrawThemeText(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCTSTR pszText,
        int cchText, DWORD dwTextFlags, DWORD dwTextFlags2, LPCRECT pRect)
    {
        HRESULT ret = S_OK;

        DTTOPTS Options = { sizeof(DTTOPTS) };
        RECT Rect = *pRect;
        ret = MyDrawThemeTextEx(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, &Rect, &Options);

        return ret;
    }

    //��ֹDrawText��API�ڲ��ݹ���� ������Alpha�޸�
    HRESULT MyDrawThemeTextEx(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCTSTR pszText,
        int cchText, DWORD dwTextFlags, LPCRECT pRect, const DTTOPTS* pOptions)
    {
        HRESULT ret = S_OK;

        if (pOptions && !(pOptions->dwFlags & DTT_CALCRECT) && !(pOptions->dwFlags & DTT_COMPOSITED))
        {
            DTTOPTS Options = *pOptions;
            Options.dwFlags |= DTT_COMPOSITED;

            HRESULT ret = S_OK;

            auto fun = [&](HDC hDC)
            {
                auto tid = GetCurrentThreadId();
                m_drawtextState.insert(std::make_pair(tid, true));
                ret = _DrawThemeTextEx_.Org(hTheme, hDC, iPartId, iStateId, pszText, cchText, dwTextFlags,
                    (LPRECT)pRect, &Options);
                m_drawtextState.erase(tid);
            };

            if (!AlphaBuffer(hdc, (LPRECT)pRect, fun))
                goto Org;

            return ret;
        }
        else {
        Org:
            ret = _DrawThemeTextEx_.Org(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, (LPRECT)pRect, pOptions);
        }
        return ret;
    }

    HRESULT MyDrawThemeBackgroundEx(
        HTHEME         hTheme,
        HDC            hdc,
        int            iPartId,
        int            iStateId,
        LPCRECT        pRect,
        const DTBGOPTS* pOptions
    )
    {
        std::wstring kname = GetThemeClassName(hTheme);

        //Blurģʽ�� ͸���� Header��Rebar��Ԥ����塢����ģ��
        if (kname == L"Header") {
            if ((iPartId == HP_HEADERITEM && iStateId == HIS_NORMAL)
                || (iPartId == HP_HEADERITEM && iStateId == HIS_SORTEDNORMAL)
                || (iPartId == 0 && iStateId == HIS_NORMAL))
                return S_OK;
        }
        else if (kname == L"Rebar")
        {
            if ((iPartId == RP_BACKGROUND || iPartId == RP_BAND) && iStateId == 0)
            {
                return S_OK;
            }
        }
        else if (kname == L"CommandModule" && IsDUIThread())
        {
            if (iPartId == 1 && iStateId == 0) {

                FillRect(hdc, pRect, m_clearBrush);
                return S_OK;
            }
        }
        else if (kname == L"PreviewPane")
        {
            if (iPartId == 1 && iStateId == 1)
                return S_OK;
        }

        return _DrawThemeBackgroundEx_.Org(hTheme, hdc, iPartId, iStateId, pRect, pOptions);
    }
}