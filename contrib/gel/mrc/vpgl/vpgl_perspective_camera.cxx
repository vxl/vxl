// This is gel/mrc/vpgl/vpgl_perspective_camera.cxx
#include "vpgl_perspective_camera.h"
//:
// \file

#include <vcsl/vcsl_graph.h>
#include <vcsl/vcsl_matrix.h>
#include <vcsl/vcsl_spatial.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_double_3.h>

vpgl_perspective_camera::vpgl_perspective_camera():
  mat_cam_(NULL),  params_(12)
{
  // First we set the internal camera parameters
  acs= new vcsl_spatial;
  lcs= new vcsl_spatial;
  graph = new vcsl_graph;
  acs->set_graph(graph);
  lcs->set_graph(graph);

  // Focal length is 1 unit of length
  params_[f] = 1.0;
  // Number of pixels per unit of length are 1
  params_[ku] = params_[kv] = 1.0;
  // Image coordinate reference frame is centered at the principal point
  params_[uo] = params_[vo] = 0.0;
  // Image coordinate axes are orthogonal
  params_[theta] = vnl_math::pi/2;

  // Now set the external camera parameters

  // Camera center is at origin
  params_[XL] = params_[YL] = params_[ZL] = 0.0;
  // Camera is not rotated
  params_[omega] = params_[phi] = params_[kappa] = 0.0;

  update();
}


vpgl_perspective_camera::~vpgl_perspective_camera()
{  }

//: Get parameter vector
vcl_vector<double> vpgl_perspective_camera::get_params() {return params_;}

//: Set parameter vector
void vpgl_perspective_camera::set_params(vcl_vector<double> new_params, const vcsl_spatial_sptr & )
{
  params_ = new_params;
  update();
}

void vpgl_perspective_camera::update_intrinsic()
{
  double alpha_u = -params_[f]*params_[ku], alpha_v = -params_[f]*params_[kv];
  double cost = vcl_cos(params_[theta]), sint = vcl_sin(params_[theta]), cott=cost/sint;
  vnl_matrix<double> M(3, 3, 0.0);
  M(0,0) = alpha_u; M(0,1) = -alpha_u*cott; M(0,2) = params_[uo];
  M(1,1) = alpha_v/sint; M(1,2) = params_[vo];
  M(2,2) = 1.0;
  vcl_cout << "Intrinsic:\n"
           << M;
  for (int row = 0; row < 3; row++)
    for (int col = 0; col < 3; col++)
      /* double ge = */ (*mat_cam_)(row, col) = M(row, col);
}

void vpgl_perspective_camera::update()
{
   vcsl_matrix_param_sptr trans_param= new vcsl_matrix_param;
   vcsl_matrix_sptr transformation = new vcsl_matrix;

  // Create a camera if it doesn't exist
  if (!mat_cam_)
    mat_cam_ = new vpgl_matrix_camera();

  trans_param->xl=params_[XL];
  trans_param->yl=params_[YL];
  trans_param->zl=params_[ZL];
  trans_param->omega=params_[omega];
  trans_param->phi=params_[phi];
  trans_param->kappa=params_[kappa];
  transformation->set_static(trans_param);
  lcs->set_unique(acs,transformation.ptr());

  update_intrinsic();
}

//------------------------------------------------------------------------------
//: projects a 3D point to the image plane with this camera.

void vpgl_perspective_camera::world_to_image(vnl_vector<double> const& vect3d,
                                             double& imgu, double& imgv,
                                             double time)
{
  vnl_vector<double> x = acs->from_local_to_cs(vect3d,lcs,time);
  mat_cam_->world_to_image(x, imgu, imgv, time);
}

//-------------------------------------------------------------------------------
//: projects a 3D point to the image plane with this camera.

void vpgl_perspective_camera::world_to_image(double x, double y, double z,
                                             double& ix, double& iy, double time)
{
  vnl_double_3 temp(x,y,z);
  vnl_vector<double> transformed=acs->from_local_to_cs(temp.as_vector(),lcs,time);
  mat_cam_->world_to_image(transformed, ix, iy, time);
}

#if 0
//-------------------------------------------------------------------------
//: Project an image point as a 3-d world ray
//

void vpgl_perspective_camera::image_to_world(vnl_vector<double>& pos,
                                             vnl_vector<double>& wray,
                                             double x, double y)
{
    mat_cam_->image_to_world(pos, wray, x, y);
}
#endif


//: Set camera parameters
double& vpgl_perspective_camera::operator() (PerspParams param_index)
{
  if ((param_index >=12) || (param_index < 0))
  {
    vcl_cerr << "vpgl_perspective_camera:  Parameter indices out of bound.\n"
             << "Program may segfault now.\n";
  }

  return params_[param_index];
}

#if 0
//---------------------------------------------------------------
//: Return the current camera position.  (the camera position is
//    -Translation * R.inverse, where R is the upper 3x3 matrix, and
//    Translation is the vector at 3,0, 3,1, 3,2.

vnl_vector<double> vpgl_perspective_camera::GetPosition() const
{
  // Get the camera translation
  return vnl_vector<double> (3, params_[XL],params_[YL],params_[ZL]);
}
#endif

void vpgl_perspective_camera::print_data(vcl_ostream &strm) const
{
  strm << "f = "  << params_[f] << vcl_endl
       << "ku = " << params_[ku] << vcl_endl
       << "kv = " << params_[kv] << vcl_endl
       << "uo = " << params_[uo] << vcl_endl
       << "vo = " << params_[vo] << vcl_endl
       << "theta = " << params_[theta] << vcl_endl
       << "XL = " << params_[XL] << vcl_endl
       << "YL = " << params_[YL] << vcl_endl
       << "ZL = " << params_[ZL] << vcl_endl
       << "omega = " << params_[omega] << vcl_endl
       << "phi = " << params_[phi] << vcl_endl
       << "kappa = " << params_[kappa] << vcl_endl;
}

void vpgl_perspective_camera::set_lcs(const vcsl_spatial_sptr & new_lcs)
{
  lcs=new_lcs;
}

vcsl_spatial_sptr  vpgl_perspective_camera::get_lcs()
{
  return lcs;
}

void vpgl_perspective_camera::set_acs(const vcsl_spatial_sptr &new_acs)
{
  acs= new_acs;
}

vcsl_spatial_sptr  vpgl_perspective_camera::get_acs()
{
  return acs;
}
