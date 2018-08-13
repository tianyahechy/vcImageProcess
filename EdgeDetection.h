#pragma once
#include "Dib2.h"
class CEdgeDetection
{
public:
	CEdgeDetection(CDib2 * pDib);
	~CEdgeDetection();
public:
	void Roberts();

private:
	CDib2 * m_pDib;
};

