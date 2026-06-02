
// URLSchemeManagerDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "URLSchemeManager.h"
#include "URLSchemeManagerDlg.h"
#include "afxdialogex.h"
#include "Common/Functions.h"
#include "Common/colors.h"
#include "Common/Json/NlohmannJson/json.hpp"

#include <deque>
#include <fstream>
#include <shlobj.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//Preferences DOM 재귀 처리 헬퍼 (정의는 load_browser_list 아래). 제거 핸들러가 정의보다 앞에 있어 전방 선언.
static int count_scheme_in_pairs (const nlohmann::json& node, const char* scheme);
static int remove_scheme_from_pairs(nlohmann::json& node, const char* scheme);


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
	DDX_Control(pDX, IDC_COMBO_BROWSER, m_combo_browser);
	DDX_Control(pDX, IDC_RICH, m_rich);
	DDX_Control(pDX, IDC_COMBO_URL_SCHEME, m_combo_url_scheme);
	DDX_Control(pDX, IDC_STATIC_REMOVE_ACCEPT, m_static_remove_accept);
	DDX_Control(pDX, IDC_STATIC_LAUNCHER_PATH, m_static_launcher_path);
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
	ON_CBN_SELCHANGE(IDC_COMBO_URL_SCHEME, &CURLSchemeManagerDlg::OnCbnSelchangeComboUrlScheme)
	ON_WM_WINDOWPOSCHANGED()
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
	init_tooltip();

	m_resize.Create(this);
	m_resize.Add(IDC_COMBO_URL_SCHEME, 0, 0, 100, 0);
	m_resize.Add(IDC_STATIC_LAUNCHER_PATH, 0, 0, 100, 0);
	m_resize.Add(IDC_RICH, 0, 0, 100, 100);

	m_combo_url_scheme.set_line_height(14);
	m_combo_url_scheme.load_history(&theApp, _T("setting\\url schemes"));

	m_combo_url_scheme.add(_T("manuallauncher.pcanypro.service"));
	m_combo_url_scheme.add(_T("manuallauncher.lmmse.service"));
	m_combo_url_scheme.add(_T("manuallauncher.lmm.service"));
	m_combo_url_scheme.add(_T("manuallauncher.helpu.service.host"));
	m_combo_url_scheme.add(_T("manuallauncher.helpu.service.host.user"));
	m_combo_url_scheme.add(_T("manuallauncher.helpu.service.supporter"));
	m_combo_url_scheme.add(_T("manuallauncher.anysupport.service.host"));
	m_combo_url_scheme.add(_T("manuallauncher.anysupport.service.supporter"));

	CString recent_url_scheme = theApp.GetProfileString(_T("setting"), _T("recent url scheme"), _T(""));
	if (recent_url_scheme.IsEmpty() == false)
		m_combo_url_scheme.SelectString(-1, recent_url_scheme);

	m_static_launcher_path.set_action_button(CSCStaticEdit::action_file);
	m_static_launcher_path.set_readonly();
	m_static_launcher_path.set_dim_text(_T("레지스트리에 URLScheme 관련 등록된 정보가 없습니다."));

	load_browser_list();

	m_combo_browser.set_line_height(16);
	m_static_remove_accept.set_text(_T("브라우저에서 URLScheme으로 실행을 <b><cr=blue>\"항상 허용\"</b></cr>한 경우 옵션 제거 가능 (체크하고 확인을 누른 경우)<br>")
									_T("URLScheme으로 실행하겠냐는 확인창을 다시 표시하고자 할 경우는 \"항상 허용 제거\"를 클릭."));

	update_button_state();

	RestoreWindowPosition(&theApp, this, _T(""), false, true);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CURLSchemeManagerDlg::init_tooltip()
{
	m_tooltip.Create(this, TTS_ALWAYSTIP | TTS_NOPREFIX | TTS_NOANIMATE);

	//필요한 옵션 설정
	m_tooltip.SetDelayTime(TTDT_AUTOPOP, -1);	//optional. 툴팁 표시 지속시간 설정.
	m_tooltip.SetDelayTime(TTDT_INITIAL, 500);	//optional. 툴팁을 표시하기 위해 마우스가 머물러야 할 최소 시간.
	m_tooltip.SetDelayTime(TTDT_RESHOW, 0);		//optional. 포인터가 한 도구에서 다른 도구로 이동할 때 후속 도구 설명 창이 표시되는 데 걸리는 시간.
	m_tooltip.SetMaxTipWidth(400);				//optional. 툴팁창의 최대 너비로서 여러줄의 툴팁을 표시할 경우 필수. ‘\n’ 문자로 멀티라인 표현 가능.
	m_tooltip.Activate(TRUE);

	m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_CONFIRM),	_T("URLScheme String을 입력한 후 이 버튼을 클릭하면 Launcher fullpath, 레지스트리 등록 여부,")
														_T("브라우저에서의 \"항상 허용\" 여부에 따라 각 버튼들의 상태가 변경됨"));
	m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_REGISTER),	_T("URLScheme String과 Launcher fullpath 설정 후 URLScheme 정보를 레지스트리에 등록해준다"));
	m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_DELETE_REGISTRY), _T("레지스트리에 등록된 URLScheme 정보를 삭제한다"));
	m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_REMOVE_CHECK), _T("URLScheme 정보가 시스템에 등록되어 있으면 웹브라우저에서 항상 이 프로그램으로 열겠냐는 대화상자가 표시되는데")
														_T("한번 체크한 후에는 다시 이 대화상자는 표시되지 않는다. \"항상 허용 제거\" 버튼을 클릭하여 대화상자를 다시 표시할 수 있다"));
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
	CString urlscheme = m_combo_url_scheme.get_cur_sel_text();
	urlscheme.Trim();
	if (urlscheme.IsEmpty())
		return;

	CString app_path = m_static_launcher_path.get_text();
	app_path.Trim();
	if (app_path.IsEmpty())
	{
		m_rich.addl(CR_WARNING, _T("[register] 실행 파일 경로가 비어 있습니다. 우측 파일 버튼으로 launcher 를 지정하세요."));
		return;
	}

	//URL 프로토콜 표준 구조:
	//  <scheme>                    (default) = "URL:<scheme> Protocol"
	//  <scheme>                    "URL Protocol" = ""   ← URL 핸들러임을 표시
	//  <scheme>\shell\open\command (default) = "<app>" "%1"
	CString default_value;
	default_value.Format(_T("URL:%s Protocol"), urlscheme);

	CString command;
	command.Format(_T("\"%s\" \"%%1\""), app_path);

	struct { HKEY root; LPCTSTR root_name; CString sub; } targets[3] =
	{
		{ HKEY_CLASSES_ROOT,  _T("HKCR"), urlscheme },
		{ HKEY_CURRENT_USER,  _T("HKCU"), _T("Software\\Classes\\") + urlscheme },
		{ HKEY_LOCAL_MACHINE, _T("HKLM"), _T("Software\\Classes\\") + urlscheme },
	};

	m_rich.addl(CR_INFO, _T("[register] '%s' → %s"), urlscheme, app_path);

	for (int i = 0; i < 3; i++)
	{
		LONG r1 = set_registry_str(targets[i].root, targets[i].sub, _T(""), default_value);
		LONG r2 = set_registry_str(targets[i].root, targets[i].sub, _T("URL Protocol"), _T(""));
		LONG r3 = set_registry_str(targets[i].root, targets[i].sub + _T("\\shell\\open\\command"), _T(""), command);

		if (r1 == ERROR_SUCCESS && r2 == ERROR_SUCCESS && r3 == ERROR_SUCCESS)
			m_rich.addl(CR_SUCCESS, _T("  %s\\%s — 등록 성공"), targets[i].root_name, targets[i].sub);
		else
			m_rich.addl(CR_ERROR,   _T("  %s\\%s — 등록 실패 (%ld/%ld/%ld, 관리자 권한 필요?)"), targets[i].root_name, targets[i].sub, r1, r2, r3);
	}

	update_button_state();
}

void CURLSchemeManagerDlg::OnBnClickedButtonDeleteRegistry()
{
	CString urlscheme = m_combo_url_scheme.get_cur_sel_text();
	urlscheme.Trim();
	if (urlscheme.IsEmpty())
		return;

	struct { HKEY root; LPCTSTR root_name; CString sub; } targets[3] =
	{
		{ HKEY_CLASSES_ROOT,  _T("HKCR"), urlscheme },
		{ HKEY_CURRENT_USER,  _T("HKCU"), _T("Software\\Classes\\") + urlscheme },
		{ HKEY_LOCAL_MACHINE, _T("HKLM"), _T("Software\\Classes\\") + urlscheme },
	};

	m_rich.addl(CR_INFO, _T("[delete] '%s'"), urlscheme);

	for (int i = 0; i < 3; i++)
	{
		LONG r = RegDeleteTree(targets[i].root, targets[i].sub);
		if (r == ERROR_SUCCESS)
			m_rich.addl(CR_SUCCESS, _T("  %s\\%s — 삭제 성공"), targets[i].root_name, targets[i].sub);
		else if (r == ERROR_FILE_NOT_FOUND)
			m_rich.addl(CR_INFO,    _T("  %s\\%s — 존재하지 않음"), targets[i].root_name, targets[i].sub);
		else
			m_rich.addl(CR_ERROR,   _T("  %s\\%s — 삭제 실패 (LONG=%ld, 관리자 권한 필요?)"), targets[i].root_name, targets[i].sub, r);
	}

	update_button_state();
}

void CURLSchemeManagerDlg::OnCbnSelchangeComboBrowser()
{
	update_button_state();
}

void CURLSchemeManagerDlg::OnBnClickedButtonRemoveCheck()
{
	CString urlscheme = m_combo_url_scheme.get_cur_sel_text();
	urlscheme.Trim();
	if (urlscheme.IsEmpty())
		return;

	CString browser = m_combo_browser.get_cur_sel_text();
	if (browser.IsEmpty())
		return;

	//브라우저가 실행 중이면 우리가 Preferences 를 고쳐도 종료 시 메모리 상태로 복원되므로
	//제거를 시도하지 않고 "실행 중이라 불가" 만 안내한다.
	CString exe;
	if (browser.Find(_T("Edge")) >= 0)
		exe = _T("msedge.exe");
	else if (browser.Find(_T("Whale")) >= 0)
		exe = _T("whale.exe");
	else if (browser.Find(_T("Chrome")) >= 0)
		exe = _T("chrome.exe");

	if (!exe.IsEmpty() && is_running(exe))
	{
		m_rich.addl(CR_ERROR, _T("[remove] %s이(가) 실행중이므로 제거할 수 없습니다. 완전히 종료 후 다시 시도하세요.")
							_T("만약 실행중인 프로세스가 없는데도 이 메시지가 표시된다면 작업관리자에서 백그라운드로 실행중인 프로세스를 직접 종료시켜야 합니다."), browser);
		if (exe == _T("msedge.exe"))
			m_rich.addl(CR_ERROR, _T("특히 Microsoft Edge의 경우는 작업 관리자 → 프로세스 → 백그라운드 프로세스 → Microsoft Edge 항목을 우클릭하여 \"작업 끝내기\"를 실행합니다."));
		return;
	}

	std::deque<CString> files;
	get_browser_preference_files(browser, files);
	if (files.empty())
	{
		m_rich.addl(CR_ERROR, _T("[remove] '%s' 프로필 Preferences 를 찾지 못했습니다."), browser);
		return;
	}

	CStringA scheme_utf8(urlscheme);	//URL scheme 은 ASCII

	int total_removed = 0;
	int write_fail = 0;
	for (const auto& path : files)
	{
		std::ifstream ifs(path.GetString(), std::ios::binary);
		if (!ifs.is_open())
			continue;
		std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
		ifs.close();

		if (content.find((const char*)scheme_utf8) == std::string::npos)
			continue;

		nlohmann::json j = nlohmann::json::parse(content, nullptr, false);
		if (j.is_discarded())
			continue;

		int removed = remove_scheme_from_pairs(j, (const char*)scheme_utf8);
		if (removed == 0)
			continue;

		//ensure_ascii=false 로 UTF-8 유지, error_handler=replace 로 dump 예외 방지.
		std::string out = j.dump(-1, ' ', false, nlohmann::json::error_handler_t::replace);

		std::ofstream ofs(path.GetString(), std::ios::binary | std::ios::trunc);
		if (!ofs.is_open())
		{
			write_fail++;
			continue;
		}
		ofs.write(out.data(), (std::streamsize)out.size());
		ofs.close();

		total_removed += removed;
	}

	//결과에 따라 단일 메시지만 표시.
	if (write_fail > 0)
		m_rich.addl(CR_ERROR,   _T("[remove] '%s' — 파일 쓰기 실패(%d). 권한/파일 잠김을 확인하세요."), urlscheme, write_fail);
	else if (total_removed > 0)
		m_rich.addl(CR_SUCCESS, _T("[remove] '%s' / %s — \"항상 허용\" %d개 제거 완료."), urlscheme, browser, total_removed);
	else
		m_rich.addl(CR_INFO,    _T("[remove] '%s' — 제거할 항목이 없습니다."), urlscheme);

	//실제로 사라졌는지 다시 검사해 버튼 상태 갱신.
	update_button_state();
}

//현재 urlscheme 입력값과 선택 브라우저 프로필 상태에 따라 버튼 활성을 갱신.
//IDC_BUTTON_REGISTER       — urlscheme 비어있지 않고 레지스트리 미등록 시 활성.
//IDC_BUTTON_DELETE_REGISTRY — 레지스트리 3곳 중 하나라도 등록되어 있으면 활성.
//IDC_BUTTON_REMOVE_CHECK    — 브라우저 프로필에 pairs 존재 시 활성. (검사 명세 확정 후 구현)
void CURLSchemeManagerDlg::update_button_state()
{
	CString urlscheme = m_combo_url_scheme.get_cur_sel_text();
	m_combo_url_scheme.add(urlscheme);
	m_combo_url_scheme.save_history(&theApp, _T("setting\\url schemes"));
	theApp.WriteProfileString(_T("setting"), _T("recent url scheme"), urlscheme);
	urlscheme.Trim();

	bool reg_exists = false;
	if (!urlscheme.IsEmpty())
	{
		reg_exists =
			is_exist_registry_key(HKEY_CLASSES_ROOT,  urlscheme) ||
			is_exist_registry_key(HKEY_CURRENT_USER,  _T("Software\\Classes\\") + urlscheme) ||
			is_exist_registry_key(HKEY_LOCAL_MACHINE, _T("Software\\Classes\\") + urlscheme);
	}

	//선택 브라우저 프로필에 urlscheme "항상 허용" 항목이 있으면 "항상 허용 제거" 버튼 활성.
	bool profile_has_pairs = false;
	if (!urlscheme.IsEmpty())
	{
		CString browser = m_combo_browser.get_cur_sel_text();
		if (!browser.IsEmpty())
			profile_has_pairs = (browser_profile_scheme_count(browser, urlscheme) > 0);
	}

	GetDlgItem(IDC_BUTTON_REGISTER)->EnableWindow(!urlscheme.IsEmpty() && !reg_exists);
	GetDlgItem(IDC_BUTTON_DELETE_REGISTRY)->EnableWindow(reg_exists);
	GetDlgItem(IDC_BUTTON_REMOVE_CHECK)->EnableWindow(profile_has_pairs);
}

//시스템에 설치된 브라우저 목록을 콤보에 채우고 기본 브라우저를 현재 선택으로 한다.
void CURLSchemeManagerDlg::load_browser_list()
{
	m_combo_browser.ResetContent();

	std::deque<CString> browsers;
	int default_index = get_browser_list(browsers);

	for (const auto& b : browsers)
		m_combo_browser.AddString(b);

	if (default_index >= 0)
		m_combo_browser.SetCurSel(default_index);
}

//Preferences DOM 어디에 있든 "allowed_origin_protocol_pairs" 객체를 재귀로 찾아
//그 안에서 scheme=true 인 (origin) 항목 수를 센다. Chrome 은 이 키를 protection/extension 등
//여러 부모 아래 둘 수 있어 top-level 경로를 가정하지 않는다.
static int count_scheme_in_pairs(const nlohmann::json& node, const char* scheme)
{
	int count = 0;

	if (node.is_object())
	{
		auto pairs = node.find("allowed_origin_protocol_pairs");
		if (pairs != node.end() && pairs->is_object())
		{
			for (auto it = pairs->begin(); it != pairs->end(); ++it)
			{
				const nlohmann::json& schemes = it.value();
				if (!schemes.is_object())
					continue;
				auto s = schemes.find(scheme);
				if (s != schemes.end() && s->is_boolean() && s->get<bool>())
					count++;
			}
		}

		for (auto it = node.begin(); it != node.end(); ++it)
			count += count_scheme_in_pairs(it.value(), scheme);
	}
	else if (node.is_array())
	{
		for (const nlohmann::json& e : node)
			count += count_scheme_in_pairs(e, scheme);
	}

	return count;
}

//count_scheme_in_pairs 와 대칭: 모든 allowed_origin_protocol_pairs 의 각 origin 에서 scheme 키를 제거.
//반환: 제거한 (origin) 항목 수.
static int remove_scheme_from_pairs(nlohmann::json& node, const char* scheme)
{
	int removed = 0;

	if (node.is_object())
	{
		auto pairs = node.find("allowed_origin_protocol_pairs");
		if (pairs != node.end() && pairs->is_object())
		{
			for (auto it = pairs->begin(); it != pairs->end(); ++it)
			{
				nlohmann::json& schemes = it.value();
				if (schemes.is_object() && schemes.contains(scheme))
				{
					schemes.erase(scheme);
					removed++;
				}
			}
		}

		for (auto it = node.begin(); it != node.end(); ++it)
			removed += remove_scheme_from_pairs(it.value(), scheme);
	}
	else if (node.is_array())
	{
		for (nlohmann::json& e : node)
			removed += remove_scheme_from_pairs(e, scheme);
	}

	return removed;
}

void CURLSchemeManagerDlg::get_browser_preference_files(const CString& browser, std::deque<CString>& files)
{
	files.clear();

	TCHAR local[MAX_PATH] = { 0, };
	if (SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, local) != S_OK)
		return;

	CString user_data;
	if (browser.Find(_T("Edge")) >= 0)
		user_data.Format(_T("%s\\Microsoft\\Edge\\User Data"), local);
	else if (browser.Find(_T("Whale")) >= 0)
		user_data.Format(_T("%s\\Naver\\Naver Whale\\User Data"), local);
	else if (browser.Find(_T("Chrome")) >= 0)
		user_data.Format(_T("%s\\Google\\Chrome\\User Data"), local);
	else
		return;	//Chromium 계열만 지원 (Firefox/IE 는 메커니즘이 다름)

	CFileFind finder;
	BOOL more = finder.FindFile(user_data + _T("\\*"));
	while (more)
	{
		more = finder.FindNextFile();
		if (!finder.IsDirectory() || finder.IsDots())
			continue;

		CString name = finder.GetFileName();
		if (name.CompareNoCase(_T("Default")) != 0 && name.Left(8).CompareNoCase(_T("Profile ")) != 0)
			continue;

		CString pref = finder.GetFilePath() + _T("\\Preferences");
		DWORD attr = GetFileAttributes(pref);
		if (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY))
			files.push_back(pref);
	}
	finder.Close();
}

int CURLSchemeManagerDlg::browser_profile_scheme_count(const CString& browser, const CString& urlscheme)
{
	std::deque<CString> files;
	get_browser_preference_files(browser, files);
	if (files.empty())
		return 0;

	CStringA scheme_utf8(urlscheme);	//URL scheme 은 RFC 상 ASCII → ACP 변환이 UTF-8 과 동일

	int count = 0;
	for (const auto& path : files)
	{
		std::ifstream ifs(path.GetString(), std::ios::binary);
		if (!ifs.is_open())
			continue;

		std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
		ifs.close();

		//1차 필터: scheme 문자열이 파일에 아예 없으면 multi-MB JSON parse 생략.
		if (content.find((const char*)scheme_utf8) == std::string::npos)
			continue;

		nlohmann::json j = nlohmann::json::parse(content, nullptr, false);
		if (j.is_discarded())
			continue;

		count += count_scheme_in_pairs(j, (const char*)scheme_utf8);
	}

	return count;
}

//"C:\path\app.exe" "%1" 형태의 command 에서 첫 따옴표 쌍 안의 실행 경로만 추출.
//따옴표가 없으면 첫 공백 전까지.
static CString extract_app_path_from_command(CString command)
{
	command.Trim();
	if (command.IsEmpty())
		return _T("");

	if (command[0] == _T('"'))
	{
		int end = command.Find(_T('"'), 1);
		if (end > 0)
			return command.Mid(1, end - 1);
	}

	int sp = command.Find(_T(' '));
	return (sp > 0) ? command.Left(sp) : command;
}

void CURLSchemeManagerDlg::OnBnClickedButtonConfirm()
{
	update_button_state();

	CString urlscheme = m_combo_url_scheme.get_cur_sel_text();
	urlscheme.Trim();
	if (urlscheme.IsEmpty())
		return;

	//등록되어 있으면 shell\open\command 를 3곳 우선순위(HKCR → HKCU → HKLM)로 읽어 app 경로를 표시.
	struct { HKEY root; CString sub; } targets[3] =
	{
		{ HKEY_CLASSES_ROOT,  urlscheme + _T("\\shell\\open\\command") },
		{ HKEY_CURRENT_USER,  _T("Software\\Classes\\") + urlscheme + _T("\\shell\\open\\command") },
		{ HKEY_LOCAL_MACHINE, _T("Software\\Classes\\") + urlscheme + _T("\\shell\\open\\command") },
	};

	CString command;
	for (int i = 0; i < 3; i++)
	{
		get_registry_str(targets[i].root, targets[i].sub, _T(""), &command);
		if (!command.IsEmpty())
			break;
	}

	if (!command.IsEmpty())
		m_static_launcher_path.set_text(extract_app_path_from_command(command));
}

void CURLSchemeManagerDlg::OnCbnSelchangeComboUrlScheme()
{
	update_button_state();
}

void CURLSchemeManagerDlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CDialogEx::OnWindowPosChanged(lpwndpos);

	SaveWindowPosition(&theApp, this);
}

BOOL CURLSchemeManagerDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	//이 코드를 넣어줘야 disabled에서도 툴팁이 동작하는데
	//이 코드를 컨트롤 클래스에 넣어줘도 소용없다.
	//이 코드는 main에 있어야만 disable 상태일때도 잘 표시된다.
	if (m_tooltip.m_hWnd)
	{
		//msg를 따로 선언해서 사용하지 않고 *pMsg를 그대로 이용하면 이상한 현상이 발생한다.
		MSG msg = *pMsg;
		msg.hwnd = (HWND)m_tooltip.SendMessage(TTM_WINDOWFROMPOINT, 0, (LPARAM) & (msg.pt));

		CPoint pt = msg.pt;

		if (msg.message >= WM_MOUSEFIRST && msg.message <= WM_MOUSELAST)
			::ScreenToClient(msg.hwnd, &pt);

		msg.lParam = MAKELONG(pt.x, pt.y);

		// relay mouse event before deleting old tool 
		m_tooltip.SendMessage(TTM_RELAYEVENT, 0, (LPARAM)&msg);
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
