#pragma once
#include "Dib2.h"
class CHistogramDib
{
public:
	CHistogramDib( CDib2 * dib);
	~CHistogramDib();

public:
	//直方图统计
	void Histogram_Statistic(float* probability);

private:
	CDib2* theDIB;
};

