// ZhangLinearCalibrate.h: interface for the ZhangLinearCalibrate class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZHANGLINEARCALIBRATE_H__A6F24F23_8E39_494F_83FB_3ABF8093481D__INCLUDED_)
#define AFX_ZHANGLINEARCALIBRATE_H__A6F24F23_8E39_494F_83FB_3ABF8093481D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vcsl/vcsl_graph_sptr.h>
#include "CameraGraph.h"

class ZhangLinearCalibrate  
{
private:
	CameraGraph* camGraph_;
public:
	void setCameraGraph(CameraGraph* pG);
	ZhangLinearCalibrate();
	virtual ~ZhangLinearCalibrate();

};

#endif // !defined(AFX_ZHANGLINEARCALIBRATE_H__A6F24F23_8E39_494F_83FB_3ABF8093481D__INCLUDED_)
