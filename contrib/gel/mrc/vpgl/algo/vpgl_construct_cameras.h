// This is gel/mrc/vpgl/algo/vpgl_construct_cameras.h
#ifndef vpgl_construct_cameras_h_
#define vpgl_construct_cameras_h_
//:
// \file
// \brief This class creates a pair of cameras from a set of corresponding points from a pair of images.
// \author Vishal Jain, (vj@lems.brown.edu)
// \date 08/31/04
//
// The first camera is assumed to be identity with supplied calibration matrix. 
// The second camera's center is unique up to a scale.
//
// \verbatim
//  Modifications
//   9/01/2006  Thomas Pollard  Converted to vpgl, changed image origin to be UL corner.
//   3/04/2007  J.L. Mundy This implementation is now deprecated, use 
//   camera extraction defined on vpgl_essential_matrix.
// \endverbatim

// not used? #include <vcl_iostream.h>
#include <vgl/vgl_point_2d.h>
#include <vcl_vector.h>
#include <vgl/vgl_point_3d.h>
#include <vnl/vnl_double_3x3.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <vpgl/vpgl_perspective_camera.h>

class vpgl_construct_cameras
{
 public:

    //: constructor
    vpgl_construct_cameras();

    //: constructor initializing the set of points
    vpgl_construct_cameras(
      vcl_vector<vgl_point_2d<double> > p0,
      vcl_vector<vgl_point_2d<double> > p1,
      const vpgl_calibration_matrix<double>* K = NULL );

    ~vpgl_construct_cameras();

    //: function to construct cameras
    // \deprecated in favour of vpgl_essential_matrix public function
    bool construct();

    //: returns the projection matrix for the cameras
    const vpgl_perspective_camera<double>& get_camera1(){return P1_;}
    const vpgl_perspective_camera<double>& get_camera2(){return P2_;}

    //: gets the world points
    void get_world_points( vcl_vector< vgl_point_3d<double> >& world_points );

 private:

    // Lifted from brct_algos, since don't want to link vsol.
    vgl_point_3d<double> triangulate_3d_point(
      const vgl_point_2d<double>& x1, 
      const vnl_double_3x4& P1,
      const vgl_point_2d<double>& x2, 
      const vnl_double_3x4& P2 );

    //: set of points
    vcl_vector<vgl_point_2d<double> > points0_;
    vcl_vector<vgl_point_2d<double> > points1_;

    vnl_double_3x3 K_;
    vnl_double_3x3 E_;
    vpgl_perspective_camera<double> P1_,P2_;
};

#endif // vpgl_construct_cameras_h_
