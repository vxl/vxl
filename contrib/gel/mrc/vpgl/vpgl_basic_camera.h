#ifndef VPGL_BASIC_CAMERA_H
#define VPGL_BASIC_CAMERA_H
//:
//  \file
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

  //: A function to set the initilization point
  virtual void set_init_pt(const vnl_vector<double>& pt);
  //: A function to get the initilization point
  virtual void get_init_pt(vnl_vector<double>& pt);

  virtual void get_matrix(vnl_matrix<double>&) const;

#if 0
  inline friend vcl_ostream &operator<<(vcl_ostream &os, const vpgl_basic_camera& obj)
    {obj.Print(os); return os;}
  inline friend vcl_ostream &operator<<(vcl_ostream &os, const vpgl_basic_camera* obj)
    {if (obj) obj->Print(os); else os << "NULL Camera"; return os;}
#endif

protected:

  //: Holds a 3D point used for Levenberg Marquardt initialization in ImageToSurface
  vnl_vector<double> init_pt;
};

//: fill passed in matrix with camera matrix.
// this only is implemented when MatrixProjection returns true.
inline void vpgl_basic_camera::get_matrix(vnl_matrix<double>&) const
{
  vcl_cerr<<"Nothing returned";
}

#endif   // VPGL_BASIC_CAMERA_H
