// ZhangLinearCalibrate.cpp: implementation of the ZhangLinearCalibrate class.
//
//////////////////////////////////////////////////////////////////////

#include "ZhangLinearCalibrate.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZhangLinearCalibrate::ZhangLinearCalibrate()
{
	camGraph_ = 0;
}

ZhangLinearCalibrate::~ZhangLinearCalibrate()
{
}

void ZhangLinearCalibrate::setCameraGraph(CameraGraph *pG)
{
	camGraph_ = pG;
}
