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
  _mat_cam(NULL),  _params(12)
{
  // First we set the internal camera parameters
  acs= new vcsl_spatial;
  lcs= new vcsl_spatial;
  graph = new vcsl_graph;
  acs->set_graph(graph);
  lcs->set_graph(graph);

  // Focal length is 1 unit of length
  _params[f] = 1.0;
  // Number of pixels per unit of length are 1
  _params[ku] = _params[kv] = 1.0;
  // Image coordinate reference frame is centered at the principal point
  _params[uo] = _params[vo] = 0.0;
  // Image coordinate axes are orthogonal
  _params[theta] = vnl_math::pi/2;

  // Now set the external camera parameters

  // Camera center is at origin
  _params[XL] = _params[YL] = _params[ZL] = 0.0;
  // Camera is not rotated
  _params[omega] = _params[phi] = _params[kappa] = 0.0;

  update();
}


vpgl_perspective_camera::~vpgl_perspective_camera()
{  }

//: Get parameter vector
vcl_vector<double> vpgl_perspective_camera::get_params() {return _params;}

//: Set parameter vector
void vpgl_perspective_camera::set_params(vcl_vector<double> new_params, const vcsl_spatial_sptr & )
{
  _params = new_params;
  update();
}

void vpgl_perspective_camera::update_intrinsic()
{
  double alpha_u = -_params[f]*_params[ku], alpha_v = -_params[f]*_params[kv];
  double cost = vcl_cos(_params[theta]), sint = vcl_sin(_params[theta]), cott=cost/sint;
  vnl_matrix<double> M(3, 3, 0.0);
  M(0,0) = alpha_u; M(0,1) = -alpha_u*cott; M(0,2) = _params[uo];
  M(1,1) = alpha_v/sint; M(1,2) = _params[vo];
  M(2,2) = 1.0;
  vcl_cout << "Intrinsic:\n"
           << M;
  for (int row = 0; row < 3; row++)
    for (int col = 0; col < 3; col++)
      /* double ge = */ (*_mat_cam)(row, col) = M(row, col);
}

void vpgl_perspective_camera::update() {
   vcsl_matrix_param_sptr trans_param= new vcsl_matrix_param;
   vcsl_matrix_sptr transformation = new vcsl_matrix;

  // Create a camera if it doesn't exist
  if (_mat_cam==(vpgl_matrix_camera *)NULL) _mat_cam = new vpgl_matrix_camera();

  trans_param->xl=_params[XL];
  trans_param->yl=_params[YL];
  trans_param->zl=_params[ZL];
  trans_param->omega=_params[omega];
  trans_param->phi=_params[phi];
  trans_param->kappa=_params[kappa];
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
  _mat_cam->world_to_image(x, imgu, imgv, time);
}

//-------------------------------------------------------------------------------
//: projects a 3D point to the image plane with this camera.

void vpgl_perspective_camera::world_to_image(double x, double y, double z,
                                             double& ix, double& iy, double time)
{
  vnl_double_3 temp(x,y,z);
  vnl_vector<double> transformed=acs->from_local_to_cs(temp.as_vector(),lcs,time);
  _mat_cam->world_to_image(transformed, ix, iy, time);
}

#if 0
//-------------------------------------------------------------------------
//: Project an image point as a 3-d world ray
//

void vpgl_perspective_camera::image_to_world(vnl_vector<double>& pos,
                                             vnl_vector<double>& wray,
                                             double x, double y)
{
    _mat_cam->image_to_world(pos, wray, x, y);
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

  return _params[param_index];
}

#if 0
//---------------------------------------------------------------
//: Return the current camera position.  (the camera position is
//    -Translation * R.inverse, where R is the upper 3x3 matrix, and
//    Translation is the vector at 3,0, 3,1, 3,2.

vnl_vector<double> vpgl_perspective_camera::GetPosition() const
{
  // Get the camera translation
  return vnl_vector<double> (3, _params[XL],_params[YL],_params[ZL]);
}
#endif

void vpgl_perspective_camera::print_data(vcl_ostream &strm) const
{
  strm << "f = "  << _params[f] << vcl_endl
       << "ku = " << _params[ku] << vcl_endl
       << "kv = " << _params[kv] << vcl_endl
       << "uo = " << _params[uo] << vcl_endl
       << "vo = " << _params[vo] << vcl_endl
       << "theta = " << _params[theta] << vcl_endl
       << "XL = " << _params[XL] << vcl_endl
       << "YL = " << _params[YL] << vcl_endl
       << "ZL = " << _params[ZL] << vcl_endl
       << "omega = " << _params[omega] << vcl_endl
       << "phi = " << _params[phi] << vcl_endl
       << "kappa = " << _params[kappa] << vcl_endl;
}

void vpgl_perspective_camera::set_lcs(const vcsl_spatial_sptr & new_lcs){
  lcs=new_lcs;
}

vcsl_spatial_sptr  vpgl_perspective_camera::get_lcs() {
  return lcs;
}

void vpgl_perspective_camera::set_acs(const vcsl_spatial_sptr &new_acs){
  acs= new_acs;
}

vcsl_spatial_sptr  vpgl_perspective_camera::get_acs() {
  return acs;
}
