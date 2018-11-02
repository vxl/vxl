#include <iostream>
#include <algorithm>
#include "boxm2_trajectory.h"
//:
// \file
#include <vpgl/vpgl_perspective_camera.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/boxm2_util.h>
#include <bpgl/bpgl_camera_utils.h>
#include <cassert>

#define RAD_FACTOR (vnl_math::pi_over_180) //radians per one degree
#define AZ_STEP (vnl_math::pi_over_180) //step around the volume is 1 degree

//: initialize cameras using parametrization in a photo overlay given by a kml file
boxm2_trajectory::boxm2_trajectory(unsigned ni, unsigned nj, double right_fov, double top_fov,
                                    double alt, double heading, double tilt, double roll,
                                    double x_start, double y_start, double x_end, double y_end,
                                    double x_increment, double y_increment, double heading_increment)
{
  // Generate a regular sampling of viewpoints starting from (x_start, y_start)
  for (double x = x_start; x < x_end; x += x_increment) {
    for (double y = y_start; y < y_end; y += y_increment) {
      for (double head = heading; head < heading + 360; head += heading_increment) {
        vpgl_perspective_camera<double>* cam =
          new vpgl_perspective_camera<double>(bpgl_camera_utils::camera_from_kml((double)ni, (double)nj, right_fov, top_fov, alt, head, tilt, roll));
        cam->set_camera_center(vgl_point_3d<double>(x,y,alt));
        cams_.push_back(cam);
      }
    }
  }
  //initialize iterator for list
  iter_ = cams_.begin();
}

//: initialize cameras using parametrization in a photo overlay given by a kml file
//  use the x,y,z images to set camera centers, height of camera will be z value + alt
boxm2_trajectory::boxm2_trajectory(vgl_box_3d<double> bb, vil_image_view<float>& x_img, vil_image_view<float>& y_img, vil_image_view<float>& z_img,
                                   unsigned ni, unsigned nj, double right_fov, double top_fov, double alt, double tilt, double roll,
                                   unsigned margin, unsigned i_start, unsigned j_start, unsigned i_inc, unsigned j_inc, double heading_start, double heading_increment)
{
  unsigned img_ni = x_img.ni();
  unsigned img_nj = x_img.nj();
  assert(img_ni == y_img.ni() && img_ni == z_img.ni() && img_nj == y_img.nj() && img_nj == z_img.nj());

  assert(img_ni > margin && img_nj > margin);
  img_ni -= margin;
  img_nj -= margin;
  if (i_start < margin) i_start = margin;
  if (j_start < margin) j_start = margin;
  for (double head = heading_start; head < 360; head += heading_increment) {
    for (unsigned i = i_start; i < img_ni; i += i_inc) {
      for (unsigned j = j_start; j < img_nj; j += j_inc) {
        double x = x_img(i,j);
        double y = y_img(i,j);
        double z = z_img(i,j);
        if ( i == i_start && j == j_start)
          std::cout << "i: " << i << " j: " << j << " x: " << x << " y: " << y << " z: " << z << std::endl;
        if (bb.contains(vgl_point_3d<double>(x,y,z))) {
            vpgl_perspective_camera<double>* cam =
            new vpgl_perspective_camera<double>(
            bpgl_camera_utils::camera_from_kml((double)ni, (double)nj, right_fov, top_fov, alt, head, tilt, roll));
            cam->set_camera_center(vgl_point_3d<double>(x,y,z+alt));
            cams_.push_back(cam);
        }
      }
    }
  }

  //initialize iterator for list
  iter_ = cams_.begin();
}


//: Initializes cameras given an incline0, incline1, radius and bounding box/volume
// Remember that an incline of 0 is straight up, and pi/2 is ground level
void boxm2_trajectory::init_cameras(double incline0, double incline1, double radius, vgl_box_3d<double> bb, unsigned ni, unsigned nj)
{
  //choose a sensible radius if defualt is turned on.
  if (radius < 0.0) {
    radius = bb.width();
  }

  //generate a trajectory by varying the incline, azimuth and radius
  double currInc = incline0;
  double currRadius = radius;
  double currAz = 0.0;

  //Generate a spiraling view around the volume
  double dInc = (incline1-incline0)/360.0;
  double maxAx = currAz + 360.0;
  for ( ; currAz<maxAx; currAz++, currInc+=dInc)
  {
    vpgl_perspective_camera<double>* cam =
        boxm2_util::construct_camera(currInc, currAz, currRadius, ni, nj, bb, false);
    cams_.push_back(cam);
  }

  //now generate a smooth incline view
  //factor to smoothly slow down azimuth delta
  double slowdownAz = 1.0;
  //currAz = 0.0f;
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

  //initialize iterator for list
  iter_ = cams_.begin();
}

//: Binary write boxm2_data_base to stream
void vsl_b_write(vsl_b_ostream& os, boxm2_trajectory const& scene) {}
//: Binary write boxm2_data_base to stream
void vsl_b_write(vsl_b_ostream& os, const boxm2_trajectory* &p) {}
//: Binary write boxm2_data_base_sptr to stream
void vsl_b_write(vsl_b_ostream& os, boxm2_trajectory_sptr& sptr) {}
//: Binary write boxm2_data_base_sptr to stream
void vsl_b_write(vsl_b_ostream& os, boxm2_trajectory_sptr const& sptr) {}

//: Binary load boxm2_data_base from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_trajectory &scene) {}
//: Binary load boxm2_data_base from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_trajectory* p) {}
//: Binary load boxm2_data_base_sptr from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_trajectory_sptr& sptr) {}
//: Binary load boxm2_data_base_sptr from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_trajectory_sptr const& sptr) {}
