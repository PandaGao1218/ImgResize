
// FpImgResizeDlg.cpp : implementation file
//

#include "pch.h"
#include <stdint.h>
#include "framework.h"
#include "FpImgResize.h"
#include "FpImgResizeDlg.h"
#include "afxdialogex.h"

#include "my_algo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning(disable : 4996)
//#include <setupapi.h>   // from MS Platform SDK
////}
//#pragma comment(lib, "Setupapi.lib" )
//#include "spti.h" 

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


//	global variables

BYTE m_bImageSrc[1024 * 1024];
BYTE m_bImageDst[1024 * 1024];

BYTE m_bImageTmp[1024 * 1024];

BYTE imadibSrc[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256 + 1024 * 1024];
BYTE imadibDst[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256 + 1024 * 1024];

//int  m_nWidthSrc = 256;
//int  m_nHeightSrc = 360;
//int  m_nImaSizeSrc = m_nWidthSrc * m_nHeightSrc;
//
//int  m_nWidthDst = 208;
//int  m_nHeightDst = 388;
//int  m_nImaSizeDst = m_nWidthDst * m_nHeightDst;

#define		PROC_RESIZE			10
#define		PROC_TRANSFORM		11
#define		PROC_CONVERT		12

// CFpImgResizeDlg dialog

CFpImgResizeDlg::CFpImgResizeDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FPIMGRESIZE_DIALOG, pParent)
	, m_Height_Dst(288)
	, m_Width_Dst(208)
	, m_Height_Src(360)
	, m_Width_Src(256)
	, m_fSave(FALSE)
	, m_fMulti(FALSE)
	, m_msgLeft(_T(""))
	, m_msgRight(_T(""))
	, m_Height_Cvt(160)
	, m_Width_Cvt(32)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFpImgResizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DSTHEIGHT, m_Height_Dst);
	DDX_Text(pDX, IDC_EDIT_DSTWIDTH, m_Width_Dst);
	DDX_Text(pDX, IDC_EDIT_SRCHEIGHT, m_Height_Src);
	DDX_Text(pDX, IDC_EDIT_SRCWIDTH, m_Width_Src);
	DDX_Control(pDX, IDC_IMAGE_DST, m_ftImage_Dst);
	DDX_Control(pDX, IDC_IMAGE_SRC, m_ftImage_Src);
	DDX_Check(pDX, IDC_CHECK_SAVE, m_fSave);
	DDX_Check(pDX, IDC_CHECK_MULTI, m_fMulti);
	DDX_Text(pDX, IDC_MSG_LEFT, m_msgLeft);
	DDX_Text(pDX, IDC_MSG_RIGHT, m_msgRight);
	DDX_Text(pDX, IDC_EDIT_CVTWIDTH, m_Width_Cvt);
	DDX_Text(pDX, IDC_EDIT_CVTHEIGHT, m_Height_Cvt);
	DDX_Control(pDX, IDC_CONVERT, m_btConvert);
	DDX_Control(pDX, IDC_ANALYSIS, m_btAnalysis);
	DDX_Control(pDX, IDC_RESIZE, m_btResize);
	DDX_Control(pDX, IDC_DETECT, m_btDetect);
}

BEGIN_MESSAGE_MAP(CFpImgResizeDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RESIZE, &CFpImgResizeDlg::OnBnClickedResize)
	ON_BN_CLICKED(IDC_ANALYSIS, &CFpImgResizeDlg::OnBnClickedAnalysis)
	ON_BN_CLICKED(IDC_CONVERT, &CFpImgResizeDlg::OnBnClickedConvert)
	ON_BN_CLICKED(IDCANCEL, &CFpImgResizeDlg::OnBnClickedCancel)
	ON_EN_CHANGE(IDC_EDIT_SRCWIDTH, &CFpImgResizeDlg::OnEnChangeEditSrcwidth)
	ON_EN_CHANGE(IDC_EDIT_SRCHEIGHT, &CFpImgResizeDlg::OnEnChangeEditSrcheight)
	ON_EN_CHANGE(IDC_EDIT_DSTWIDTH, &CFpImgResizeDlg::OnEnChangeEditDstwidth)
	ON_EN_CHANGE(IDC_EDIT_DSTHEIGHT, &CFpImgResizeDlg::OnEnChangeEditDstheight)
	ON_BN_CLICKED(IDC_DETECT, &CFpImgResizeDlg::OnBnClickedDetect)
	ON_EN_CHANGE(IDC_EDIT_CVTWIDTH, &CFpImgResizeDlg::OnEnChangeEditCvtwidth)
	ON_BN_CLICKED(IDC_TRANSFORM, &CFpImgResizeDlg::OnBnClickedTransform)
	ON_BN_CLICKED(IDC_CUTTING, &CFpImgResizeDlg::OnBnClickedCutting)
	ON_BN_CLICKED(IDC_EXPAND, &CFpImgResizeDlg::OnBnClickedExpand)
END_MESSAGE_MAP()


// CFpImgResizeDlg message handlers

BOOL CFpImgResizeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	BYTE* lpBits;
	BITMAPINFOHEADER* bmi;
	RGBQUAD* pRgbQuad;
	UINT i;

	bmi = (BITMAPINFOHEADER*)imadibSrc;
	bmi->biSize = sizeof(BITMAPINFOHEADER);
	bmi->biWidth = 256;
	bmi->biHeight = 360;
	bmi->biPlanes = 1;
	bmi->biBitCount = 8;
	bmi->biCompression = 0;
	bmi->biSizeImage = 0 * 0;
	bmi->biXPelsPerMeter = 0;
	bmi->biYPelsPerMeter = 0;
	bmi->biClrUsed = (DWORD)256;
	bmi->biClrImportant = (DWORD)0;
	pRgbQuad = (RGBQUAD*)((BYTE*)imadibSrc + (*(LPDWORD)imadibSrc));
	for (i = 0; i < 256; i++) {
		(pRgbQuad + i)->rgbBlue = (BYTE)i;
		(pRgbQuad + i)->rgbGreen = (BYTE)i;
		(pRgbQuad + i)->rgbRed = (BYTE)i;
		(pRgbQuad + i)->rgbReserved = PC_RESERVED;
	}
	lpBits = (BYTE*)imadibSrc + (*(LPDWORD)imadibSrc) + 256 * sizeof(RGBQUAD);

	bmi = (BITMAPINFOHEADER*)imadibDst;
	bmi->biSize = sizeof(BITMAPINFOHEADER);
	bmi->biWidth = 256;
	bmi->biHeight = 360;
	bmi->biPlanes = 1;
	bmi->biBitCount = 8;
	bmi->biCompression = 0;
	bmi->biSizeImage = 0 * 0;
	bmi->biXPelsPerMeter = 0;
	bmi->biYPelsPerMeter = 0;
	bmi->biClrUsed = (DWORD)256;
	bmi->biClrImportant = (DWORD)0;
	pRgbQuad = (RGBQUAD*)((BYTE*)imadibDst + (*(LPDWORD)imadibDst));
	for (i = 0; i < 256; i++) {
		(pRgbQuad + i)->rgbBlue = (BYTE)i;
		(pRgbQuad + i)->rgbGreen = (BYTE)i;
		(pRgbQuad + i)->rgbRed = (BYTE)i;
		(pRgbQuad + i)->rgbReserved = PC_RESERVED;
	}
	lpBits = (BYTE*)imadibDst + (*(LPDWORD)imadibDst) + 256 * sizeof(RGBQUAD);



	CFont m_editfont;
	CFont m_btnFont;

	//设置 Static Text 静态文本字体放大
	m_editfont.CreatePointFont(180, _T("黑体"), NULL);
	GetDlgItem(IDC_EDIT_SRCWIDTH)->SetFont(&m_editfont);
	GetDlgItem(IDC_EDIT_SRCHEIGHT)->SetFont(&m_editfont);
	GetDlgItem(IDC_EDIT_DSTWIDTH)->SetFont(&m_editfont);
	GetDlgItem(IDC_EDIT_DSTHEIGHT)->SetFont(&m_editfont);
	GetDlgItem(IDC_EDIT_CVTWIDTH)->SetFont(&m_editfont);
	GetDlgItem(IDC_EDIT_CVTHEIGHT)->SetFont(&m_editfont);

	m_btnFont.CreatePointFont(300, _T("黑体"), NULL);
	GetDlgItem(IDC_RESIZE)->SetFont(&m_btnFont);
	GetDlgItem(IDC_TRANSFORM)->SetFont(&m_btnFont);
	GetDlgItem(IDC_CONVERT)->SetFont(&m_btnFont);
	GetDlgItem(IDC_ANALYSIS)->SetFont(&m_btnFont);
	GetDlgItem(IDCANCEL)->SetFont(&m_btnFont);
	GetDlgItem(IDC_CHECK_SAVE)->SetFont(&m_btnFont);
	GetDlgItem(IDC_CHECK_MULTI)->SetFont(&m_btnFont);
	GetDlgItem(IDC_DETECT)->SetFont(&m_btnFont);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFpImgResizeDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFpImgResizeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFpImgResizeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

#define		DATA_SHIFT			16

void ImageTransform(BYTE* pImgIn, int width, int height, BYTE* pImgOut, int widthOut, int heightOut)
{
	int ifX, ifY, ifX1, ifY1, xmax, ymax;
	int ir1, ir2, dx, dy;
	BYTE byte, rgb1, rgb2, rgb3, rgb4, * ptrOut = pImgOut;
	int xScale, yScale, fX, fY, x, y;

	if (width == widthOut && height == heightOut) return;

	xScale = (width << DATA_SHIFT) / widthOut;
	yScale = (height << DATA_SHIFT) / heightOut;

	xmax = width - 1;
	ymax = height - 1;

	for (y = 0; y < heightOut; y++) {
		fY = y * yScale;
		ifY = (fY >> DATA_SHIFT);
		ifY1 = min(ymax, ifY + 1);
		dy = fY - (ifY << DATA_SHIFT);
		for (x = 0; x < widthOut; x++, ptrOut++) {
			fX = x * xScale;
			ifX = (fX >> DATA_SHIFT);
			ifX1 = min(xmax, ifX + 1);
			dx = fX - (ifX << DATA_SHIFT);
			// Interpolate using the four nearest pixels in the source
			rgb1 = pImgIn[ifY * width + ifX];
			rgb2 = pImgIn[ifY * width + ifX1];
			rgb3 = pImgIn[ifY1 * width + ifX];
			rgb4 = pImgIn[ifY1 * width + ifX1];
			// Interplate in x direction:
			ir1 = rgb1 + (((rgb3 - rgb1) * dy) >> DATA_SHIFT);
			ir2 = rgb2 + (((rgb4 - rgb2) * dy) >> DATA_SHIFT);
			// Interpolate in y:
			byte = (BYTE)(ir1 + (((ir2 - ir1) * dx) >> DATA_SHIFT));
			// Set output
			(*ptrOut) = byte;
		}
	}
}


void CFpImgResizeDlg::OnBnClickedResize()
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);

	if (m_fMulti) {
		CFolderPickerDialog cfd(NULL, 0, this, 0);

		if (cfd.DoModal() == IDOK)
		{
			CString strFolder = cfd.GetPathName();

			MultiResize(strFolder, PROC_RESIZE);
		}
	}
	else {
		CFileDialog dlg(TRUE, _T("File Open"), NULL, NULL, _T("Raw Files(*.raw)|*.raw|Bmp Files (*.bmp)|*.bmp|All Files|*.*||"), this);
		if (dlg.DoModal() == IDCANCEL) return;

		CString path = dlg.GetPathName();
		CString path_r;

		CString extention = PathFindExtension(path);
		if (extention == ".bmp")
		{
			ReadImageBmp(path, m_bImageSrc);

			path_r.Format(_T("%s_%d_%d.bmp"), path, m_Width_Dst, m_Height_Dst);
		}
		else if (extention == ".raw")
		{
			// Read file contents
			CFile file(path, CFile::modeRead);
			UINT cnt = (UINT)file.GetLength();
			file.Read(m_bImageSrc, cnt);
			file.Close();

			path_r.Format(_T("%s_%d_%d.raw"), path, m_Width_Dst, m_Height_Dst);
		}
		else return;

		ImageView(m_bImageSrc, m_Width_Src, m_Height_Src, 0);

		// Filtering
		ImageResize(m_bImageSrc, m_Width_Src, m_Height_Src, m_bImageDst, m_Width_Dst, m_Height_Dst);

		//ImageTransform(m_bImageSrc, m_Width_Src, m_Height_Src, m_bImageDst, m_Width_Dst, m_Height_Dst);

		ImageView(m_bImageDst, m_Width_Dst, m_Height_Dst, 2);

		if (m_fSave) {
			if (extention == ".bmp")
			{
				SaveImageBmp(path_r, imadibDst, m_Width_Dst, m_Height_Dst);
			}
			else if (extention == ".raw")
			{
				SaveImageRaw(path_r, m_bImageDst, m_Width_Dst * m_Height_Dst);
			}

		}
		//		m_msg_new = "图像处理完成.";

	}


	UpdateData(FALSE);

}


void CFpImgResizeDlg::ReadImageBmp(CString filename, BYTE* image)
{
	// TODO: Add your implementation code here.

	CFile file;
	CFileException ex;
	BITMAPFILEHEADER bmfHdr;
	BITMAPINFOHEADER bmpInfo;
	int bmStart;

	if (!file.Open(filename, CFile::modeRead, &ex)) return;

	file.SeekToBegin();
	file.Read(&bmfHdr, sizeof(BITMAPFILEHEADER));
	if (bmfHdr.bfType != ((WORD)('M' << 8 | 'B'))) return;

	bmStart = bmfHdr.bfOffBits;

	file.Read(&bmpInfo, sizeof(BITMAPINFOHEADER));

	m_Width_Src = bmpInfo.biWidth;
	m_Height_Src = bmpInfo.biHeight;

	DWORD friction;
	int channels;
	switch (bmpInfo.biBitCount)
	{
	case 8:
		channels = 1;
		friction = (4 - (m_Width_Src & 3)) & 3;
		break;
	case 24:
		channels = 3;
		friction = (4 - (m_Width_Src & 3)) & 3;
		break;
	case 32:
		channels = 4;
		friction = 0;
		break;
	default:
		return;
	}

	BYTE buffer[4];
	file.Seek(bmStart, CFile::begin);
	for (int i = m_Height_Src - 1; i >= 0; i--) {
		for (int j = 0; j < m_Width_Src; j++) {
			file.Read(buffer, channels);
			if (channels == 1) image[i * m_Width_Src + j] = buffer[0];
			else image[i * m_Width_Src + j] = (BYTE)(0.11 * buffer[0] + 0.59 * buffer[1] + 0.3 * buffer[2]);
		}
		file.Read(buffer, friction);
	}
	file.Close();

	return;
}

void CFpImgResizeDlg::ImageView(unsigned char* image, int width, int height, int viewNum)
{
	// TODO: Add your implementation code here.

	int i, j, nByteBound;
	BITMAPINFOHEADER* bmi;
	BYTE* lpBits;
	int multiple = 1;//(m_btZoomOut.GetCheck()+1);

	BYTE up_ima[1281];

	if (viewNum == 0) {		//	Src
		bmi = (BITMAPINFOHEADER*)imadibSrc;
		lpBits = (BYTE*)imadibSrc + (*(LPDWORD)imadibSrc) + 256 * sizeof(RGBQUAD);
		bmi->biWidth = width * multiple;
		bmi->biHeight = height * multiple;
		nByteBound = ((bmi->biWidth + 3) >> 2) << 2;
		memset(lpBits, 0, (bmi->biHeight * nByteBound));
		
		for (i = 0; i < bmi->biHeight; i++) {
			for (j = 0; j < bmi->biWidth; j++) {
				up_ima[j] = image[j / multiple + (i / multiple) * bmi->biWidth / multiple];
			}
			BYTE * db_ima = lpBits + (bmi->biHeight - i - 1) * nByteBound;
			memcpy(db_ima, up_ima, bmi->biWidth);
		}

		CClientDC dc(&m_ftImage_Src);
		CRect rc;
		m_ftImage_Src.GetClientRect(&rc);
		OnDrawImage(&dc, rc, imadibSrc);
	}
	else {			//	Dst
		bmi = (BITMAPINFOHEADER*)imadibDst;
		lpBits = (BYTE*)imadibDst + (*(LPDWORD)imadibDst) + 256 * sizeof(RGBQUAD);
		bmi->biWidth = width * multiple;
		bmi->biHeight = height * multiple;
		nByteBound = ((bmi->biWidth + 3) >> 2) << 2;
		memset(lpBits, 0, (bmi->biHeight * nByteBound));

		for (i = 0; i < bmi->biHeight; i++) {
			for (j = 0; j < bmi->biWidth; j++) {
				up_ima[j] = image[j / multiple + (i / multiple) * bmi->biWidth / multiple];
			}
			BYTE * db_ima = lpBits + (bmi->biHeight - i - 1) * nByteBound;
			memcpy(db_ima, up_ima, bmi->biWidth);
		}

		CClientDC dc(&m_ftImage_Dst);
		CRect rc;
		m_ftImage_Dst.GetClientRect(&rc);
		OnDrawImage(&dc, rc, imadibDst);
	}
}

int CFpImgResizeDlg::OnDrawImage(CDC* pDC, CRect rc, BYTE* imadib)
{
	// TODO: Add your implementation code here.

	BITMAPINFOHEADER* bmi;
	BYTE* lpBits;
	static int i_drawed = 0;

	CRgn rgn;
	rgn.CreateRectRgnIndirect(&rc);
	pDC->SelectClipRgn(&rgn);

	bmi = (BITMAPINFOHEADER*)imadib;
	lpBits = (BYTE*)imadib + (*(LPDWORD)imadib) + 256 * sizeof(RGBQUAD);
	pDC->FillRect(&rc, CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetStretchBltMode(COLORONCOLOR);
	::StretchDIBits(pDC->m_hDC,
		0,
		0,
		bmi->biWidth,
		bmi->biHeight,
		0, 0,
		bmi->biWidth, bmi->biHeight,
		lpBits, (LPBITMAPINFO)bmi, DIB_RGB_COLORS, SRCCOPY);
	rc.right = rc.left + bmi->biWidth;
	rc.bottom = rc.top + bmi->biHeight;
	pDC->FrameRect(&rc, CBrush::FromHandle((HBRUSH)GetStockObject(LTGRAY_BRUSH)));

	pDC->SelectClipRgn(NULL);
	rgn.DeleteObject();

	return 0;
}

CString CFpImgResizeDlg::SelectFolderDialog()
{
	// TODO: Add your implementation code here.
	TCHAR      szFolderPath[MAX_PATH] = { 0 };
	CString     strFolderPath = TEXT("");

	BROWSEINFO      sInfo;  //文件夹属性结构体
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
	sInfo.pidlRoot = 0;
	sInfo.lpszTitle = _T("请选择处理结果存储路径:");
	sInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_DONTGOBELOWDOMAIN | BIF_NEWDIALOGSTYLE;
	sInfo.lpfn = NULL;

	// 显示文件夹选择对话框  
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);
	if (lpidlBrowse != NULL)
	{
		// 取得文件夹名  
		if (::SHGetPathFromIDList(lpidlBrowse, szFolderPath))
		{
			strFolderPath = szFolderPath;
		}
	}
	if (lpidlBrowse != NULL)
	{
		::CoTaskMemFree(lpidlBrowse);
	}
	//文件夹属性说明：
	//https://docs.microsoft.com/en-us/windows/desktop/api/shlobj_core/ns-shlobj_core-_browseinfoa


	return strFolderPath;
}

int CFpImgResizeDlg::ImageResize(BYTE* srcImage, int sw, int sh, BYTE* dstImage, int dw, int dh)
{
	// TODO: Add your implementation code here.

	if (sw < dw || sh < dh)	return 0;

	int dx = (sw - dw) / 2;
	int dy = (sh - dh) / 2;

	BYTE* p = srcImage + sw * dy + dx;
	BYTE* q = dstImage;

	for (int i = 0; i < dh; i++) {
		memcpy(q, p, dw);
		p += sw;
		q += dw;
	}

	return 1;
}

int CFpImgResizeDlg::SaveImageBmp(CString fn, BYTE* bmpImageBuf, int w, int h)
{
	// TODO: Add your implementation code here.

	// https://blog.csdn.net/michealmeng555/article/details/5751831
	// https://blog.csdn.net/t514528359zl/article/details/19970403?spm=1001.2101.3001.6650.1&utm_medium=distribute.pc_relevant.none-task-blog-2~default~BlogCommendFromBaidu~Rate-1-19970403-blog-108790408.pc_relevant_vip_default&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2~default~BlogCommendFromBaidu~Rate-1-19970403-blog-108790408.pc_relevant_vip_default&utm_relevant_index=2

	BITMAPINFOHEADER* bmi;
	BITMAPFILEHEADER fh;
	int	nByteBound;
	CString filename;

	bmi = (BITMAPINFOHEADER*)bmpImageBuf;
	nByteBound = ((bmi->biWidth + 3) >> 2) << 2;
	strcpy((char*)& fh.bfType, "BM");
	fh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 1024 + bmi->biHeight * nByteBound;
	fh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 1024;

	filename = fn;// +_T("_1.bmp");


	char* saveFileName = (LPSTR)(LPCTSTR)filename;
	//char* saveFileName = (char*)malloc(filename.GetLength());
	//memcpy(saveFileName, filename.GetBuffer(filename.GetLength()), filename.GetLength());
	//free(saveFileName);

	CStdioFile myFile;
	CFileException fileException;

	if (myFile.Open((LPCTSTR)filename, (CFile::typeBinary | CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyWrite), &fileException))
	{
		LONG64 length = sizeof(BITMAPINFOHEADER) + 1024 + bmi->biHeight * nByteBound;

		myFile.Write(&fh, sizeof(fh));
		myFile.Write(bmpImageBuf, (UINT)length);

	}
	else
	{
		TRACE("Can't open file %s,error=%u/n", saveFileName, fileException.m_cause);
	}
	
	myFile.Close();
	//fp = fopen((LPSTR)(LPCTSTR)filename, "wb");
	//fp = fopen(filename.GetBuffer(0), "wb");
	//if (fp) {
	//	fwrite(&fh, sizeof(fh), 1, fp);
	//	fwrite(bmpImageBuf, sizeof(BITMAPINFOHEADER) + 1024 + bmi->biHeight * nByteBound, 1, fp);
	//	fclose(fp);
	//}

	return 0;
}

int CFpImgResizeDlg::SaveImageRaw(CString fn, BYTE* RawImageBuf, LONG64 length)
{
	// TODO: Add your implementation code here.

	CString filename;

	filename = fn;

	char* saveFileName = (LPSTR)(LPCTSTR)filename;

	CStdioFile myFile;
	CFileException fileException;

	if (myFile.Open((LPCTSTR)filename, (CFile::typeBinary | CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyWrite), &fileException))
	{
		myFile.Write(RawImageBuf, (UINT)length);

	}
	else
	{
		TRACE("Can't open file %s,error=%u/n", saveFileName, fileException.m_cause);
	}

	myFile.Close();

	return 0;
}

int CFpImgResizeDlg::SavePlaneText(CString fn, CString str)
{
	// TODO: Add your implementation code here.

	CString filename;

	filename = fn;

	char* saveFileName = (LPSTR)(LPCTSTR)filename;

	CStdioFile myFile;
	CFileException fileException;

	if (myFile.Open((LPCTSTR)filename, (CFile::modeNoTruncate | CFile::modeCreate | CFile::modeWrite), &fileException))
	{
//		myFile.Write(RawImageBuf, (UINT)length);
		myFile.SeekToEnd();
		myFile.WriteString(str);


	}
	else
	{
		TRACE("Can't open file %s,error=%u/n", saveFileName, fileException.m_cause);
	}

	myFile.Close();

	return 0;

}

int CFpImgResizeDlg::MultiResize(CString path, int kind)
{
	// TODO: Add your implementation code here.

	CString strText_i, str;
	CFileFind finder;

	bool bFindFolder = finder.FindFile(path + "\\*.*");
	while (bFindFolder)
	{
		bFindFolder = finder.FindNextFile();
		if (finder.IsDots())
			continue;
		if (finder.IsDirectory())//是文件夹
		{
			//FindAllFile(strParent+finder.GetFileName()+"\\");//递归打开文件夹   

		}

	}

	bool bFind = finder.FindFile(path + "\\*.*");
	while (bFind)
	{
		bFind = finder.FindNextFile();
		if (finder.IsDots())
			continue;
		if (finder.IsDirectory())//是文件夹
		{
			//FindAllFile(strParent+finder.GetFileName()+"\\");//递归打开文件夹   
			CString subPn = finder.GetFileName();//文件夹名称
			MultiResize(path + "\\" + subPn, kind);
		}
		else {
			CString fn = path + "\\" + finder.GetFileName();//文件夹名称dlg.GetPathName();
			int dotPos = fn.ReverseFind('.');
			CString fileExt = fn.Right(fn.GetLength() - dotPos);
			CString fn_r;
			if (fileExt == ".bmp") {
				ReadImageBmp(fn, m_bImageSrc);
				fn_r.Format(_T("%s_%d_%d.bmp"), fn, m_Width_Dst, m_Height_Dst);

				ImageView(m_bImageSrc, m_Width_Src, m_Height_Src, 0);
				//m_msg = "图像读取完成.";

				// Filtering
				//ImageFilterF(m_bImageOrigin);
				//m_nWidthDst = m_Width_Dst;
				//m_nHeightDst = m_Height_Dst;
				switch (kind) {
				case PROC_RESIZE:		//	
					ImageResize(m_bImageSrc, m_Width_Src, m_Height_Src, m_bImageDst, m_Width_Dst, m_Height_Dst);
					break;
				case PROC_TRANSFORM:
					ImageTransform(m_bImageSrc, m_Width_Src, m_Height_Src, m_bImageDst, m_Width_Dst, m_Height_Dst);
					break;
				case PROC_CONVERT:
					break;
				}

				ImageView(m_bImageDst, m_Width_Dst, m_Height_Dst, 2);
				//m_msg_new = "图像处理完成.";

				//	save
				if (m_fSave) {
					SaveImageBmp(fn_r, imadibDst, m_Width_Dst, m_Height_Dst);
				}
			}
			else if (fileExt == ".raw") {

				// Read file contents
				CFile file(fn, CFile::modeRead);
				UINT cnt = (UINT)file.GetLength();
				file.Read(m_bImageSrc, cnt);
				file.Close();

				fn_r.Format(_T("%s_%d_%d.raw"), fn, m_Width_Dst, m_Height_Dst);

				ImageView(m_bImageSrc, m_Width_Src, m_Height_Src, 0);
				//m_msg = "图像读取完成.";

				// Filtering
				//ImageFilterF(m_bImageOrigin);
				//m_nWidthDst = m_Width_Dst;
				//m_nHeightDst = m_Height_Dst;
				ImageResize(m_bImageSrc, m_Width_Src, m_Height_Src, m_bImageDst, m_Width_Dst, m_Height_Dst);

				ImageView(m_bImageDst, m_Width_Dst, m_Height_Dst, 2);
				//m_msg_new = "图像处理完成.";

				//	save
				if (m_fSave) {
					SaveImageRaw(fn_r, m_bImageDst, m_Width_Dst*m_Height_Dst);
				}
			}
			else	continue;
		}
	}

	return 0;
}

int CFpImgResizeDlg::MultiConvert(CString path)
{
	// TODO: Add your implementation code here.

	CString strText_i, str;
	CFileFind finder;

	bool bFindFolder = finder.FindFile(path + "\\*.*");
	while (bFindFolder)
	{
		bFindFolder = finder.FindNextFile();
		if (finder.IsDots())
			continue;
		if (finder.IsDirectory())//是文件夹
		{
			//FindAllFile(strParent+finder.GetFileName()+"\\");//递归打开文件夹   

		}

	}

	bool bFind = finder.FindFile(path + "\\*.*");
	while (bFind)
	{
		bFind = finder.FindNextFile();
		if (finder.IsDots())
			continue;
		if (finder.IsDirectory())//是文件夹
		{
			//FindAllFile(strParent+finder.GetFileName()+"\\");//递归打开文件夹   
			CString subPn = finder.GetFileName();//文件夹名称
			MultiConvert(path + "\\" + subPn);
		}
		else {
			CString fn = path + "\\" + finder.GetFileName();//文件夹名称dlg.GetPathName();
			int dotPos = fn.ReverseFind('.');
			CString fileExt = fn.Right(fn.GetLength() - dotPos);
			CString fn_r;
			if (fileExt == ".bmp") {
				ReadImageBmp(fn, m_bImageSrc);

				DeleteFile(fn);

				//fn_r.Format(_T("%s.raw"), fn);
				fn_r.Format(_T("%s.raw"), fn.Left(dotPos));

				ImageView(m_bImageSrc, m_Width_Cvt, m_Height_Cvt, 0);

				//	save
				if (m_fSave) {
					SaveImageRaw(fn_r, m_bImageSrc, m_Width_Cvt * m_Height_Cvt);
				}
			}
			else if (fileExt == ".raw") {

				// Read file contents
				CFile file(fn, CFile::modeRead);
				UINT cnt = (UINT)file.GetLength();
				file.Read(m_bImageSrc, cnt);
				file.Close();

				DeleteFile(fn);

				//fn_r.Format(_T("%s.bmp"), fn);
				fn_r.Format(_T("%s.bmp"), fn.Left(dotPos));

				ImageView(m_bImageSrc, m_Width_Cvt, m_Height_Cvt, 0);

				//	save
				if (m_fSave) {
					SaveImageBmp(fn_r, imadibSrc, m_Width_Cvt, m_Height_Cvt);
				}
			}
			else	continue;
		}
	}

	return 0;
}

int CFpImgResizeDlg::MultiCutting(CString path)
{
	// TODO: Add your implementation code here.

	CString strText_i, str;
	CFileFind finder;

	bool bFind = finder.FindFile(path + "\\*.*");
	while (bFind)
	{
		bFind = finder.FindNextFile();
		if (finder.IsDots())
			continue;
		if (finder.IsDirectory())//是文件夹
		{
			//FindAllFile(strParent+finder.GetFileName()+"\\");//递归打开文件夹   
			CString subPn = finder.GetFileName();//文件夹名称
			MultiCutting(path + "\\" + subPn);
		}
		else {
			CString fn = path + "\\" + finder.GetFileName();//文件夹名称dlg.GetPathName();
			int dotPos = fn.ReverseFind('.');
			CString fileExt = fn.Right(fn.GetLength() - dotPos);
			CString fn_r;
			if (fileExt == ".bmp") {
				ReadImageBmp(fn, m_bImageSrc);

				DeleteFile(fn);

				//fn_r.Format(_T("%s.raw"), fn);
				fn_r.Format(_T("%s.bmp"), fn.Left(dotPos));

				for (int kk = 0; kk < 160; kk++) {
					memcpy(m_bImageTmp + 64 * kk, m_bImageSrc + 80 * kk, 64);
				}

				ImageView(m_bImageTmp, 64, 160, 0);

				//	save
				if (m_fSave) {
					SaveImageBmp(fn_r, imadibSrc, 64, 160);
				}
			}
			else if (fileExt == ".raw") {

				// Read file contents
				CFile file(fn, CFile::modeRead);
				UINT cnt = (UINT)file.GetLength();
				file.Read(m_bImageSrc, cnt);
				file.Close();

				DeleteFile(fn);

				//fn_r.Format(_T("%s.bmp"), fn);
				fn_r.Format(_T("%s.raw"), fn.Left(dotPos));

				for (int kk = 0; kk < 160; kk++) {
					memcpy(m_bImageTmp + 64 * kk, m_bImageSrc + 80 * kk, 64);
				}

				ImageView(m_bImageTmp, 64, 160, 0);

				//	save
				if (m_fSave) {
					SaveImageRaw(fn_r, imadibSrc, 64 * 160);
				}
			}
			else	continue;
		}
	}

	return 0;
}

int CFpImgResizeDlg::MultiExpand(CString path)
{
	// TODO: Add your implementation code here.

	CString strText_i, str;
	CFileFind finder;

	bool bFind = finder.FindFile(path + "\\*.*");
	while (bFind)
	{
		bFind = finder.FindNextFile();
		if (finder.IsDots())
			continue;
		if (finder.IsDirectory())//是文件夹
		{
			//FindAllFile(strParent+finder.GetFileName()+"\\");//递归打开文件夹   
			CString subPn = finder.GetFileName();//文件夹名称
			MultiExpand(path + "\\" + subPn);
		}
		else {
			CString fn = path + "\\" + finder.GetFileName();//文件夹名称dlg.GetPathName();
			int dotPos = fn.ReverseFind('.');
			CString fileExt = fn.Right(fn.GetLength() - dotPos);
			CString fn_r;
			if (fileExt == ".bmp") {
				ReadImageBmp(fn, m_bImageSrc);

				DeleteFile(fn);

				//fn_r.Format(_T("%s.raw"), fn);
				fn_r.Format(_T("%s.bmp"), fn.Left(dotPos));

				memset(m_bImageTmp, 0, 80 * 160);
				for (int kk = 0; kk < 160; kk++) {
					memcpy(m_bImageTmp + 80 * kk, m_bImageSrc + 64 * kk, 32);
					memcpy(m_bImageTmp + 80 * kk + 32 + 7, m_bImageSrc + 64 * kk + 32, 32);
				}

				ImageView(m_bImageTmp, 80, 160, 0);

				//	save
				if (m_fSave) {
					SaveImageBmp(fn_r, imadibSrc, 80, 160);
				}
			}
			else if (fileExt == ".raw") {

				// Read file contents
				CFile file(fn, CFile::modeRead);
				UINT cnt = (UINT)file.GetLength();
				file.Read(m_bImageSrc, cnt);
				file.Close();

				DeleteFile(fn);

				//fn_r.Format(_T("%s.bmp"), fn);
				fn_r.Format(_T("%s.raw"), fn.Left(dotPos));

				for (int kk = 0; kk < 160; kk++) {
					memcpy(m_bImageTmp + 80 * kk, m_bImageSrc + 64 * kk, 32);
					memcpy(m_bImageTmp + 80 * kk + 32 + 7, m_bImageSrc + 64 * kk + 32, 32);
				}

				ImageView(m_bImageTmp, 80, 160, 0);

				//	save
				if (m_fSave) {
					SaveImageRaw(fn_r, imadibSrc, 80 * 160);
				}
			}
			else	continue;
		}
	}

	return 0;
}

bool Image2Bytes(const CImage& img, BYTE** bytes, size_t& byteSize) {
	if (img.IsNull()) return false;
	IStream* pStrImg = NULL;
	if (CreateStreamOnHGlobal(NULL, TRUE, &pStrImg) != S_OK) return false;
	img.Save(pStrImg, Gdiplus::ImageFormatBMP);
	HGLOBAL hGlobalImg = NULL;
	GetHGlobalFromStream(pStrImg, &hGlobalImg);
	BYTE* pBits = (BYTE*)GlobalLock(hGlobalImg);
	byteSize = GlobalSize(pBits);
	*bytes = new BYTE[byteSize];
	memcpy(*bytes, pBits, byteSize);
	GlobalUnlock(hGlobalImg);
	pStrImg->Release();
	GlobalFree(hGlobalImg);
	return true;
}

bool Bytes2Image(const BYTE* bytes, const size_t byteSize, CImage& img) {
	if (bytes == NULL) return false;
	HGLOBAL hGlobalImg = GlobalAlloc(GMEM_MOVEABLE, byteSize);
	BYTE* pBits = (BYTE*)GlobalLock(hGlobalImg);
	memcpy(pBits, bytes, byteSize);
	GlobalUnlock(hGlobalImg);
	IStream* pStrImg = NULL;
	if (CreateStreamOnHGlobal(hGlobalImg, TRUE, &pStrImg) != S_OK) {
		GlobalFree(hGlobalImg);
		return false;
	}
	if (!img.IsNull()) img.Destroy();
	img.Load(pStrImg);
	pStrImg->Release();
	GlobalFree(hGlobalImg);
	return true;
}

void calc_histogram(uint8_t* I, int* H, int rows, int cols)
{
	int n, N = rows * cols;
	for (n = 0; n < 256; n++) H[n] = 0;
	for (n = 0; n < N; n++) H[I[n]]++;
}
void check_histogram_1(int* H, int th1, int th2, uint8_t* c)
{
	// c[0] < c[1] < c[2]
	// c[0]: low  limit of gray level
	// c[1]: center of gray range
	// c[2]: high limit of gray level

	int i, sum, c0, c1, c2;

	// darkness
	sum = 0; c0 = 0;
	for (i = 0; i < 256; i++) {
		sum += H[i];
		if (sum > th1) {
			c0 = i;
			break;
		}
	}

	// lightness
	sum = 0; c2 = 255;
	for (i = 255; i >= 0; i--) {
		sum += H[i];
		if (sum > th2) {
			c2 = i;
			break;
		}
	}

	c1 = (c0 + c2) / 2;

	c[0] = (uint8_t)c0;
	c[1] = (uint8_t)c1;
	c[2] = (uint8_t)c2;

}
uint8_t otsu_thresh_2(int* counts)
{
	uint8_t id2, id3, id, th, id1[256];
	int i, num_bins, sumcounts, n, sumth;
	float mu_t, maxval, pi, sump, summu, prevsigma, sigmai, maxval2, ts, temp;
	float omega[256], mu[256], sigma[256], dsigma[256];

	//
	num_bins = 256;
	sumcounts = 0;
	for (i = 0; i < num_bins; i++)
		sumcounts += counts[i];

	//
	sump = 0; summu = 0;
	for (i = 0; i < num_bins; i++) {

		pi = (float)counts[i] / sumcounts;
		sump += pi;
		summu += (pi * i);

		omega[i] = sump;
		mu[i] = summu;

	}
	mu_t = mu[num_bins - 1];

	//
	maxval = 0;
	for (i = 0; i < num_bins; i++) {
		temp = mu_t * omega[i] - mu[i];
		sigmai = temp * temp / (omega[i] * (1.0f - omega[i]));
		if (sigmai > maxval)
			maxval = sigmai;
		sigma[i] = sigmai;
		if (i == 0)
			dsigma[i] = 0.0f;
		else
			dsigma[i] = (float)fabs((double)(sigmai - prevsigma));
		prevsigma = sigmai;
	}

	//
	maxval2 = maxval / 2.0f;
	for (i = 0; i < num_bins; i++)
		id1[i] = sigma[i] > maxval2;
	ts = 1.0f;
	while (1) {

		n = 0; sumth = 0;
		for (i = 0; i < num_bins; i++) {
			id2 = dsigma[i] < ts;
			id3 = id1[i] * id2;
			id = id3 > 0;
			if (id > 0) {
				n++;
				th = (uint8_t)i;
				sumth += (int)th;
			}
		}
		if (n == 0) {
			ts = ts + 1.0f;
			if (ts >= 50.0f) {
				n = 1;
				sumth = 128;
				break;
			}
		}
		else
			break;

	}
	return (uint8_t)(sumth / n);
}
//
int image_score(uint8_t* I)
{
	uint8_t th;
	uint8_t C[3];
	int n, rows, cols, H[256], H2[256], sum, N, score;

	rows = 360; cols = 256; N = rows * cols;

	calc_histogram(I, H, rows, cols);
	check_histogram_1(H, 1024, 1024, C);
	th = otsu_thresh_2(H);

	sum = 0;
	for (n = 0; n < N; n++) {
		if (I[n] < th) sum++;
	}
	return (sum * 100) / N;
}


void CFpImgResizeDlg::OnBnClickedAnalysis()
{
	// TODO: Add your control notification handler code here

	CString path = OnReadImage();

//	int score = getImageAreaScore((uint8_t*)m_bImageSrc, 256, 360);
//	m_msgLeft.Format(_T("score:%d"), score);

//	int globalScore = 0, areaScore = 0, wetScore = 0;
//	LAPI_IsFinger(m_bImageSrc, m_Width_Src, m_Height_Src, &globalScore, &areaScore, &wetScore);
	m_msgLeft.Format(_T("file:%s"), path);
//	m_msgRight.Format(_T("score:%d, gscore:%d, aScore = %d, wScore = %d"), score, globalScore, areaScore, wetScore);


	int sc = image_score(m_bImageSrc);
	m_msgRight.Format(_T("score:%d"), sc);

	UpdateData(FALSE);
	//MULTI_PROCESS();

}


void CFpImgResizeDlg::OnBnClickedConvert()
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);

	if (m_fMulti) {

//		CString pn = SelectFolderDialog();

//		if (pn == "")	return;

//		MultiConvert(pn);
		CFolderPickerDialog cfd(NULL, 0, this, 0);

		if (cfd.DoModal() == IDOK)
		{
			CString strFolder = cfd.GetPathName();

			MultiConvert(strFolder);
		}
		
	}
	else {
		CFileDialog dlg(TRUE, _T("File Open"), NULL, NULL, _T("Raw Files(*.raw)|*.raw|Bmp Files (*.bmp)|*.bmp|All Files|*.*||"), this);
		if (dlg.DoModal() == IDCANCEL) return;

		CString path = dlg.GetPathName();
		CString path_r;

		CString extention = PathFindExtension(path);
		if (extention == ".bmp")
		{
			ReadImageBmp(path, m_bImageSrc);

			path_r.Format(_T("%s.raw"), path);
		}
		else if (extention == ".raw")
		{
			// Read file contents
			CFile file(path, CFile::modeRead);
			UINT cnt = (UINT)file.GetLength();
			file.Read(m_bImageSrc, cnt);
			file.Close();

			path_r.Format(_T("%s.bmp"), path);
		}
		else return;

		ImageView(m_bImageSrc, m_Width_Cvt, m_Height_Cvt, 0);

		if (m_fSave) {
			if (extention == ".bmp")
			{
				SaveImageRaw(path_r, m_bImageSrc, m_Width_Cvt * m_Height_Cvt);
			}
			else if (extention == ".raw")
			{
				SaveImageBmp(path_r, imadibSrc, m_Width_Cvt, m_Height_Cvt);
			}

		}

	}


	UpdateData(FALSE);
}


void CFpImgResizeDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}


void CFpImgResizeDlg::OnEnChangeEditSrcwidth()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
//	m_nWidthSrc = m_Width_Src;
}


void CFpImgResizeDlg::OnEnChangeEditSrcheight()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here

//	m_nHeightSrc = m_Height_Src;

}


void CFpImgResizeDlg::OnEnChangeEditDstwidth()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
//	m_nWidthDst = m_Width_Dst;
}


void CFpImgResizeDlg::OnEnChangeEditDstheight()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
//	m_nHeightDst = m_Height_Dst;
}


int CFpImgResizeDlg::MultiDetect(CString path)
{
	// TODO: Add your implementation code here.

	CString strText_i, str;
	CFileFind finder;

	//bool bFindFolder = finder.FindFile(path + "\\*.*");
	//while (bFindFolder)
	//{
	//	bFindFolder = finder.FindNextFile();
	//	if (finder.IsDots())
	//		continue;
	//	if (finder.IsDirectory())//是文件夹
	//	{
	//		//FindAllFile(strParent+finder.GetFileName()+"\\");//递归打开文件夹   

	//	}

	//}

	bool bFind = finder.FindFile(path + "\\*.*");
	while (bFind)
	{
		bFind = finder.FindNextFile();
		if (finder.IsDots())
			continue;
		if (finder.IsDirectory())//是文件夹
		{
			//FindAllFile(strParent+finder.GetFileName()+"\\");//递归打开文件夹   
			CString subPn = finder.GetFileName();//文件夹名称
			MultiConvert(path + "\\" + subPn);
		}
		else {
			int globalScore = 0, areaScore = 0, wetScore = 0;
			
			CString fn = path + "\\" + finder.GetFileName();//文件夹名称dlg.GetPathName();
			int dotPos = fn.ReverseFind('.');
			CString fileExt = fn.Right(fn.GetLength() - dotPos);
			CString fn_r = LogPath;

			if (fileExt == ".bmp") {
				ReadImageBmp(fn, m_bImageSrc);

				ImageView(m_bImageSrc, m_Width_Cvt, m_Height_Cvt, 0);

				int score = getImageAreaScore((uint8_t*)m_bImageSrc, m_Width_Cvt, m_Height_Cvt);
				LAPI_IsFinger(m_bImageSrc, m_Width_Src, m_Height_Src, &globalScore, &areaScore, &wetScore);

				m_msgRight.Format(_T("file:%s ==> score:%d, gscore:%d, aScore = %d, wScore = %d\r\n"), fn, score, globalScore, areaScore, wetScore);
				m_msgRight.Format(_T("%d %d %d %d\n"), score, globalScore, areaScore, wetScore);

				//	save
				if (m_fSave) {
					SavePlaneText(fn_r, m_msgRight);
				}
			}
			else if (fileExt == ".raw") {

				// Read file contents
				CFile file(fn, CFile::modeRead);
				UINT cnt = (UINT)file.GetLength();
				file.Read(m_bImageSrc, cnt);
				file.Close();

				ImageView(m_bImageSrc, m_Width_Cvt, m_Height_Cvt, 0);

				int score = getImageAreaScore((uint8_t*)m_bImageSrc, m_Width_Cvt, m_Height_Cvt);

				LAPI_IsFinger(m_bImageSrc, m_Width_Src, m_Height_Src, &globalScore, &areaScore, &wetScore);

				m_msgRight.Format(_T("file:%s ==> score:%d, gscore:%d, aScore = %d, wScore = %d\r\n"), fn, score, globalScore, areaScore, wetScore);
				m_msgRight.Format(_T("%d %d %d %d\n"), score, globalScore, areaScore, wetScore);

				//	save
				if (m_fSave) {
					SavePlaneText(fn_r, m_msgRight);
				}
			}
			else	continue;
		}
	}

	return 0;
}

unsigned char ztemp[1024];
UINT32* ptemp;
UINT32 aaaaa1, aaaaa2, aaaaa3, aaaaa4;
unsigned char aa1, aa2, aa3, aa4;
void CFpImgResizeDlg::OnBnClickedDetect()
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);


	//for (int i = 0; i < 1024; i++) {
	//	ztemp[i] = (unsigned char)(i & 0xFF);
	//}

	//ptemp = (UINT32*)ztemp;

	//aaaaa1 = *ptemp;	ptemp++;
	//aaaaa2 = *ptemp;	ptemp++;
	//aaaaa3 = *ptemp;	ptemp++;
	//aaaaa4 = *ptemp;	ptemp++;

	//aa1 = (aaaaa2 >> 0) & 0xff;
	//aa2 = (aaaaa2 >> 8) & 0xff;
	//aa3 = (aaaaa2 >> 16) & 0xff;
	//aa4 = (aaaaa2 >> 24) & 0xff;

	//return;
	if (m_fMulti) {
		CFolderPickerDialog cfd(NULL, 0, this, 0);

		if (cfd.DoModal() == IDOK)
		{
			CString strFolder = cfd.GetPathName();

			LogPath.Format(_T("%s\scoreLog.txt"), strFolder);

			MultiDetect(strFolder);
		}
	}
	else {

		CFileDialog dlg(TRUE, _T("File Open"), NULL, NULL, _T("Bmp Files (*.bmp)|*.bmp|Raw Files(*.raw)|*.raw|All Files|*.*||"), this);
		if (dlg.DoModal() == IDCANCEL) return;

		CString path = dlg.GetPathName();
		CString path_r;

		CString extention = PathFindExtension(path);
		if (extention == ".bmp")
		{
			ReadImageBmp(path, m_bImageSrc);

			path_r.Format(_T("%s_%d_%d.bmp"), path, m_Width_Cvt, m_Height_Cvt);
		}
		else if (extention == ".raw")
		{
			// Read file contents
			CFile file(path, CFile::modeRead);
			UINT cnt = (UINT)file.GetLength();
			file.Read(m_bImageSrc, cnt);
			file.Close();

			path_r.Format(_T("%s_%d_%d.raw"), path, m_Width_Cvt, m_Height_Cvt);
		}
		else return;

		ImageView(m_bImageSrc, m_Width_Cvt, m_Height_Cvt, 0);

		int globalScore = 0, areaScore = 0, wetScore = 0;

		int score = getImageAreaScore((uint8_t*)m_bImageSrc, m_Width_Cvt, m_Height_Cvt);
		m_msgLeft.Format(_T("score:%d"), score);

		LAPI_IsFinger(m_bImageSrc, m_Width_Cvt, m_Height_Cvt, &globalScore, &areaScore, &wetScore);
		m_msgLeft.Format(_T("file:%s"), path);
		m_msgRight.Format(_T("score:%d, gscore:%d, aScore = %d, wScore = %d"), score, globalScore, areaScore, wetScore);

		//if (globalScore >= g_stDeviceInfo.m_byFPCheckScore) return FINGER_ERR_SUCCESS;
		////	if (globalScore < AREA_THRES) return FINGER_ERR_FP_NOT_DETECTED;
		//if (areaScore < AREA_THRES) return FINGER_ERR_FP_NOT_DETECTED;
		//if (areaScore < g_stDeviceInfo.m_byFPCheckScore) return FINGER_ERR_SMALL_AREA;
		//if (wetScore > 0) return FINGER_ERR_WET_FINGER;
		//if (wetScore < 0) return FINGER_ERR_DRY_FINGER;
	}





	UpdateData(FALSE);

}


CString CFpImgResizeDlg::OnReadImage()
{
	// TODO: Add your implementation code here.

	CString path;

	// TODO: 在此添加命令处理程序代码
	CFileDialog fileDlg(TRUE);
	fileDlg.m_ofn.lpstrFilter = _T("Bitmap Files(*.bmp)\0*.bmp\0Raw Files(*.raw)\0*.raw\0All Files(*.*)\0*.*\0\0");
	if (IDOK == fileDlg.DoModal())
	{
		path = fileDlg.GetPathName();
	}

	CString extention = PathFindExtension(path);

	// Read file contents
	CFile file(path, CFile::modeRead);
	UINT cnt = (UINT)file.GetLength();
	file.Read(m_bImageSrc, cnt);
	file.Close();

	//load image to cimage class
	CImage image;
	if (extention == ".bmp")
	{
		image.Load(path);
	}
	else if (extention == ".raw")
	{
		image.Create(m_Width_Src, m_Height_Src, 8/*byte per pixcel*/);
		//Bytes2Image(m_bImage, 256 * 360, image);

		RGBQUAD* tab = new RGBQUAD[256];
		for (int i = 0; i < 256; ++i)
		{
			tab[i].rgbRed = i;
			tab[i].rgbGreen = i;
			tab[i].rgbBlue = i;
			tab[i].rgbReserved = 0;
		}
		image.SetColorTable(0, 256, tab);
		delete[] tab;

		// Copy pixel values
		// Warining: does not convert from RGB to BGR
		for (int i = 0; i < m_Height_Src; i++)
		{
			void* dst = image.GetPixelAddress(0, i);
			const void* src = &m_bImageSrc[i * m_Width_Src]/* put the pointer to the i'th source row here */;
			memcpy(dst, src, m_Width_Src);
		}
		//int pp = 0;
		//for (int row = 0; row < m_nHeight; row++) {
		//	for (int col = 0; col < m_nWidth; col++) {
		//		BYTE px = m_bImage[pp++];
		//		image.SetPixel(row, col, COLOR16(px));
		//	}
		//}
	}
	else
	{
		return path;
	}

	// TODO: display image
	CStatic* picturebox = (CStatic*)(GetDlgItem(IDC_IMAGE_SRC));
	CRect rect;
	picturebox->GetClientRect(rect);
	// create CClientDC for picturebox
	CClientDC dc(picturebox);
	//load image to cBitmap class
	CBitmap m_bitmap;
	m_bitmap.Attach(image.Detach());
	//create cdc to load bitmap
	CDC memoryDC;
	memoryDC.CreateCompatibleDC(&dc);
	//add cbiteap to memoryoc object;
	memoryDC.SelectObject(m_bitmap);
	//get bitmap dimenation for m_bitmap
	BITMAP bmp;
	m_bitmap.GetBitmap(&bmp);
	//set stretch build mode to color on color
	dc.SetStretchBltMode(COLORONCOLOR);
	dc.StretchBlt(rect.left, rect.top, rect.Width(), rect.Height(), &memoryDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);


	//	calculation for score
	UpdateData(TRUE);

	return path;

}


void CFpImgResizeDlg::OnEnChangeEditCvtwidth()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here

}


void CFpImgResizeDlg::OnBnClickedTransform()
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);

	if (m_fMulti) {
		CFolderPickerDialog cfd(NULL, 0, this, 0);

		if (cfd.DoModal() == IDOK)
		{
			CString strFolder = cfd.GetPathName();

			MultiResize(strFolder, PROC_TRANSFORM);
		}
	}
	else {
		CFileDialog dlg(TRUE, _T("File Open"), NULL, NULL, _T("Raw Files(*.raw)|*.raw|Bmp Files (*.bmp)|*.bmp|All Files|*.*||"), this);
		if (dlg.DoModal() == IDCANCEL) return;

		CString path = dlg.GetPathName();
		CString path_r;

		CString extention = PathFindExtension(path);
		if (extention == ".bmp")
		{
			ReadImageBmp(path, m_bImageSrc);

			path_r.Format(_T("%s_%d_%d.bmp"), path, m_Width_Dst, m_Height_Dst);
		}
		else if (extention == ".raw")
		{
			// Read file contents
			CFile file(path, CFile::modeRead);
			UINT cnt = (UINT)file.GetLength();
			file.Read(m_bImageSrc, cnt);
			file.Close();

			path_r.Format(_T("%s_%d_%d.raw"), path, m_Width_Dst, m_Height_Dst);
		}
		else return;

		ImageView(m_bImageSrc, m_Width_Src, m_Height_Src, 0);

		// Filtering
		//m_nWidthDst = m_Width_Dst;
		//m_nHeightDst = m_Height_Dst;
		
		ImageTransform(m_bImageSrc, m_Width_Src, m_Height_Src, m_bImageDst, m_Width_Dst, m_Height_Dst);

		ImageView(m_bImageDst, m_Width_Dst, m_Height_Dst, 2);

		if (m_fSave) {
			if (extention == ".bmp")
			{
				SaveImageBmp(path_r, imadibDst, m_Width_Dst, m_Height_Dst);
			}
			else if (extention == ".raw")
			{
				SaveImageRaw(path_r, m_bImageDst, m_Width_Dst * m_Height_Dst);
			}

		}
		//		m_msg_new = "图像处理完成.";

	}

	UpdateData(FALSE);

}


void CFpImgResizeDlg::OnBnClickedCutting()
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);

	if (m_fMulti) {

		//		CString pn = SelectFolderDialog();

		//		if (pn == "")	return;

		//		MultiConvert(pn);
		CFolderPickerDialog cfd(NULL, 0, this, 0);

		if (cfd.DoModal() == IDOK)
		{
			CString strFolder = cfd.GetPathName();

			MultiCutting(strFolder);
		}

	}
	else {
		CFileDialog dlg(TRUE, _T("File Open"), NULL, NULL, _T("Bmp Files (*.bmp)|*.bmp|Raw Files(*.raw)|*.raw|All Files|*.*||"), this);
		if (dlg.DoModal() == IDCANCEL) return;

		CString path = dlg.GetPathName();
		CString path_r;

		CString extention = PathFindExtension(path);
		if (extention == ".bmp")
		{
			ReadImageBmp(path, m_bImageSrc);

			path_r.Format(_T("%s.bmp"), path.Mid(0, path.GetLength() - 4));
		}
		else if (extention == ".raw")
		{
			// Read file contents
			CFile file(path, CFile::modeRead);
			UINT cnt = (UINT)file.GetLength();
			file.Read(m_bImageSrc, cnt);
			file.Close();

			path_r.Format(_T("%s.raw"), path.Mid(0, path.GetLength()-4));
		}
		else return;

		for (int kk = 0; kk < 160; kk++) {
			memcpy(m_bImageTmp + 64*kk, m_bImageSrc+ 80*kk, 64);
		}

		ImageView(m_bImageTmp, 64, m_Height_Cvt, 0);

		if (m_fSave) {
			if (extention == ".bmp")
			{
				SaveImageBmp(path_r, imadibSrc, 64, m_Height_Cvt);
			}
			else if (extention == ".raw")
			{
				SaveImageRaw(path_r, m_bImageTmp, 64 * m_Height_Cvt);
			}

		}

	}

	UpdateData(FALSE);
}


void CFpImgResizeDlg::OnBnClickedExpand()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if (m_fMulti) {

		CFolderPickerDialog cfd(NULL, 0, this, 0);

		if (cfd.DoModal() == IDOK)
		{
			CString strFolder = cfd.GetPathName();

			MultiExpand(strFolder);
		}

	}
	else {
		CFileDialog dlg(TRUE, _T("File Open"), NULL, NULL, _T("Bmp Files (*.bmp)|*.bmp|Raw Files(*.raw)|*.raw|All Files|*.*||"), this);
		if (dlg.DoModal() == IDCANCEL) return;

		CString path = dlg.GetPathName();
		CString path_r;

		CString extention = PathFindExtension(path);
		if (extention == ".bmp")
		{
			ReadImageBmp(path, m_bImageSrc);

			path_r.Format(_T("%s.bmp"), path.Mid(0, path.GetLength() - 4));
		}
		else if (extention == ".raw")
		{
			// Read file contents
			CFile file(path, CFile::modeRead);
			UINT cnt = (UINT)file.GetLength();
			file.Read(m_bImageSrc, cnt);
			file.Close();

			path_r.Format(_T("%s.raw"), path.Mid(0, path.GetLength() - 4));
		}
		else return;

		memset(m_bImageTmp, 0, 80 * 160);
		for (int kk = 0; kk < 160; kk++) {
			memcpy(m_bImageTmp + 80 * kk, m_bImageSrc + 64 * kk, 32);
			memcpy(m_bImageTmp + 80 * kk + 32 + 7, m_bImageSrc + 64 * kk + 32, 32);
		}

		ImageView(m_bImageTmp, 80, m_Height_Cvt, 0);

		if (m_fSave) {
			if (extention == ".bmp")
			{
				SaveImageBmp(path_r, imadibSrc, 80, m_Height_Cvt);
			}
			else if (extention == ".raw")
			{
				SaveImageRaw(path_r, m_bImageTmp, 80 * m_Height_Cvt);
			}

		}

	}

	UpdateData(FALSE);
}
