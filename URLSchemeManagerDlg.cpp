
// URLSchemeManagerDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "URLSchemeManager.h"
#include "URLSchemeManagerDlg.h"
#include "afxdialogex.h"
#include "Common/Functions.h"

#include <vector>
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CURLSchemeManagerDlg 대화 상자



CURLSchemeManagerDlg::CURLSchemeManagerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_URLSCHEMEMANAGER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CURLSchemeManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_URLSCHEME, m_static_urlscheme);
	DDX_Control(pDX, IDC_COMBO_BROWSER, m_combo_browser);
	DDX_Control(pDX, IDC_RICH, m_rich);
}

BEGIN_MESSAGE_MAP(CURLSchemeManagerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CURLSchemeManagerDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CURLSchemeManagerDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_REGISTER, &CURLSchemeManagerDlg::OnBnClickedButtonRegister)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_REGISTRY, &CURLSchemeManagerDlg::OnBnClickedButtonDeleteRegistry)
	ON_CBN_SELCHANGE(IDC_COMBO_BROWSER, &CURLSchemeManagerDlg::OnCbnSelchangeComboBrowser)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_CHECK, &CURLSchemeManagerDlg::OnBnClickedButtonRemoveCheck)
	ON_BN_CLICKED(IDC_BUTTON_CONFIRM, &CURLSchemeManagerDlg::OnBnClickedButtonConfirm)
END_MESSAGE_MAP()


// CURLSchemeManagerDlg 메시지 처리기

BOOL CURLSchemeManagerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	load_browser_list();

	m_combo_browser.set_line_height(16);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CURLSchemeManagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CURLSchemeManagerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CURLSchemeManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CURLSchemeManagerDlg::OnBnClickedOk()
{
	//Enter 키가 IDOK 로 라우팅되어도 다이얼로그가 종료되지 않도록 base 호출 무력화.
	//edit 입력 후 Enter 시 버튼 활성 상태만 갱신.
	update_button_state();

	//CDialogEx::OnOK();
}

void CURLSchemeManagerDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}

void CURLSchemeManagerDlg::OnBnClickedButtonRegister()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CURLSchemeManagerDlg::OnBnClickedButtonDeleteRegistry()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CURLSchemeManagerDlg::OnCbnSelchangeComboBrowser()
{
	update_button_state();
}

void CURLSchemeManagerDlg::OnBnClickedButtonRemoveCheck()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

//현재 urlscheme 입력값과 선택 브라우저 프로필 상태에 따라 버튼 활성을 갱신.
//IDC_BUTTON_REGISTER       — urlscheme 비어있지 않고 레지스트리 미등록 시 활성.
//IDC_BUTTON_DELETE_REGISTRY — 레지스트리 3곳 중 하나라도 등록되어 있으면 활성.
//IDC_BUTTON_REMOVE_CHECK    — 브라우저 프로필에 pairs 존재 시 활성. (검사 명세 확정 후 구현)
void CURLSchemeManagerDlg::update_button_state()
{
	CString urlscheme = m_static_urlscheme.get_text();
	urlscheme.Trim();

	bool reg_exists = false;
	if (!urlscheme.IsEmpty())
	{
		reg_exists =
			is_exist_registry_key(HKEY_CLASSES_ROOT,  urlscheme) ||
			is_exist_registry_key(HKEY_CURRENT_USER,  _T("Software\\Classes\\") + urlscheme) ||
			is_exist_registry_key(HKEY_LOCAL_MACHINE, _T("Software\\Classes\\") + urlscheme);
	}

	//TODO: 선택된 브라우저(m_combo_browser.GetCurSel()) 프로필에서 urlscheme pair 검사.
	bool profile_has_pairs = false;

	GetDlgItem(IDC_BUTTON_REGISTER)->EnableWindow(!urlscheme.IsEmpty() && !reg_exists);
	GetDlgItem(IDC_BUTTON_DELETE_REGISTRY)->EnableWindow(reg_exists);
	GetDlgItem(IDC_BUTTON_REMOVE_CHECK)->EnableWindow(profile_has_pairs);
}

//시스템에 설치된 브라우저 목록을 구하고 기본 브라우저를 0번에 넣어준다.
void CURLSchemeManagerDlg::load_browser_list()
{
	m_combo_browser.ResetContent();

	std::vector<CString> browsers;
	HKEY roots[2] = { HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER };

	for (int r = 0; r < 2; r++)
	{
		HKEY hkey = NULL;
		if (RegOpenKeyEx(roots[r], _T("SOFTWARE\\Clients\\StartMenuInternet"),
			0, KEY_READ, &hkey) != ERROR_SUCCESS)
			continue;

		TCHAR subkey_name[256];
		DWORD index = 0;
		DWORD name_size = _countof(subkey_name);

		while (RegEnumKeyEx(hkey, index++, subkey_name, &name_size,
			NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
		{
			CString section;
			section.Format(_T("SOFTWARE\\Clients\\StartMenuInternet\\%s"), subkey_name);

			CString display_name;
			get_registry_str(roots[r], section, _T(""), &display_name);
			if (display_name.IsEmpty())
				display_name = subkey_name;

			bool exists = false;
			for (const auto& b : browsers)
			{
				if (b.CompareNoCase(display_name) == 0)
				{
					exists = true;
					break;
				}
			}
			if (!exists)
				browsers.push_back(display_name);

			name_size = _countof(subkey_name);
		}
		RegCloseKey(hkey);
	}

	CString default_browser = get_default_browser_info();

	int found = -1;
	for (size_t i = 0; i < browsers.size(); i++)
	{
		if (browsers[i].CompareNoCase(default_browser) == 0)
		{
			found = (int)i;
			break;
		}
	}

	if (found > 0)
		std::swap(browsers[0], browsers[found]);
	else if (found < 0 && !default_browser.IsEmpty())
		browsers.insert(browsers.begin(), default_browser);

	for (const auto& b : browsers)
		m_combo_browser.AddString(b);

	if (!browsers.empty())
		m_combo_browser.SetCurSel(0);
}

void CURLSchemeManagerDlg::OnBnClickedButtonConfirm()
{
	OnBnClickedOk();
}
