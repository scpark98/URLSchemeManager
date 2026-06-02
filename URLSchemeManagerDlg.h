
// URLSchemeManagerDlg.h: 헤더 파일
//

#pragma once

#include "Common/ResizeCtrl.h"
#include "Common/CComboBox/SCComboBox/SCComboBox.h"
#include "Common/CEdit/RichEditCtrlEx/RichEditCtrlEx.h"
#include "Common/CEdit/CSCStaticEdit/SCStaticEdit.h"
#include "Common/CStatic/SCStatic/SCStatic.h"

#include <deque>

// CURLSchemeManagerDlg 대화 상자
class CURLSchemeManagerDlg : public CDialogEx
{
// 생성입니다.
public:
	CURLSchemeManagerDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_URLSCHEMEMANAGER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

protected:
	CToolTipCtrl	m_tooltip;
	void			init_tooltip();

	CResizeCtrl		m_resize;

	void			load_browser_list();
	void			update_button_state();

	//선택 브라우저(Chromium 계열)의 프로필 Preferences 파일 경로들(Default + Profile *)을 수집.
	void			get_browser_preference_files(const CString& browser, std::deque<CString>& files);
	//선택 브라우저 프로필에 urlscheme 의 "항상 허용"(allowed_origin_protocol_pairs[origin][scheme]=true) 항목 개수.
	int				browser_profile_scheme_count(const CString& browser, const CString& urlscheme);

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonRegister();
	afx_msg void OnBnClickedButtonDeleteRegistry();
	CSCComboBox m_combo_browser;
	afx_msg void OnCbnSelchangeComboBrowser();
	afx_msg void OnBnClickedButtonRemoveCheck();
	afx_msg void OnBnClickedButtonConfirm();
	CRichEditCtrlEx m_rich;
	CSCComboBox m_combo_url_scheme;
	afx_msg void OnCbnSelchangeComboUrlScheme();
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	CSCStatic m_static_remove_accept;
	CSCStaticEdit m_static_launcher_path;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
