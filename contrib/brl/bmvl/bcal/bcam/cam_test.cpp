#include "ZhangCameraNode.h"
#include "ZhangLinearCalibrate.h"
#include "CalibratePlane.h"
#include "CameraGraph.h"
#include <vcsl/vcsl_matrix.h>

int main()
{
	CameraGraph cg;

	// add a calibration object into the graph
	vcsl_spatial_sptr caliPlane = new CalibratePlane;
	cg.addSource(caliPlane);

	// add a camera into a graph
	int nViews = 7; 
	vcsl_spatial_sptr cam = new ZhangCameraNode(nViews);
	vcsl_spatial_transformation_sptr trans = new vcsl_matrix;
	cg.addVertex(cam, trans);

	ZhangLinearCalibrate lc;

  return 0;
}
