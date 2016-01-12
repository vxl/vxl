//PUBLIC RELEASE APPROVAL FROM AFRL
//Case Number: RY-14-0126
//PA Approval Number: 88ABW-2014-1143
#ifndef boxm2_ocl_reg_minfo_func_h_
#define boxm2_ocl_reg_minfo_func_h_
//:
// \file
// \brief A cost function for registering volumes using mutual information
// \author Vishal Jain
// \date March 9, 2012
//
#include <vnl/vnl_vector.h>
#include <vnl/vnl_cost_function.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <bocl/bocl_device.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_lru_cache2.h>
#include <boxm2/io/boxm2_stream_scene_cache.h>
#include <boxm2/reg/ocl/boxm2_ocl_reg_mutual_info.h>

//: A cost function for registering video frames by minimizing square difference in intensities.
class boxm2_ocl_reg_minfo_func2 : public boxm2_ocl_reg_mutual_info2 ,public vnl_cost_function
{
 public:
  //: Constructor. The source image is mapped to the destination frame by dt. nbins is the number of histogram bins used to compute entropies.
  boxm2_ocl_reg_minfo_func2( boxm2_opencl_cache_sptr& cacheA,
                            boxm2_stream_scene_cache& cacheB,
                            bocl_device_sptr device,
                            int nbins,
                            double scale = 1.0);

  //: The main function.
  //  Given the parameter vector x, compute the vector of residuals fx.
  //  Fx has been sized appropriately before the call.
  virtual double f(vnl_vector<double> const& x);

  // === debug purposes ===

  //: error based on absolute difference
  double error(vgl_rotation_3d<double> rot, vgl_vector_3d<double> trans);


 protected:

};


#endif // boxm2_ocl_reg_minfo_func_h_
