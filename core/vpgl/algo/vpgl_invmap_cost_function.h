// This is core/vpgl/algo/vpgl_invmap_cost_function.h
#ifndef vpgl_invmap_cost_function_h_
#define vpgl_invmap_cost_function_h_
//:
// \file
#include <vnl/vnl_cost_function.h>
#include <vnl/vnl_vector_fixed.h>
#include <vpgl/vpgl_camera.h>

class vpgl_invmap_cost_function: public vnl_cost_function
{
  //: Which parameterization to use for the plane
  enum plane_param{X_Y=0, X_Z, Y_Z};
 public:
  //: Constructor - rcam pointer is not deleted by this class
  vpgl_invmap_cost_function(vnl_vector_fixed<double, 2> const& image_point,
                            vnl_vector_fixed<double, 4> const& plane,
                            const vpgl_camera<double>* rcam);
  ~vpgl_invmap_cost_function() override = default;
  //: The cost function. x is a vector holding the two plane parameters
  double f(vnl_vector<double> const& x) override;
  //: set the parameter values from the 3-d point
  void set_params(vnl_vector_fixed<double,3> const& xyz, vnl_vector_fixed<double,2> &x);
  //: set the parameter values from the 3-d point
  //  (Deprecated interface)
  void set_params(vnl_vector_fixed<double,3> const& xyz, vnl_vector<double> &x);
  //: get the 3-d point defined by the parameters (and the plane).
  void point_3d(vnl_vector_fixed<double,2> const& x, vnl_vector_fixed<double, 3>& xyz);
  //: get the 3-d point defined by the parameters (and the plane).
  //  (Deprecated interface)
  void point_3d(vnl_vector<double> const& x, vnl_vector_fixed<double, 3>& xyz);

 protected:
  //: image point
  vnl_vector_fixed<double, 2> image_point_;
  //: plane coefficients
  vnl_vector_fixed<double, 4> plane_;
  //: rational camera
  const vpgl_camera<double>* cam_ptr_;
  //: the well-conditioned parameterization
  plane_param pp_;
};

#endif // vpgl_invmap_cost_function_h_
