#pragma once
#include "afx.h"

//��ɫ��汾
#define PalVersion	0x300

class CDib :
	public CObject
{
public:
	CDib();
	virtual ~CDib();
	BOOL DrawDib(HDC, LPRECT, HGLOBAL, LPRECT, CPalette*);	//��ʾλͼ
	BOOL ConstructPalette(HGLOBAL, CPalette*);				//�����߼���ɫ��
	LPSTR GetBits(LPSTR);									//��ȡλͼ���ݵ���ڵ�ַ
	DWORD getWidth(LPSTR);									//���λͼ���
	DWORD getHeight(LPSTR);									//���λͼ�߶�
	WORD  getPalSize(LPSTR);								//ȡ�õ�ɫ��Ĵ�С
	WORD  getColorNum(LPSTR);								//��ȡλͼ��ɫ��
	WORD  getBitCount();									//��ȡ����������ռλ��
	HGLOBAL copyObject(HGLOBAL);							//���ڸ���λͼ����
	BOOL saveFile(HGLOBAL, CFile&);							//��λͼ���浽�ļ�
	HGLOBAL loadFile(CFile& file);							//���ļ�����λͼ
	//ת��λͼ�ֽ�Ϊ4�ı����Խ���ͼ����
	int getReqByteWidth(int);
	long  getRectWidth(LPRECT);								//��ȡ����Ŀ��
	long  getRectHeight(LPRECT);							//��ȡ����ĸ߶�
    // ��ȡλͼ����
	LPBYTE GetData();
public:
	void clearMemory();
	void InitMembers();

public:

private:
	LPBITMAPINFO		lpbminfo;			//λͼ��Ϣָ��
	LPBITMAPINFOHEADER	lpbmihrd;			//λͼ��Ϣͷָ��
	BITMAPFILEHEADER	bmfHeader;			//λͼ�ļ�ͷָ�룬�趯̬������ͷ�
	LPSTR				lpdib;				//λͼָ�룬������λͼ�ļ�ͷ���������ݣ��趯̬������ͷ�
	LPSTR				lpDIBBits;			//λͼ����ָ��
	DWORD				dwDIBSize;			//DIB��С
	HGLOBAL				m_hDib;				//DIB����ľ��
	RGBQUAD	*			lpRgbQuad;			//λͼ��ɫ��ָ��
	
};

