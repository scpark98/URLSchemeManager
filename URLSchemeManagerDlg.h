
// URLSchemeManagerDlg.h: 헤더 파일
//

#pragma once

#include "Common/CComboBox/SCComboBox/SCComboBox.h"
#include "Common/CEdit/CSCStaticEdit/SCStaticEdit.h"

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
	void			load_browser_list();
	void			update_button_state();

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
	CSCStaticEdit m_static_urlscheme;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonRegister();
	afx_msg void OnBnClickedButtonDeleteRegistry();
	CSCComboBox m_combo_browser;
	afx_msg void OnCbnSelchangeComboBrowser();
	afx_msg void OnBnClickedButtonRemoveCheck();
	afx_msg void OnBnClickedButtonConfirm();
	CRichEditCtrl m_rich;
};
