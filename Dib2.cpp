//======================================================================
// ���ݣ� �豸�޹�λͼ��-Դ�ļ�
// ���ܣ� ��1��λͼ�ļ����뱣�棻
//        ��2���ж�λͼ�Ƿ���Ч����ȡλͼ���ļ�����
//        ��3����ȡλͼ�ĳ�������С����Ϣ��
//        ��4���ж�λͼ�Ƿ�����ɫ����ȡλͼ��ɫ��������ɫ�����ɵ�
//             ɫ�塢ʹ����ɫ����ÿ��������ռλ����ÿ��������ռλ����
//        ��5����ȡλͼ���ݣ�
//        ��6����ʾλͼ��
//        ��7������ɫλͼת���ɻҶ�λͼ��
//        ��8�����Ҷ�λͼת���ɲ�ɫλͼ��
// ���ߣ� ��ƽ��
// ��ϵ�� lipingke@126.com
// ���ڣ� 2009-7-26
//======================================================================

#include "StdAfx.h"
#include "Dib2.h"

//=======================================================
// �������ܣ� ���캯��
// ��������� ��
// ����ֵ��   ��
//=======================================================
CDib2::CDib2(void)
{
    m_lpBmpFileHeader = NULL;
    m_lpDib = NULL;   
    m_lpBmpInfo = NULL;
    m_lpBmpInfoHeader = NULL;
    m_lpRgbQuad = NULL;
    m_lpData = NULL;
    m_lpGradeData = NULL;
    m_lpColorData = NULL;
    m_hPalette = NULL;
    m_bHasRgbQuad = FALSE;
    m_bValid = FALSE;
}

//=======================================================
// �������ܣ� ��������
// ��������� ��
// ����ֵ��   ��
//=======================================================
CDib2::~CDib2(void)
{
    // ����ռ�
    Empty();
}

//=======================================================
// �������ܣ� ���ļ�����λͼ
// ��������� lpszPath-������λͼ�ļ�·��
// ����ֵ��   λͼ���ؽ����TRUE-�ɹ���FALSE-ʧ��
//=======================================================
BOOL CDib2::LoadFromFile(LPCTSTR lpszPath)
{
    // ��¼λͼ�ļ���
    strcpy(m_fileName, lpszPath);

    // �Զ�ģʽ��λͼ�ļ�
    CFile dibFile;
    if(!dibFile.Open(m_fileName, CFile::modeRead | CFile::shareDenyWrite))
    {
        return FALSE;
    }

    // ����ռ�
    Empty();

    // ��ȡλͼ�ļ�ͷ
    m_lpBmpFileHeader = (LPBITMAPFILEHEADER)new BYTE[sizeof(BITMAPFILEHEADER)];
    int nCount = dibFile.Read((void *)m_lpBmpFileHeader, sizeof(BITMAPFILEHEADER));
    if(nCount != sizeof(BITMAPFILEHEADER)) 
    {
        return FALSE;
    } 

    // �жϴ��ļ��ǲ���λͼ�ļ�����0x4d42������BM����
    if(m_lpBmpFileHeader->bfType == 0x4d42)
    {
        // ��λͼ�ļ�

        // �����λͼ�ļ�ͷ�Ŀռ��С��������ռ�
        DWORD dwDibSize = dibFile.GetLength() - sizeof(BITMAPFILEHEADER);
        m_lpDib = new BYTE[dwDibSize];

        // ��ȡ��λͼ�ļ�ͷ����������
        dibFile.Read(m_lpDib, dwDibSize);

        // �ر�λͼ�ļ�
        dibFile.Close();

        // ����λͼ��Ϣָ��
        m_lpBmpInfo = (LPBITMAPINFO)m_lpDib;

        // ����λͼ��Ϣͷָ��
        m_lpBmpInfoHeader = (LPBITMAPINFOHEADER)m_lpDib;

        // ����ÿ����ռ��λ��
        m_uBitCount = m_lpBmpInfoHeader->biBitCount;

        // ����ÿ��������ռλ��
        m_uLineByte = (GetWidth() * m_uBitCount / 8 + 3) / 4 * 4;

        // ����λͼ��ɫ��ָ��
        m_lpRgbQuad = (LPRGBQUAD)(m_lpDib + m_lpBmpInfoHeader->biSize);

        // ����λͼ��ɫ
        m_uNumOfColor = CalcNumOfColor();

        // ���λͼû������λͼʹ�õ���ɫ����������
        if(m_lpBmpInfoHeader->biClrUsed == 0)
        {
            m_lpBmpInfoHeader->biClrUsed = m_uNumOfColor;
        }

        // ������ɫ����
        m_uRgbQuadLength = m_uNumOfColor * sizeof(RGBQUAD);

        // ����λͼ����ָ��
        m_lpData = m_lpDib + m_lpBmpInfoHeader->biSize + m_uRgbQuadLength;

        // �ж��Ƿ�����ɫ��
        if(m_lpRgbQuad == (LPRGBQUAD)m_lpData)
        {
            m_lpRgbQuad = NULL;    // ��λͼ��ɫ��ָ���ÿ�
            m_bHasRgbQuad = FALSE; // ����ɫ��
        }
        else
        {
            m_bHasRgbQuad = TRUE;  // ����ɫ��
            MakePalette();         // ������ɫ�����ɵ�ɫ��
        }        

        // ����λͼ��С
        m_lpBmpInfoHeader->biSizeImage = GetSize();

        // λͼ��Ч
        m_bValid = TRUE;

        return TRUE;
    }
    else
    {
        // ����λͼ�ļ�
        m_bValid = FALSE;

        return FALSE;
    }     

}

//=======================================================
// �������ܣ� ��λͼ���浽�ļ�
// ��������� lpszPath-λͼ�ļ�����·��
// ����ֵ��   λͼ��������TRUE-�ɹ���FALSE-ʧ��
//=======================================================
BOOL CDib2::SaveToFile(LPCTSTR lpszPath)
{
    // ��¼λͼ�ļ���
    strcpy(m_fileName, lpszPath);

    // ��дģʽ���ļ�
	CFile dibFile;
	if(!dibFile.Open(lpszPath, CFile::modeCreate | CFile::modeReadWrite 
		| CFile::shareExclusive))
    {
        return FALSE;
    }

    // ��д�ļ�ͷ�ṹ
    BITMAPFILEHEADER bmpFileHeader;
    bmpFileHeader.bfType = 0x4d42; // "0x4d42" ���� "BM"
    bmpFileHeader.bfSize = 0;
    bmpFileHeader.bfReserved1 = bmpFileHeader.bfReserved2 = 0;
    bmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
                              + m_uRgbQuadLength;	

    // ���ļ�ͷ�ṹд���ļ�
    dibFile.Write(m_lpBmpFileHeader, sizeof(BITMAPFILEHEADER));

    // ���ļ���Ϣͷ�ṹд���ļ�
    dibFile.Write(m_lpBmpInfoHeader, sizeof(BITMAPINFOHEADER));

    // �������ɫ��Ļ�������ɫ��д���ļ�
    if(m_uRgbQuadLength != 0)
    {
        dibFile.Write(m_lpRgbQuad, m_uRgbQuadLength);
    }                                                        

    // ��λͼ����д���ļ�
    UINT uDataSize = (GetWidth() * m_uBitCount / 8 + 3) / 4 * 4 * GetHeight();
    dibFile.Write(m_lpData, uDataSize);

    // �ر��ļ�
    dibFile.Close();
		
    return TRUE;
}

//=======================================================
// �������ܣ� ��ȡλͼ�ļ���
// ��������� ��
// ����ֵ��   LPCTSTR λͼ�ļ���
//=======================================================
LPCTSTR CDib2::GetFileName()
{
    return m_fileName;
}

//=======================================================
// �������ܣ� ��ȡλͼ��С
// ��������� ��
// ����ֵ��   DWORD λͼ��С
//=======================================================
DWORD CDib2::GetSize()
{
    if(m_lpBmpInfoHeader->biSizeImage != 0)
    {
        return m_lpBmpInfoHeader->biSizeImage;
    }
    else
    {
        DWORD dwWidth = (DWORD)GetWidth();
        DWORD dwHeight = (DWORD)GetHeight();        
        return dwWidth * dwHeight;
    }
}


//=======================================================
// �������ܣ� ��ȡλͼ���
// ��������� ��
// ����ֵ��   UINT λͼ���
//=======================================================
UINT CDib2::GetWidth()
{
    return (UINT)m_lpBmpInfoHeader->biWidth;
}

//=======================================================
// �������ܣ� ��ȡλͼ�߶�
// ��������� ��
// ����ֵ��   UINT λͼ�߶�
//=======================================================
UINT CDib2::GetHeight()
{
    return (UINT)m_lpBmpInfoHeader->biHeight;
}

//=======================================================
// �������ܣ� ��ȡλͼ�Ŀ�Ⱥ͸߶�
// ��������� ��
// ����ֵ��   λͼ�Ŀ�Ⱥ͸߶�
//=======================================================
CSize CDib2::GetDimension()
{
    return CSize(GetWidth(), GetHeight());
}

//=======================================================
// �������ܣ� ����λͼ��ɫ��
// ��������� ��
// ����ֵ��   UINT λͼ��ɫ��
//=======================================================
UINT CDib2::CalcNumOfColor()
{
    UINT uNumOfColor;     

    if ((m_lpBmpInfoHeader->biClrUsed == 0) 
        && (m_lpBmpInfoHeader->biBitCount < 9))
	{
		switch (m_lpBmpInfoHeader->biBitCount)
		{
		    case 1: uNumOfColor = 2; break;
		    case 4: uNumOfColor = 16; break;
		    case 8: uNumOfColor = 256;
		}
	}
    else
    {
        uNumOfColor = (UINT) m_lpBmpInfoHeader->biClrUsed;
    }  		

    return uNumOfColor;

}

//=======================================================
// �������ܣ� ��ȡ������ռλ��
// ��������� ��
// ����ֵ��   ������ռλ��
//=======================================================
UINT CDib2::GetBitCount()
{
    return m_uBitCount;
}

//=======================================================
// �������ܣ� ��ȡλͼ��ɫ��
// ��������� ��
// ����ֵ��   λͼ��ɫ��
//=======================================================
UINT CDib2::GetNumOfColor()
{
    return m_uNumOfColor;

}

//=======================================================
// �������ܣ� ��ȡλͼ��ɫ��
// ��������� ��
// ����ֵ��   LPRGBQUAD λͼ��ɫ��ָ��
//=======================================================
LPRGBQUAD CDib2::GetRgbQuad()
{
    return m_lpRgbQuad;
}

//=======================================================
// �������ܣ� ��ȡλͼ����
// ��������� ��
// ����ֵ��   LPBYTE λͼ����ָ��
//=======================================================
LPBYTE CDib2::GetData()
{
    return m_lpData;
}

//=======================================================
// �������ܣ� ��ȡ�Ҷ�λͼ����
// ��������� ��
// ����ֵ��   LPBYTE �Ҷ�λͼ����ָ��
//=======================================================
LPBYTE CDib2::GetGradeData()
{
    // ������ɫ��ʱ���ǻҶ�λͼ
    if(GetRgbQuad())
    {
        m_lpGradeData = m_lpData;
    }
    return m_lpGradeData;
}

//=======================================================
// �������ܣ� ��ȡ��ɫλͼ����
// ��������� ��
// ����ֵ��   LPBYTE ��ɫλͼ����ָ��
//=======================================================
LPBYTE CDib2::GetColorData()
{
    return m_lpColorData;
}

//=======================================================
// �������ܣ� ������ɫ�����ɵ�ɫ��
// ��������� ��
// ����ֵ��   ���ɽ����TRUE-�ɹ���FALSE-ʧ��
//=======================================================
BOOL CDib2::MakePalette()
{
    // �����ɫ����Ϊ0���������߼���ɫ��
	if(m_uRgbQuadLength == 0) 
    {
        return FALSE;
    }

	//ɾ���ɵĵ�ɫ�����
	if(m_hPalette != NULL) DeleteObject(m_hPalette);

	// ���뻺�����������߼���ɫ��
	LPLOGPALETTE lpLogPalette = (LPLOGPALETTE) new BYTE[2 * sizeof(WORD) 
                                + m_uRgbQuadLength * sizeof(PALETTEENTRY)];
	lpLogPalette->palVersion = 0x300;
	lpLogPalette->palNumEntries = m_uRgbQuadLength;
	LPRGBQUAD lpRgbQuad = (LPRGBQUAD) m_lpRgbQuad;
	for(int i = 0; i < m_uRgbQuadLength; i++) {
		lpLogPalette->palPalEntry[i].peRed = lpRgbQuad->rgbRed;
		lpLogPalette->palPalEntry[i].peGreen = lpRgbQuad->rgbGreen;
		lpLogPalette->palPalEntry[i].peBlue = lpRgbQuad->rgbBlue;
		lpLogPalette->palPalEntry[i].peFlags = 0;
		lpRgbQuad++;
	}

	// �����߼���ɫ��
	m_hPalette = CreatePalette(lpLogPalette);

	// �ͷŻ�����
	delete lpLogPalette;

    return TRUE;
}

//=======================================================
// �������ܣ� ��ʾλͼ
// ���������
//            pDC-�豸����ָ��
//            origin-��ʾ������������Ͻ�
//            size-��ʾ��������ĳߴ�
// ����ֵ��
//            ��ʾ�����TRUE-�ɹ���FALSE-ʧ��
//=======================================================
BOOL CDib2::Draw(CDC *pDC, CPoint origin, CSize size)
{
    // �ɵĵ�ɫ����
	HPALETTE hOldPalette = NULL;

	// ���λͼָ��Ϊ�գ��򷵻�FALSE
	if(m_lpDib == NULL) 
    {
        return FALSE;
    }

	// ���λͼ�е�ɫ�壬��ѡ���豸������
	if(m_hPalette != NULL) 
    {
		hOldPalette = SelectPalette(pDC->GetSafeHdc(), m_hPalette, TRUE);
	}

	// ����λͼ����ģʽ
	pDC->SetStretchBltMode(COLORONCOLOR);

	// ��λͼ��pDC��ָ����豸�Ͻ�����ʾ
	StretchDIBits(pDC->GetSafeHdc(), origin.x, origin.y, size.cx, size.cy,
		0, 0, GetWidth(), GetHeight(), m_lpData, m_lpBmpInfo, DIB_RGB_COLORS, SRCCOPY);

	// �ָ��ɵĵ�ɫ��
	if(hOldPalette != NULL)
    {
        SelectPalette(pDC->GetSafeHdc(), hOldPalette, TRUE);
    }

    return TRUE;
}

//=======================================================
// �������ܣ� ��ȡÿ��������ռ�ֽ���
// ��������� ��
// ����ֵ��   ÿ��������ռ�ֽ���
//=======================================================
UINT CDib2::GetLineByte()
{
    return m_uLineByte;
}

//=======================================================
// �������ܣ� ��ɫλͼת�Ҷ�λͼ
// ��������� ��
// ����ֵ��   ת�������TRUE-�ɹ���FALSE-ʧ��
//=======================================================
BOOL CDib2::RgbToGrade()
{
    // ��ɫ�����ʱ���ǻҶ�ͼ������ת����������Ҫת��
    if(GetRgbQuad())
    {
        m_lpGradeData = m_lpData;
    }
    else
    {
        BYTE r, g, b;
        
        UINT uHeight = GetHeight();
        UINT uWidth = GetWidth();
        UINT uLineByte = GetLineByte();

        // ����ѷ���ռ䣬�ͷ�ԭ�пռ�
        if((m_lpGradeData != NULL) && (m_lpGradeData != m_lpData))
        {
            delete [] m_lpGradeData;
        }
        // ���·���ռ�
        m_lpGradeData = (LPBYTE)new BYTE[GetSize()];
        
        // ����ת��
        for(int i = 0; i < uHeight; i++)
        {
            for(int j = 0; j < uWidth; j++)
            {
                b = m_lpData[i * m_uLineByte + 3 * j];
                g = m_lpData[i * m_uLineByte + 3 * j + 1];
                r = m_lpData[i * m_uLineByte + 3 * j + 2];
                m_lpGradeData[i * uWidth + j] = (BYTE)(0.3 * r + 0.59 * g + 0.11 * b); 
            }
        }

        // ָ��ת�������λͼ����
        m_lpData = m_lpGradeData;

        // ����λͼͷ��Ϣ
        // ÿ������ռλ��
        m_lpBmpInfoHeader->biBitCount = 8;
        m_uBitCount = 8;
        // ÿ��������ռ�ֽ���
        m_uLineByte = (GetWidth() * m_uBitCount / 8 + 3) / 4 * 4;

    }

    return TRUE;   
}   

//=======================================================
// �������ܣ� �Ҷ�λͼת��ɫλͼ
// ��������� ��
// ����ֵ��   ת�������TRUE-�ɹ���FALSE-ʧ��
//=======================================================
BOOL CDib2::GradeToRgb()
{
    // ��ɫ�����ʱ�ǻҶ�ͼ����ת����������Ҫת��
    if(GetRgbQuad())
    {
        UINT uHeight = GetHeight();
        UINT uWidth = GetWidth();
        UINT uLineByte = GetLineByte();

        // ����ѷ���ռ䣬�ͷ�ԭ�пռ�
        if((m_lpColorData != NULL) && (m_lpColorData != m_lpData))
        {
            delete [] m_lpColorData;
        }
        // ���·���ռ�
        m_lpColorData = (LPBYTE)new BYTE[GetSize()*3];

        // ����ת��
        for(int i = 0; i < uHeight; i++)
        {
            for(int j = 0; j < uWidth; j++)
            {
                m_lpColorData[(uHeight - i - 1) * m_uLineByte + 3 * j] = 
                    m_lpData[(uHeight - i - 1) * uWidth + i];
                m_lpColorData[(uHeight - i - 1) * m_uLineByte + 3 * j + 1] = 
                    m_lpData[(uHeight - i - 1) * uWidth + i];
                m_lpColorData[(uHeight - i - 1) * m_uLineByte + 3 * j + 2] = 
                    m_lpData[(uHeight - i - 1) * uWidth + i];

            }
        }

        // ָ��ת�������λͼ����
        m_lpData = m_lpColorData;

        // ����λͼͷ��Ϣ
        // ÿ������ռλ��
        m_lpBmpInfoHeader->biBitCount = 24;
        m_uBitCount = 24;
        // ÿ��������ռ�ֽ���
        m_uLineByte = (GetWidth() * m_uBitCount / 8 + 3) / 4 * 4;

    }        

    return TRUE;   
}   
 
//=======================================================
// �������ܣ� �ж��Ƿ�����ɫ��
// ��������� ��
// ����ֵ��   �жϽ����TRUE-������ɫ��FALSE-������ɫ��
//=======================================================
BOOL CDib2::HasRgbQuad()
{
    return m_bHasRgbQuad;
}

//=======================================================
// �������ܣ� �ж��Ƿ��ǻҶ�ͼ
// ��������� ��
// ����ֵ��   �жϽ����TRUE-�ǻҶ�ͼ��FALSE-�ǲ�ɫͼ
//=======================================================
BOOL CDib2::IsGrade()
{
    return (m_uBitCount < 9);
}

//=======================================================
// �������ܣ� �ж�λͼ�Ƿ���Ч
// ��������� ��
// ����ֵ��   �жϽ����TRUE-λͼ��Ч��FALSE-λͼ��Ч
//=======================================================
BOOL CDib2::IsValid()
{
    return m_bValid;
}

//=======================================================
// �������ܣ� ����ռ�
// ��������� ��
// ����ֵ��   ��
//=======================================================
void CDib2::Empty()
{
    // �ͷ�λͼ�ļ�ͷָ��ռ�
    if(m_lpBmpFileHeader != NULL)
    {
        delete [] m_lpBmpFileHeader;
    }

    // �ͷŻҶ�λͼ���ݿռ�
    if((m_lpGradeData != NULL) && (m_lpGradeData != m_lpData))
    {
        delete [] m_lpGradeData;
    }

    // �ͷŲ�ɫλͼ���ݿռ�
    if((m_lpColorData != NULL) && (m_lpColorData != m_lpData))
    {
        delete [] m_lpColorData;
    }

    // �ͷ�λͼָ��ռ�
    if(m_lpDib != NULL)
    {
        delete [] m_lpDib;
        m_lpBmpInfo = NULL;
        m_lpBmpInfoHeader = NULL;
        m_lpRgbQuad = NULL;
        m_lpData = NULL;           
    }       

    // �ͷŵ�ɫ��
    if(m_hPalette != NULL)
    {
        DeleteObject(m_hPalette);
        m_hPalette = NULL;
    }    

    // ���ò�����ɫ��
    m_bHasRgbQuad = FALSE;
    
    // ����λͼ��Ч
    m_bValid = FALSE;

}  
