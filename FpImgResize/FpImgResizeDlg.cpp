
// FpImgResizeDlg.cpp : implementation file
//

#include "pch.h"
#include <stdint.h>
#include "framework.h"
#include "FpImgResize.h"
#include "FpImgResizeDlg.h"
#include "afxdialogex.h"

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

BYTE imadibSrc[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256 + 1024 * 1024];
BYTE imadibDst[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256 + 1024 * 1024];

int  m_nWidthSrc = 256;
int  m_nHeightSrc = 360;
int  m_nImaSizeSrc = m_nWidthSrc * m_nHeightSrc;

int  m_nWidthDst = 208;
int  m_nHeightDst = 388;
int  m_nImaSizeDst = m_nWidthDst * m_nHeightDst;


// CFpImgResizeDlg dialog

CFpImgResizeDlg::CFpImgResizeDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FPIMGRESIZE_DIALOG, pParent)
	, m_Height_Dst(288)
	, m_Width_Dst(208)
	, m_Height_Src(360)
	, m_Width_Src(256)
	, m_fSave(FALSE)
	, m_fMulti(FALSE)
	, m_fShow(TRUE)
	, m_msgLeft(_T(""))
	, m_msgRight(_T(""))
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
	DDX_Check(pDX, IDC_CHECK_SHOW, m_fShow);
	DDX_Text(pDX, IDC_MSG_LEFT, m_msgLeft);
	DDX_Text(pDX, IDC_MSG_RIGHT, m_msgRight);
}

BEGIN_MESSAGE_MAP(CFpImgResizeDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, &CFpImgResizeDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_ANALYSIS, &CFpImgResizeDlg::OnBnClickedAnalysis)
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

	m_btnFont.CreatePointFont(300, _T("黑体"), NULL);
	GetDlgItem(IDC_START)->SetFont(&m_btnFont);
	GetDlgItem(IDCANCEL)->SetFont(&m_btnFont);

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


void CFpImgResizeDlg::OnBnClickedStart()
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);

	if (m_fMulti) {
		CString pn = SelectFolderDialog();
		if (pn == "")	return;

		MultiResize(pn);
	}
	else {
		CFileDialog dlg(TRUE, _T("File Open"), NULL, NULL, _T("Finger Files (*.bmp)|*.bmp|All Files|*.*||"), this);
		if (dlg.DoModal() == IDCANCEL) return;

		CString fn = dlg.GetPathName();
		ReadImage(fn, m_bImageSrc);

		ImageView(m_bImageSrc, m_nWidthSrc, m_nHeightSrc, 0);
//		m_msg = "图像读取完成.";

		// Filtering
		m_nWidthDst = m_Width_Dst;
		m_nHeightDst = m_Height_Dst;
		ImageResize(m_bImageSrc, m_nWidthSrc, m_nHeightSrc, m_bImageDst, m_nWidthDst, m_nHeightDst);

		ImageView(m_bImageDst, m_nWidthDst, m_nHeightDst, 2);

		if (m_fSave) {
			SaveImage(fn, imadibDst, m_nWidthDst, m_nHeightDst);
		}
//		m_msg_new = "图像处理完成.";
	}


	UpdateData(FALSE);

}




void CFpImgResizeDlg::ReadImage(CString filename, BYTE* image)
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

	m_nWidthSrc = bmpInfo.biWidth;
	m_nHeightSrc = bmpInfo.biHeight;

	DWORD friction;
	int channels;
	switch (bmpInfo.biBitCount)
	{
	case 8:
		channels = 1;
		friction = (4 - (m_nWidthSrc & 3)) & 3;
		break;
	case 24:
		channels = 3;
		friction = (4 - (m_nWidthSrc & 3)) & 3;
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
	for (int i = m_nHeightSrc - 1; i >= 0; i--) {
		for (int j = 0; j < m_nWidthSrc; j++) {
			file.Read(buffer, channels);
			if (channels == 1) image[i * m_nWidthSrc + j] = buffer[0];
			else image[i * m_nWidthSrc + j] = (BYTE)(0.11 * buffer[0] + 0.59 * buffer[1] + 0.3 * buffer[2]);
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


int CFpImgResizeDlg::SaveImage(CString fn, BYTE* bmpImageBuf, int w, int h)
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
	
	//fp = fopen((LPSTR)(LPCTSTR)filename, "wb");
	//fp = fopen(filename.GetBuffer(0), "wb");
	//if (fp) {
	//	fwrite(&fh, sizeof(fh), 1, fp);
	//	fwrite(bmpImageBuf, sizeof(BITMAPINFOHEADER) + 1024 + bmi->biHeight * nByteBound, 1, fp);
	//	fclose(fp);
	//}

	return 0;
}


int CFpImgResizeDlg::MultiResize(CString path)
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
			MultiResize(path + "\\" + subPn);
		}
		else {
			CString fn = path + "\\" + finder.GetFileName();//文件夹名称dlg.GetPathName();
			int dotPos = fn.ReverseFind('.');
			CString fileExt = fn.Right(fn.GetLength() - dotPos);
			if (fileExt != ".bmp")	continue;

			ReadImage(fn, m_bImageSrc);

			if (m_fShow)
				ImageView(m_bImageSrc, m_nWidthSrc, m_nHeightSrc, 0);
			//m_msg = "图像读取完成.";

			// Filtering
			//ImageFilterF(m_bImageOrigin);
			m_nWidthDst = m_Width_Dst;
			m_nHeightDst = m_Height_Dst;
			ImageResize(m_bImageSrc, m_nWidthSrc, m_nHeightSrc, m_bImageDst, m_nWidthDst, m_nHeightDst);

			if (m_fShow)
				ImageView(m_bImageDst, m_nWidthDst, m_nHeightDst, 2);
			//m_msg_new = "图像处理完成.";

			//	save
			if (m_fSave) {
				SaveImage(fn, imadibDst, m_nWidthDst, m_nHeightDst);
			}

		}
	}

	return 0;
}


void LAPI_IsFinger(unsigned char* image, int width, int height, int* gScore, int* aScore, int* wScore)
{
#define X_BLOCK		(10)
#define Y_BLOCK		(10)
#define GAP			(8)		//diff between rigde and valley
#define	THRES1		(15)	//exist finger?, percent of black
#define	THRES2		(90)	//wet, percent of black
#define	THRES3		(90)	//dry, gray value
#define	BAD_COFF	(0)		//weight

	int WIDTH_SUB = width / X_BLOCK;
	int HEIGHT_SUB = height / Y_BLOCK;
	int hist[256], off1, off2;

	int i, m, x, y, mx, my, c0, c1, c2;
	int area = 0, valid = 0, dry = 0, wet = 0, black, white;

	for (i = 0; i < 256; i++) hist[i] = 0;

	for (i = 0; i < width * height; i++) {
		m = image[i];
		hist[m] ++;
	}

	mx = my = 0;
	for (i = 0; i < 256; i++) {
		mx += (i * hist[i]);
		my += (1 * hist[i]);
	}
	if (my == 0)					//all black
	{
		*gScore = 0;
		*aScore = 100;
		*wScore = 100;				//0:normal, -100:dry, 100:wet 
		return;
	}
	c0 = mx / my;						//center gray value

	mx = my = 0;
	for (i = 0; i < c0; i++) {
		mx += (i * hist[i]);
		my += (1 * hist[i]);
	}
	if (my == 0) {					//all white
		c1 = 0;
		*gScore = 0;
		*aScore = 0;
		*wScore = 0;				//0:normal, -100:dry, 100:wet 
		return;
	}
	else c1 = mx / my;				//ridge gray value
	if (c1 == 0)					//almost black
	{
		*gScore = 0;
		*aScore = 100;
		*wScore = 100;
		return;
	}

	mx = my = 0;
	for (i = c0; i < 256; i++) {
		mx += (i * hist[i]);
		my += (1 * hist[i]);
	}
	if (my == 0) c2 = 255;
	else c2 = mx / my;				//valley gray value

	if ((c2 - c1) < GAP)
	{
		*gScore = 0;
		*aScore = 0;
		*wScore = 0;				//0:normal, -100:dry, 100:wet 
		return;
	}

	off1 = (c0 - c1) / 2;
	off2 = (c2 - c0) / 2;

	for (x = 0; x < X_BLOCK; x++)
	{
		for (y = 0; y < Y_BLOCK; y++)
		{
			black = 0; white = 0;
			for (mx = 0; mx < WIDTH_SUB; mx++)
			{
				for (my = 0; my < HEIGHT_SUB; my++)
				{
					m = image[(y * HEIGHT_SUB + my) * width + x * WIDTH_SUB + mx];
					if (m < (c0 - off1)) black++;						//ridge
					else if (m > (c0 + off2)) white++;					//valley or background
				}
			}
			if (black > (WIDTH_SUB * HEIGHT_SUB * THRES1 / 100))					//exist finger	
			{
				area++;
				if (black > WIDTH_SUB * HEIGHT_SUB * THRES2 / 100) wet++;			//wet
				else if (white > WIDTH_SUB * HEIGHT_SUB * THRES3 / 100) dry++;	//dry
				else valid++;
			}
		}
	}

	m = max(wet, dry);
	valid -= (BAD_COFF * m);
	valid = max(0, valid);

	*gScore = valid * 100 / (X_BLOCK * Y_BLOCK);
	*aScore = area * 100 / (X_BLOCK * Y_BLOCK);
	if (dry > wet) m = -m;
	*wScore = m * 100 / (X_BLOCK * Y_BLOCK);
}

int getImageAreaScore(uint8_t* imgBuf, int width, int height) {

#define X_BLOCK		(10)
#define Y_BLOCK		(10)
#define	OFFSET1		(20)	//for black, gray unit
#define	OFFSET2		(0)		//for white, gray unit
#define	THRES1		(10)	//exist finger?, percent of black
#define	THRES2		(95)	//wet, percent of black
#define	THRES3		(90)	//dry, gray value
#define	BAD_COFF	(0)		//weight
	int w, h;
	int hist[256];

	int i, m, x, y, mx, my, c0, c1;//, c2;
	int valid = 0, dry = 0, wet = 0, black, white, ret = 0;
	int WIDTH_SUB;
	int HEIGHT_SUB;

	w = 256;
	h = 360;

	WIDTH_SUB = w / X_BLOCK;
	HEIGHT_SUB = h / Y_BLOCK;


	for (i = 0; i < 256; i++) hist[i] = 0;

	for (i = 0; i < w * h; i++) {
		m = imgBuf[i];
		hist[m] ++;
	}

	mx = my = 0;
	for (i = 0; i < 256; i++) {
		mx += (i * hist[i]);
		my += (1 * hist[i]);
	}
	if (my == 0)					//all black
	{
		ret = 0;
		goto go_end;
	}
	c0 = mx / my;						//center gray value

	mx = my = 0;
	for (i = 0; i < c0; i++) {
		mx += (i * hist[i]);
		my += (1 * hist[i]);
	}
	if (my == 0) {					//all white
		ret = 0;
		goto go_end;
	}
	else c1 = mx / my;				//ridge gray value
	if (c1 == 0)					//almost black
	{
		ret = 0;
		goto go_end;
	}

	mx = my = 0;
	for (i = c0; i < 256; i++) {
		mx += (i * hist[i]);
		my += (1 * hist[i]);
	}
	if (my == 0)					//almost black
	{
		ret = 0;
		goto go_end;
	}
	//else c2 = mx / my;			//valley gray value

	for (x = 0; x < X_BLOCK; x++)
	{
		for (y = 0; y < Y_BLOCK; y++)
		{
			black = 0; white = 0;
			for (mx = 0; mx < WIDTH_SUB; mx++)
			{
				for (my = 0; my < HEIGHT_SUB; my++)
				{
					m = imgBuf[(y * HEIGHT_SUB + my) * w + x * WIDTH_SUB + mx];
					if (m < (c0 - OFFSET1)) black++;						//ridge
					else if (m > (c0 + OFFSET2)) white++;					//valley or background
				}
			}
			if (black > (WIDTH_SUB * HEIGHT_SUB * THRES1 / 100))					//exist finger
			{
				if (black > WIDTH_SUB * HEIGHT_SUB * THRES2 / 100) wet++;			//wet
				else if (white > WIDTH_SUB * HEIGHT_SUB * THRES3 / 100) dry++;	//dry
				else valid++;
			}
		}
	}

	m = max(wet, dry);
	valid -= (BAD_COFF * m);
	valid = max(0, valid);
	ret = valid * 100 / (X_BLOCK * Y_BLOCK);
	/*
		for (i = 0; i < 256; i ++ ) hist[i] = 0;
		cex = ((width -2*0)/2) / PBK;
		cey = ((height-2*0)/2) / PBK;

		for ( k = 0; k < 3; k ++ ) {
			p = (k + 1) * (cey / 4);
			yy = 2 * p * PBK + 0;
			for ( q = 0; q < cex; q ++ ) {
				xx = 2 * q * PBK + 0;
				for ( i = 0; i < PBK; i ++ ) {
					y = yy + 2*i;
					for ( j = 0; j < PBK; j ++ ) {
						x = xx + 2*j;
						m = imgBuf [(y + (IM_CY-height)/2)*IM_CX+(x + (IM_CX-width)/2)];
						hist[m] ++;
					}
				}
			}
		}

		m1 = m2 = 0;
		for (i = 0; i < 256; i ++ ) {
			m1 += (i*hist[i]);
			m2 += (1*hist[i]);
		}
		if (m2==0) return 0;
		m = m1/m2;
		m1 = m2 = 0;
		for (i = 0; i < 256; i ++ ) {
			n = i - m; if (n<0) n = -n;
			m1 += (n*hist[i]);
			m2 += (1*hist[i]);
		}
		ret = m1/m2;
		ret = min(100,200*ret/128);
		n = 0;
		for (i = 0; i < m; i ++ ) n += hist[i];
		ret = min(100,ret*2*n/m2);
	*/
	/*
		#define PBK			4
		int i, j, k, p, q, x, y;
		int cex, cey, m, n;
		int ret, d, xx, yy;
		int cnt[3];
		int sum;
		cex = ((IM_CX -2*0)/2) / PBK;
		cey = ((IM_CY-2*0)/2) / PBK;
		 for ( k = 0; k < 3; k ++ ) {
			p = (k + 1) * (cey / 4);
			cnt[k] = 0;
			for ( q = 0; q < cex; q ++ ) {
				xx = 2 * q * PBK + 0;
				yy = 2 * p * PBK + 0;
				m = 0;
				for ( i = 0; i < PBK; i ++ ) {
					y = yy + 2*i;
					for ( j = 0; j < PBK; j ++ ) {
						x = xx + 2*j;
						m += imgBuf [y*IM_CX+x];
					}
				}
				m /= (PBK*PBK);
				n = 0;
				for ( i = 0; i < PBK; i ++ ) {
					y = yy + 2*i;
					for ( j = 0; j < PBK; j ++ ) {
						x =  xx + 2*j;
						d = m - imgBuf [y*IM_CX+x];
						if ( d < 0 ) d = -d;
						n += d;
					}
				}
				n /= (PBK*PBK);
				if (n>16) cnt[k] ++;
			}
		}
		n = cex;
		for ( k = 0; k < 3; k ++ ) {
			if ( n > cnt[k] ) n = cnt[k];
		}
		sum = 1 * cex;
		ret = 100 * n / sum;

		if ( ret > 100 ) ret = 100;
	*/

go_end:



	return ret;// 80;//45;
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

void CFpImgResizeDlg::OnBnClickedAnalysis()
{
	// TODO: Add your control notification handler code here

	CString path;

	// TODO: 在此添加命令处理程序代码
	CFileDialog fileDlg(TRUE);
	fileDlg.m_ofn.lpstrFilter = _T("Raw Files(*.raw)\0*.raw\0Bitmap Files(*.bmp)\0*.bmp\0All Files(*.*)\0*.*\0\0");
	if (IDOK == fileDlg.DoModal())
	{
		path = fileDlg.GetPathName();
	}

	CString extention = PathFindExtension(path);

	// Read file contents
	CFile file(path, CFile::modeRead);
	int cnt = file.GetLength();
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
		image.Create(m_nWidthSrc, m_nHeightSrc, 8/*byte per pixcel*/);
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
		for (int i = 0; i < m_nHeightSrc; i++)
		{
			void* dst = image.GetPixelAddress(0, i);
			const void* src = &m_bImageSrc[i * m_nWidthSrc]/* put the pointer to the i'th source row here */;
			memcpy(dst, src, m_nWidthSrc);
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
		return;
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

	int score = getImageAreaScore((uint8_t*)m_bImageSrc, 256, 360);
	m_msgLeft.Format(_T("score:%d"), score);

	int globalScore = 0, areaScore = 0, wetScore = 0;
	LAPI_IsFinger(m_bImageSrc, m_nWidthSrc, m_nHeightSrc, &globalScore, &areaScore, &wetScore);
	m_msgLeft.Format(_T("file:%s"), path);
	m_msgRight.Format(_T("score:%d, gscore:%d, aScore = %d, wScore = %d"), score, globalScore, areaScore, wetScore);

	UpdateData(FALSE);
	//MULTI_PROCESS();

}
