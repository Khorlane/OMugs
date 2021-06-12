/***********************************************************
* H o l y Q u e s t  -  an attempt at something different  *
* File:   HolyQuestDlg.h                                   *
* Usage:  HolyQuest startup dialog                         *
* Author: Steve Bryant (khorlane@www.holyquest.org)        *
*                                                          *
* This program belongs to Stephen L. Bryant.               *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#if !defined(AFX_HOLYQUESTDLG_H__A79E5225_25FE_11D6_8E4B_00B0D052AAF2__INCLUDED_)
#define AFX_HOLYQUESTDLG_H__A79E5225_25FE_11D6_8E4B_00B0D052AAF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CHolyQuestDlg dialog

class CHolyQuestDlg : public CDialog
{
// Construction
public:
	CHolyQuestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CHolyQuestDlg)
	enum { IDD = IDD_HOLYQUEST_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHolyQuestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CHolyQuestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnOk();
	afx_msg void OnStart();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOLYQUESTDLG_H__A79E5225_25FE_11D6_8E4B_00B0D052AAF2__INCLUDED_)