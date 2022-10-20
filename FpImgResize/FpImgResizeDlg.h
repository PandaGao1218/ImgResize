
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
	afx_msg void OnBnClickedStart();
	void ReadImage(CString filename, BYTE* image);
	void ImageView(unsigned char* image, int width, int height, int viewNum);
	int OnDrawImage(CDC* pDC, CRect rc, BYTE* image);
	BOOL m_fSave;
	BOOL m_fMulti;
	CString SelectFolderDialog();
	int ImageResize(BYTE* srcImage, int sw, int sh, BYTE* dstImage, int dw, int dh);
	int SaveImage(CString fn, BYTE* imageBuf, int w, int h);
	BOOL m_fShow;
	int MultiResize(CString path);
};
