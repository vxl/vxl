#ifndef VPGL_BASIC_CAMERA_H
#define VPGL_BASIC_CAMERA_H
//:
// \file
// \brief Base class for cameras
//
// The code is taken and adapted from BasicCamera.h of TargetJr for VxL photogrammetry library.
//
// \author
//     Patti Vrobel
//
//-----------------------------------------------------------------------------

#include <vcl_iostream.h>
#include <vbl/vbl_ref_count.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vpgl/vpgl_basic_camera_sptr.h>

class vpgl_graphics_camera;
class vpgl_basic_camera;
class vpgl_matrix_camera;


class vpgl_basic_camera : public vbl_ref_count
{
 public:
 // Constructors
  vpgl_basic_camera();
  // copy constructor - compiler-provided one sets ref_count to nonzero which is wrong -PVr
  vpgl_basic_camera(vpgl_basic_camera const& c) : vbl_ref_count(), init_pt(c.init_pt) {}
 // Destructors
  virtual ~vpgl_basic_camera();

  // virtual interface for camera classes

  //---------------------------------------
  // Different world-to-image methods
  virtual void world_to_image(vnl_vector<double> const& world_3d_pt,
                              double& image_u, double& image_v, double time);

  virtual void world_to_image(double world_x, double world_y, double world_z,
                              double& image_u, double& image_v, double time);

  //-----------------------------------------
  virtual void image_to_world(vnl_vector<double>& ray3d_origin,
                              vnl_vector<double>& world, double u, double v);

  //: A function to set the initialization point
  virtual void set_init_pt(vnl_vector<double> const& pt);
  //: A function to get the initialization point
  virtual void get_init_pt(vnl_vector<double>& pt);

  //: fill passed in matrix with camera matrix.
  // this is only implemented when MatrixProjection returns true.
  virtual void get_matrix(vnl_matrix<double>&) const = 0;

 protected:
  //: Holds a 3D point used for Levenberg Marquardt initialization in ImageToSurface
  vnl_vector<double> init_pt;
};

#endif // VPGL_BASIC_CAMERA_H
