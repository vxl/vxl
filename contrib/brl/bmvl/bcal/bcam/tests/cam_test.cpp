#include <bmvl/bcal/bcam/ZhangCameraNode.h>
#include <bmvl/bcal/bcam/ZhangLinearCalibrate.h>
#include <bmvl/bcal/bcam/CalibratePlane.h>
#include <bmvl/bcal/bcam/CameraGraph.h>
#include <vcsl/vcsl_matrix.h>

int main()
{
  CameraGraph cg;

  // add a calibration object into the graph
  vcsl_spatial_sptr caliPlane = new CalibratePlane;
  ((CalibratePlane*)caliPlane.ptr())->readData("Model.txt");
  cg.addSource(caliPlane);

  // add a camera into a graph
  int nViews = 7, iCamID = 0;
  vcsl_spatial_sptr cam = new ZhangCameraNode(iCamID, nViews);
  vcsl_spatial_transformation_sptr trans = new vcsl_matrix;
  ((ZhangCameraNode*)cam.ptr())->readData("data1.txt" , 0);
  ((ZhangCameraNode*)cam.ptr())->readData("data2.txt" , 1);
  ((ZhangCameraNode*)cam.ptr())->readData("data3.txt" , 2);
  ((ZhangCameraNode*)cam.ptr())->readData("data4.txt" , 3);
  ((ZhangCameraNode*)cam.ptr())->readData("data5.txt" , 4);

  cg.addVertex(cam, trans);

  // do the calibration
  ZhangLinearCalibrate lc;

  return 0;
}

