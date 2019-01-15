#include "stdafx.h"
#include "Dib.h"
#include <atlbase.h>
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//DIB�ļ�ͷ��־(�ַ���"BM"),�����ж�λͼ�ı�־��
#define DIB_MAKER	((WORD) ( 'M' << 8) | 'B')

CDib::CDib()
{
	
	this->InitMembers();
	
}

//��ʼ����Ա����
void CDib::InitMembers()
{
	m_hDib = NULL;
	lpbmihrd = NULL;
	lpdib = NULL;
	lpDIBBits = NULL;
	dwDIBSize = 0;
	lpRgbQuad = NULL;
}

void CDib::clearMemory()
{
	if( m_hDib != NULL)
		::GlobalFree(m_hDib);
	lpbmihrd = NULL;
	lpdib = NULL;
	lpDIBBits = NULL;
	dwDIBSize = 0;
	lpRgbQuad = NULL;
}

CDib::~CDib()
{
	//����ռ�
	this->clearMemory();
}
//����DIB�е�ɫ��Ĵ�С
WORD CDib::getPalSize(LPSTR lpdib)
{
	return (WORD)(getColorNum(lpdib) * sizeof(RGBQUAD));
}
//����DIB���ص���ʼλ�ã�������ָ������ָ��
LPSTR CDib::GetBits(LPSTR)
{
	return (lpdib + ((LPBITMAPINFOHEADER)lpdib)->biSize + this->getPalSize(lpdib));
}
//���ļ�����λͼ
HGLOBAL CDib::loadFile(CFile& file)
{
	//��ȡ�ļ���С
	DWORD dwFileSize = file.GetLength();
	//��ȡλͼ�ļ�ͷ
	int nCount = file.Read((LPSTR)&bmfHeader, sizeof(bmfHeader));
	if (nCount != sizeof(bmfHeader))
	{
		return NULL;
	}

	//�жϴ��ļ��Ƿ�λͼ��0x4d42����bmP
	if (bmfHeader.bfType != DIB_MAKER)
	{
		return NULL;
	}

	//����DIB�ڴ�
	m_hDib = (HGLOBAL) ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, dwFileSize - sizeof(BITMAPFILEHEADER));
	//����ʧ�ܣ�����NULL
	if ( m_hDib == 0 )
	{
		return NULL;
	}

	//��CDib��ĳ�Ա������ֵ
	//����
	lpdib = (LPSTR) ::GlobalLock((HGLOBAL)m_hDib);
	lpbminfo = (BITMAPINFO*)lpdib;
	lpbmihrd = (BITMAPINFOHEADER*)lpdib;
	lpRgbQuad = (RGBQUAD*)(lpdib + lpbmihrd->biSize);
	int m_numberOfColors = this->getColorNum((LPSTR)lpbmihrd);
	if ( lpbmihrd->biClrUsed == 0)
	{
		lpbmihrd->biClrUsed = m_numberOfColors;
	}

	//������ɫ����
	DWORD colorTableSize = m_numberOfColors * sizeof(RGBQUAD);
	//����ָ��
	lpDIBBits = lpdib + lpbmihrd->biSize + colorTableSize;
	//��С��һ��
	if (file.Read(lpdib, dwFileSize - sizeof(BITMAPFILEHEADER)) != dwFileSize - sizeof(BITMAPFILEHEADER))
	{
		//�������
		::GlobalUnlock((HGLOBAL)m_hDib);
		//�ͷ��ڴ�
		::GlobalFree((HGLOBAL)m_hDib);
		return NULL;
	}
	//�������
	::GlobalUnlock((HGLOBAL)m_hDib);
	//����DIB���
	return m_hDib;
}

//=======================================================
// �������ܣ� ��ȡλͼ����
// ��������� ��
// ����ֵ��   LPBYTE λͼ����ָ��
//=======================================================
LPBYTE CDib::GetData()
{
	return (LPBYTE)lpDIBBits;
}
//��λͼ���浽�ļ�
BOOL CDib::saveFile(HGLOBAL hDib, CFile& file)
{
	//���DIBΪ�գ�����FALSE
	if ( hDib == NULL )
	{
		return FALSE;
	}

	//��ȡBITMAPINFO�ṹ��������
	lpbmihrd = (LPBITMAPINFOHEADER) ::GlobalLock((HGLOBAL)hDib);
	if ( lpbmihrd == NULL)
	{
		return FALSE;
	}
	//����ļ�ͷ
	bmfHeader.bfType = DIB_MAKER;
	//�ļ�ͷ��С+��ɫ���С
	dwDIBSize = *(LPDWORD)lpbmihrd + getPalSize((LPSTR)lpbmihrd);
	//���صĴ�С,��СΪwidth * height
	DWORD dwBmBitsSize = this->getReqByteWidth((lpbmihrd->biWidth) * ((DWORD)lpbmihrd->biBitCount)) *lpbmihrd->biHeight;
	//�����DIBÿ���ֽ���Ϊ4�ı���ʱ�Ĵ�С
	dwDIBSize += dwBmBitsSize;
	//����biSizeImage
	lpbmihrd->biSizeImage = dwBmBitsSize;
	//�ļ���С
	bmfHeader.bfSize = dwDIBSize + sizeof(BITMAPFILEHEADER);
	bmfHeader.bfReserved1 = 0;
	bmfHeader.bfReserved2 = 0;
	//����ƫ����
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + lpbmihrd->biSize + getPalSize((LPSTR)lpbmihrd);
	TRY
	{
		//д�ļ�ͷ
		file.Write(&bmfHeader, sizeof(BITMAPFILEHEADER));
		//дDIBͷ������
		file.Write(lpbmihrd, dwDIBSize);
	}
	CATCH(CFileException, e)
	{
		//�������
		::GlobalUnlock((HGLOBAL)hDib);
		//�׳��쳣
		THROW_LAST();
	}
	END_CATCH
		//�������
		::GlobalUnlock((HGLOBAL)hDib);
	return TRUE;

}   

//��ȡλͼ�߶�
DWORD CDib::getHeight(LPSTR lpdib)
{
	return ((LPBITMAPINFOHEADER)lpdib)->biHeight;
}

//���λͼ�Ŀ��
DWORD CDib::getWidth(LPSTR lpdib)
{
	return ((LPBITMAPINFOHEADER)lpdib)->biWidth;
}

//��ȡ����������ռλ��
WORD CDib::getBitCount()
{
	return ((LPBITMAPINFOHEADER)lpdib)->biBitCount;
}

//��ȡ��Ҫ�����ֽ�����ӦΪ4�ı���
int CDib::getReqByteWidth(int bits)
{
	int getBytes = (bits + 31) / 32 * 4;
	return getBytes;
}

long CDib::getRectWidth(LPRECT lpRect)
{
	long nWidth = lpRect->right - lpRect->left;
	return nWidth;
}

long CDib::getRectHeight(LPRECT lpRect)
{
	long nHeight = lpRect->bottom - lpRect->top;
	return nHeight;
}

//��ȡλͼ��ɫ��
WORD CDib::getColorNum(LPSTR lpdib)
{
	long dwClrUsed = ((LPBITMAPINFOHEADER)lpdib)->biClrUsed;
	if (dwClrUsed != 0)
	{
		return (WORD)dwClrUsed;
	}
	//��ȡbiBitCountֵ
	WORD wBitCount = ((LPBITMAPINFOHEADER)lpdib)->biBitCount;
	
	switch (wBitCount)
	{
	case 1:
		return 2;	

	case 4:
		return 16;

	case 8:
		return 256;

	default:
		return 0;
	}

}

//������ɫ�����ɵ�ɫ��
BOOL CDib::ConstructPalette( HGLOBAL hDIB,		//DIB����ľ��
	CPalette * pPal								//��ɫ��ָ��
	)
{
	//�������
	BOOL bSuccess = FALSE;
	//�ж��Ƿ�����Ч��DIB����
	if (hDIB == NULL)
	{
		return FALSE;
	}
	//����DIB
	lpdib = (LPSTR) ::GlobalLock((HGLOBAL)hDIB);
	lpbminfo = (LPBITMAPINFO)lpdib;
	//��ȡDIB����ɫ�����ɫ��Ŀ
	long wNumColors = this->getColorNum(lpdib);
	//�����ɫ����Ϊ0���������߼���ɫ��
	if (wNumColors == 0)
	{
		return FALSE;
	}
	
	//����Ϊ�߼���ɫ���ڴ�
	//�߼���ɫ����
	HANDLE hLogPal = ::GlobalAlloc(GHND, sizeof(LOGPALETTE) + sizeof(PALETTEENTRY) * wNumColors);
	//���ʧ�����˳�
	if ( hLogPal == 0 )
	{
		//�������
		::GlobalUnlock((HGLOBAL)hDIB);
		return FALSE;
	}
	LPLOGPALETTE lpLogPalette = (LPLOGPALETTE) ::GlobalLock((HGLOBAL)hLogPal);
	
	//�����߼���ɫ��
	//���õ�ɫ��汾��
	lpLogPalette->palVersion = PalVersion;
	//������ɫ��Ŀ
	lpLogPalette->palNumEntries = (WORD)wNumColors;
	//��ȡ��ɫ��
	for (size_t i = 0; i < (int)wNumColors; i++)
	{
		lpLogPalette->palPalEntry[i].peRed = lpbminfo->bmiColors[i].rgbRed;
		lpLogPalette->palPalEntry[i].peGreen = lpbminfo->bmiColors[i].rgbGreen;
		lpLogPalette->palPalEntry[i].peBlue = lpbminfo->bmiColors[i].rgbBlue;
		lpLogPalette->palPalEntry[i].peFlags = 0; //����λ
		
	}
	//�����߼���ɫ��
	bSuccess = pPal->CreatePalette(lpLogPalette);

	//�������
	::GlobalUnlock((HGLOBAL)hLogPal);
	//�ͷ��߼���ɫ��
	::GlobalFree((HGLOBAL)hLogPal);	
	//�������
	::GlobalUnlock((HGLOBAL)hDIB);
	return bSuccess;
}

//��ʾλͼ
BOOL CDib::DrawDib( HDC		hDC,			//����豸DC
					LPRECT	lpDCRect,		//���ƾ�������
					HGLOBAL	hDIB,			//DIB����ľ��
					LPRECT	lpDIBRect,		//DIB���������
					CPalette* pPal			//��ɫ��ָ��
	)
{
	//�ػ��ɹ���־
	BOOL	bSuccess = FALSE;
	//�ɵĵ�ɫ����
	HPALETTE hOldPalette = NULL;
	//�ж��Ƿ�����Ч��DIB����,�����򷵻�
	if ( hDIB == NULL)
	{
		return FALSE;
	}
	//����DIB
	lpdib = (LPSTR) ::GlobalLock((HGLOBAL)hDIB);
	//�ҵ�DIBͼ��������ʼλ��
	lpDIBBits = GetBits(lpdib);
	//���λͼ�е�ɫ��,��ѡ���豸������
	if (pPal != NULL)
	{
		HPALETTE hPal = (HPALETTE)pPal->m_hObject;
		hOldPalette = SelectPalette(hDC, hPal, TRUE);
	}
	//������ʾģʽ
	::SetStretchBltMode(hDC,COLORONCOLOR);

	//��λͼ��pDC��ָ����豸�Ͻ�����ʾ
	bSuccess = ::StretchDIBits(hDC,			//�豸�������
		lpDCRect->left,						//Ŀ��X����
		lpDCRect->top,						//Ŀ��Y����
		this->getRectWidth(lpDCRect),		//Ŀ����
		this->getRectHeight(lpDCRect),		//Ŀ��߶�
		lpDIBRect->left,					//ԴX���� 
		lpDIBRect->top,						//ԴY����
		this->getRectWidth(lpDIBRect),		//Դ���
		this->getRectHeight(lpDIBRect),		//Դ�߶�
		lpDIBBits,							//ָ��dib���ص�ָ��
		(LPBITMAPINFO) lpdib,				//ָ��λͼ��Ϣ�ṹ��ָ��
		DIB_RGB_COLORS,						//ʹ�õ���ɫ��Ŀ
		SRCCOPY);
	::GlobalUnlock(hDIB);					//�������
	//�ָ�ϵͳ��ɫ��
	if ( hOldPalette != NULL )
	{
		::SelectPalette(hDC, hOldPalette, TRUE);
	}
	return bSuccess;
}

//����ָ�����ڴ�����
HGLOBAL CDib::copyObject(HGLOBAL hGlob)
{
	if ( hGlob == NULL)
	{
		return NULL;
	}

	//��ȡָ���ڴ������С
	DWORD dwLen = ::GlobalSize((HGLOBAL)hGlob);
	//�������ڴ�ռ�
	HGLOBAL hTemp = ::GlobalAlloc(GHND, dwLen);
	//�жϷ����Ƿ�ɹ�
	if ( hTemp != NULL )
	{
		//����
		void * lpTemp = ::GlobalLock((HGLOBAL)hTemp);
		void * lp = ::GlobalLock((HGLOBAL)hGlob);
		//����
		memcpy(lpTemp, lp, dwLen);
		//�������
		::GlobalUnlock(hTemp);
		::GlobalUnlock(hGlob);
	}
	return hTemp;
}