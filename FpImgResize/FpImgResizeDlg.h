
// FpImgResizeDlg.h : header file
//

#pragma once


// CFpImgResizeDlg dialog
class CFpImgResizeDlg : public CDialogEx
{
// Construction
public:
	CFpImgResizeDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FPIMGRESIZE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	int m_Height_Dst;
	int m_Width_Dst;
	int m_Height_Src;
	int m_Width_Src;
	CStatic m_ftImage_Dst;
	CStatic m_ftImage_Src;
	afx_msg void OnBnClickedResize();
	void ReadImageBmp(CString filename, BYTE* image);
	void ImageView(unsigned char* image, int width, int height, int viewNum);
	int OnDrawImage(CDC* pDC, CRect rc, BYTE* image);
	BOOL m_fSave;
	BOOL m_fMulti;
	CString SelectFolderDialog();
	int ImageResize(BYTE* srcImage, int sw, int sh, BYTE* dstImage, int dw, int dh);
	int SaveImageBmp(CString fn, BYTE* imageBuf, int w, int h);
	int SaveImageRaw(CString fn, BYTE* imageBuf, LONG64 length);
	int SavePlaneText(CString fn, CString str);
	int MultiResize(CString path, int kind);
	int MultiConvert(CString path);
	int MultiCutting(CString path);
	int MultiExpand(CString path);
	int MultiDetect(CString path);
	afx_msg void OnBnClickedAnalysis();
	CString LogPath;
	CString m_msgLeft;
	CString m_msgRight;
	afx_msg void OnBnClickedConvert();
	afx_msg void OnBnClickedCancel();
	int m_Width_Cvt;
	int m_Height_Cvt;
	CButton m_btConvert;
	CButton m_btAnalysis;
	CButton m_btResize;

	afx_msg void OnEnChangeEditSrcwidth();
	afx_msg void OnEnChangeEditSrcheight();
	afx_msg void OnEnChangeEditDstwidth();
	afx_msg void OnEnChangeEditDstheight();
	afx_msg void OnBnClickedDetect();
	CButton m_btDetect;
	CString OnReadImage();
	afx_msg void OnEnChangeEditCvtwidth();
	afx_msg void OnBnClickedTransform();
	afx_msg void OnBnClickedCutting();
	afx_msg void OnBnClickedExpand();
};
