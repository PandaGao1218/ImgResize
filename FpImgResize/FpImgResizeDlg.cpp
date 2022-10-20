
// FpImgResizeDlg.cpp : implementation file
//

#include "pch.h"
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
	, m_fShow(FALSE)
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
}

BEGIN_MESSAGE_MAP(CFpImgResizeDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, &CFpImgResizeDlg::OnBnClickedStart)
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

	filename = fn + _T("_1.bmp");


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
