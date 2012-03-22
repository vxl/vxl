#include "boxm2_ocl_reg_minfo_func.h"
#include "boxm2_ocl_reg_mutual_info.h"


boxm2_ocl_reg_minfo_func::boxm2_ocl_reg_minfo_func( boxm2_opencl_cache_sptr& cacheA,
                                                    boxm2_stream_scene_cache& cacheB,
                                                    bocl_device_sptr device,
                                                    int nbins)
: vnl_cost_function(6),cacheB_(cacheB)
{
  cacheA_ = cacheA;

  device_ = device;
  nbins_  = nbins;
}

double boxm2_ocl_reg_minfo_func::f(vnl_vector<double> const& x)
{
  vgl_vector_3d<double> tx(x[0],x[1],x[2]) ;

  vgl_rotation_3d<double> r(x[3],x[4],x[5]);
  float val = 0.0f;
  boxm2_ocl_register_world(cacheA_,cacheB_,r,tx,nbins_,device_,val);
  return -val;
}

double boxm2_ocl_reg_minfo_func:: mutual_info(vgl_rotation_3d<double> rot,vgl_vector_3d<double> trans)
{
  float val = 0.0f;
  boxm2_ocl_register_world(cacheA_,cacheB_,rot,trans,nbins_,device_,val);
  return val;
}

double boxm2_ocl_reg_minfo_func:: error(vgl_rotation_3d<double> rot,vgl_vector_3d<double> trans)
{
  float val = 0.0f;
  boxm2_ocl_register_world(cacheA_,cacheB_,rot,trans,nbins_,device_,val);
  return -val;
}
