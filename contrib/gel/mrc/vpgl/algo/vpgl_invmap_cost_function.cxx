#include <vpgl/algo/vpgl_invmap_cost_function.h>
//:
// \file
#include <vcl_cmath.h>
#include <vcl_iostream.h>

vpgl_invmap_cost_function::
vpgl_invmap_cost_function(vnl_vector_fixed<double, 2> const& image_point,
                          vnl_vector_fixed<double, 4> const& plane,
                          const vpgl_rational_camera<double>* const rcam)
 : vnl_cost_function(2), image_point_(image_point),
   plane_(plane), rcam_ptr_(rcam), pp_(X_Y)
{
  //determine which parameterization of the plane to use
  // order the plane normals
  double anx = vcl_fabs(plane_[0]), any = vcl_fabs(plane_[1]),
    anz = vcl_fabs(plane_[2]);
  if (anx<any && anz<any)
    { pp_ = X_Z; return;}
  if (any<anx && anz<anx)
     pp_ = Y_Z;
}
//: The main function.
double vpgl_invmap_cost_function::f(vnl_vector<double> const& x)
{
  if (!rcam_ptr_)
    return 0;
  // fill out the 3-d point from the parameters
  vnl_vector_fixed<double, 3> p_3d;
  this->point_3d(x, p_3d);

  // project the current point estimate onto the image
  vnl_vector_fixed<double, 2> p_2d = rcam_ptr_->project(p_3d);

  // compute the residual
  double resid = (image_point_[0] - p_2d[0])*(image_point_[0] - p_2d[0]);
  resid += (image_point_[1] - p_2d[1])*(image_point_[1] - p_2d[1]);
  return resid;
}

void vpgl_invmap_cost_function::
set_params(vnl_vector_fixed<double, 3> const& xyz, vnl_vector<double> &x)
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
      x[0] = 0; x[1] = 0;
      vcl_cerr << "Improper prameterization in vpgl_invmap_cost_function\n";
    }
  }
}

void vpgl_invmap_cost_function::
point_3d(vnl_vector<double> const& x, vnl_vector_fixed<double, 3>& xyz)
{
  //Switch on plane parameterization
  switch (pp_)
  {
    case X_Y:
    {
      xyz[0] = x[0];
      xyz[1] = x[1];
      xyz[2] = -(plane_[0]*x[0] + plane_[1]*x[1] + plane_[3])/plane_[2];
      break;
    }
    case X_Z:
    {
      xyz[0] = x[0];
      xyz[2] = x[1];
      xyz[1] = -(plane_[0]*x[0] + plane_[2]*x[1] + plane_[3])/plane_[1];
      break;
    }
    case Y_Z:
    {
      xyz[1] = x[0];
      xyz[2] = x[1];
      xyz[0] = -(plane_[1]*x[0] + plane_[2]*x[1] + plane_[3])/plane_[0];
      break;
    }
    default:
    {
      xyz[0] = 0; xyz[1] = 0; xyz[2] = 0;
      vcl_cerr << "Improper prameterization in vpgl_invmap_cost_function\n";
    }
  }
}

