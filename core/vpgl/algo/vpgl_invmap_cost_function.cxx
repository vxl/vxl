#include <cmath>
#include <iostream>
#include "vpgl_invmap_cost_function.h"
//:
// \file
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include <vcl_compiler.h>

vpgl_invmap_cost_function::vpgl_invmap_cost_function(const vnl_vector_fixed<double, 2> & image_point,
                                                     const vnl_vector_fixed<double, 4> & plane,
                                                     const vpgl_camera<double> & cam)
  : vnl_cost_function(2)
  , image_point_(image_point)
  , plane_(plane)
  , cam_(cam)
  , pp_(X_Y)
{
  // determine which parameterization of the plane to use
  // order the plane normals
  double anx = std::fabs(plane_[0]), any = std::fabs(plane_[1]), anz = std::fabs(plane_[2]);
  if (anx < any && anz < any)
  {
    pp_ = X_Z;
    return;
  }
  if (any < anx && anz < anx)
    pp_ = Y_Z;
}

//: The main function.
double
vpgl_invmap_cost_function::f(const vnl_vector<double> & x)
{
  // fill out the 3-d point from the parameters
  vnl_vector_fixed<double, 3> p_3d;
  this->point_3d(vnl_vector_fixed<double, 2>(x[0], x[1]), p_3d);

  // project the current point estimate onto the image
  double u, v, X = p_3d[0], Y = p_3d[1], Z = p_3d[2];
  cam_.project(X, Y, Z, u, v);
  vnl_vector_fixed<double, 2> p_2d;
  p_2d[0] = u;
  p_2d[1] = v;
  // compute the residual
  double resid = (image_point_[0] - p_2d[0]) * (image_point_[0] - p_2d[0]);
  resid += (image_point_[1] - p_2d[1]) * (image_point_[1] - p_2d[1]);
  return resid;
}

void
vpgl_invmap_cost_function::set_params(const vnl_vector_fixed<double, 3> & xyz, vnl_vector_fixed<double, 2> & x)
{
  switch (pp_)
  {
    case X_Y:
    {
      x[0] = xyz[0];
      x[1] = xyz[1];
      break;
    }
    case X_Z:
    {
      x[0] = xyz[0];
      x[1] = xyz[2];
      break;
    }
    case Y_Z:
    {
      x[0] = xyz[1];
      x[1] = xyz[2];
      break;
    }
    default:
    {
      x[0] = 0;
      x[1] = 0;
      std::cerr << "Improper prameterization in vpgl_invmap_cost_function\n";
    }
  }
}

#if !VXL_LEGACY_FUTURE_REMOVE
VXL_DEPRECATED_MSG("Will be removed in future versions of VXL")
void
vpgl_invmap_cost_function::set_params(const vnl_vector_fixed<double, 3> & xyz, vnl_vector<double> & x)
{
  switch (pp_)
  {
    case X_Y:
    {
      x[0] = xyz[0];
      x[1] = xyz[1];
      break;
    }
    case X_Z:
    {
      x[0] = xyz[0];
      x[1] = xyz[2];
      break;
    }
    case Y_Z:
    {
      x[0] = xyz[1];
      x[1] = xyz[2];
      break;
    }
    default:
    {
      x[0] = 0;
      x[1] = 0;
      std::cerr << "Improper prameterization in vpgl_invmap_cost_function\n";
    }
  }
}
#endif

void
vpgl_invmap_cost_function::point_3d(vnl_vector_fixed<double, 2> const & x, vnl_vector_fixed<double, 3> & xyz)
{
  // Switch on plane parameterization
  switch (pp_)
  {
    case X_Y:
    {
      xyz[0] = x[0];
      xyz[1] = x[1];
      xyz[2] = -(plane_[0] * x[0] + plane_[1] * x[1] + plane_[3]) / plane_[2];
      break;
    }
    case X_Z:
    {
      xyz[0] = x[0];
      xyz[2] = x[1];
      xyz[1] = -(plane_[0] * x[0] + plane_[2] * x[1] + plane_[3]) / plane_[1];
      break;
    }
    case Y_Z:
    {
      xyz[1] = x[0];
      xyz[2] = x[1];
      xyz[0] = -(plane_[1] * x[0] + plane_[2] * x[1] + plane_[3]) / plane_[0];
      break;
    }
    default:
    {
      xyz[0] = 0;
      xyz[1] = 0;
      xyz[2] = 0;
      std::cerr << "Improper prameterization in vpgl_invmap_cost_function\n";
    }
  }
}

#if !VXL_LEGACY_FUTURE_REMOVE
VXL_DEPRECATED_MSG("Will be removed in future versions of VXL")
void
vpgl_invmap_cost_function::point_3d(const vnl_vector<double> & x, vnl_vector_fixed<double, 3> & xyz)
{
  // Switch on plane parameterization
  switch (pp_)
  {
    case X_Y:
    {
      xyz[0] = x[0];
      xyz[1] = x[1];
      xyz[2] = -(plane_[0] * x[0] + plane_[1] * x[1] + plane_[3]) / plane_[2];
      break;
    }
    case X_Z:
    {
      xyz[0] = x[0];
      xyz[2] = x[1];
      xyz[1] = -(plane_[0] * x[0] + plane_[2] * x[1] + plane_[3]) / plane_[1];
      break;
    }
    case Y_Z:
    {
      xyz[1] = x[0];
      xyz[2] = x[1];
      xyz[0] = -(plane_[1] * x[0] + plane_[2] * x[1] + plane_[3]) / plane_[0];
      break;
    }
    default:
    {
      xyz[0] = 0;
      xyz[1] = 0;
      xyz[2] = 0;
      std::cerr << "Improper prameterization in vpgl_invmap_cost_function\n";
    }
  }
}
#endif
