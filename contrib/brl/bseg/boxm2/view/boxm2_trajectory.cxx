#include "boxm2_trajectory.h"
//:
// \file
#include <vpgl/vpgl_perspective_camera.h>
#include <vcl_algorithm.h>
#include <boxm2/boxm2_util.h>

#define RAD_FACTOR (vnl_math::pi/180.0) //radians per one degree
#define AZ_STEP (vnl_math::pi/180.0) //step around the volume is 1 degree


//: Initializes cameras given an incline0, incline1, radius and bounding box/volume
// Remember that an incline of 0 is straight up, and pi/2 is ground level
void boxm2_trajectory::init_cameras(double incline0, double incline1, double radius, vgl_box_3d<double> bb, unsigned ni, unsigned nj)
{
  //choose a sensible radius if defualt is turned on. 
  if(radius < 0.0) {
    radius = bb.width(); 
  }
  
  //generate a trajectory by varying the incline, azimuth and radius
  double currInc = incline0;
  double currRadius = radius;
  double currAz = 0.0;

  //Generate a spiraling view around the volume
  double dInc = (incline1-incline0)/360.0;
  for (currAz=0.0; currAz<360.0; currAz++, currInc+=dInc)
  {
    vpgl_perspective_camera<double>* cam =
        boxm2_util::construct_camera(currInc, currAz, currRadius, ni, nj, bb, false);
    cams_.push_back(cam);
  }

  //now generate a smooth incline view
  //factor to smoothly slow down azimuth delta
  double slowdownAz = 1.0;
  currAz = 0.0f;
  for ( ;currInc>15.0; currInc-=.5, currAz += slowdownAz)
  {
    vpgl_perspective_camera<double>* cam =
        boxm2_util::construct_camera(currInc, currAz, currRadius, ni, nj, bb, false);
    cams_.push_back(cam);

    //make sure slowdown az stops at 0.0
    slowdownAz = (slowdownAz >= 0.0) ? slowdownAz-.03 : slowdownAz;
  }

  //zoom in a bit
  double min_radius = radius/2.0;
  double radius_incr = (radius-min_radius)/100.0;
  for (currRadius = radius; currRadius>min_radius; currRadius-=radius_incr)
  {
    vpgl_perspective_camera<double>* cam =
        boxm2_util::construct_camera(currInc, currAz, currRadius, ni, nj, bb, false);
    cams_.push_back(cam);

    currInc = (currInc>incline1) ? currInc-- : currInc;
    currAz++;
  }

  //zoom out a bit
  for ( ; currRadius<radius; currRadius+=radius_incr)
  {
    vpgl_perspective_camera<double>* cam =
        boxm2_util::construct_camera(currInc, currAz, currRadius, ni, nj, bb, false);
    cams_.push_back(cam);

    currInc+=.5;
  }
}
