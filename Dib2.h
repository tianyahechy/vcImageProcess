//======================================================================
// ���ݣ� �豸�޹�λͼ��-ͷ�ļ�
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

#pragma once

#include "afx.h"

class CDib2 : public CObject
{
public:
    // ���캯��
	CDib2(void);

    // ��������
	~CDib2(void);

    // ���ļ�����λͼ
    BOOL LoadFromFile(LPCTSTR lpszPath);

    // ��λͼ���浽�ļ�
    BOOL SaveToFile(LPCTSTR lpszPath);

    // ��ȡλͼ�ļ���
    LPCTSTR GetFileName();

    // ��ȡλͼ��С
    DWORD GetSize();

    // ��ȡλͼ���
    UINT GetWidth();

    // ��ȡλͼ�߶�
    UINT GetHeight();

    // ��ȡλͼ�Ŀ�Ⱥ͸߶�
    CSize GetDimension();

    // ��ȡ������ռλ��
    UINT GetBitCount();

    // ��ȡλͼ��ɫ��
    UINT GetNumOfColor();

    // ��ȡλͼ��ɫ��
    LPRGBQUAD GetRgbQuad();

    // ��ȡλͼ����
    LPBYTE GetData();

    // ��ȡ�Ҷ�λͼ����
    LPBYTE GetGradeData();

    // ��ȡ��ɫλͼ����
    LPBYTE GetColorData();

    // ������ɫ�����ɵ�ɫ��
    BOOL MakePalette();
      
    // ��ʾλͼ
    BOOL Draw(CDC *pDC, CPoint origin, CSize size);

    // ��ȡÿ��������ռ�ֽ���
    UINT GetLineByte();

    // ��ɫλͼת�Ҷ�λͼ
    BOOL RgbToGrade();

    // �Ҷ�λͼת��ɫλͼ
    BOOL GradeToRgb();

    // �ж��Ƿ�����ɫ��
    BOOL HasRgbQuad();

    // �ж��Ƿ��ǻҶ�ͼ
    BOOL IsGrade();

    // �ж�λͼ�Ƿ���Ч
    BOOL IsValid();  

    // ����ռ�
    void Empty();

protected:
    // ����λͼ��ɫ��
    UINT CalcNumOfColor();

private:
    // λͼ�ļ���
    char m_fileName[_MAX_PATH];

    // λͼ�ļ�ͷָ��
    LPBITMAPFILEHEADER m_lpBmpFileHeader;

    // λͼָ�루������λͼ�ļ�ͷ���������ݣ�
	LPBYTE m_lpDib; // ��Ҫ��̬������ͷ�

    // λͼ��Ϣָ��
    LPBITMAPINFO m_lpBmpInfo;

	// λͼ��Ϣͷָ��
	LPBITMAPINFOHEADER m_lpBmpInfoHeader;  

    // λͼ��ɫ��ָ��
	LPRGBQUAD m_lpRgbQuad; 

    // λͼ����ָ��
	LPBYTE m_lpData; 

    // �Ҷ�λͼ����ָ�루λͼת��ʱ�ã�
	LPBYTE m_lpGradeData; // ��Ҫ��̬������ͷ�

    // ��ɫλͼ����ָ�루λͼת��ʱ�ã�
	LPBYTE m_lpColorData; // ��Ҫ��̬������ͷ� 

    // ��ɫ����
	UINT m_uRgbQuadLength;

    // λͼ��ɫ��
    UINT m_uNumOfColor;

	// ÿ����ռ��λ��
	UINT m_uBitCount;  

    // ÿ��������ռ�ֽ�����Ϊ4�ı���
    UINT m_uLineByte;

    // �Ƿ�����ɫ��
    BOOL m_bHasRgbQuad;
	
	// ��ɫ����
	HPALETTE m_hPalette;

    // λͼ�Ƿ���Ч
    BOOL m_bValid;
};
