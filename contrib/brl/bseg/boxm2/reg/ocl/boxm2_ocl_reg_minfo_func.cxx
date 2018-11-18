#include "boxm2_ocl_reg_minfo_func2.h"
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boct/boct_bit_tree.h>


boxm2_ocl_reg_minfo_func2::boxm2_ocl_reg_minfo_func2( boxm2_opencl_cache_sptr& cacheA,
                                                    boxm2_stream_scene_cache& cacheB,
                                                    bocl_device_sptr device,
                                                    int nbins,
                                                    double scale)
: vnl_cost_function(6), boxm2_ocl_reg_mutual_info2(cacheA,cacheB,device,nbins, scale)
{

}

double boxm2_ocl_reg_minfo_func2::f(vnl_vector<double> const& x)
{
  vgl_vector_3d<double>   tx(x[0],x[1],x[2]);
  vgl_rotation_3d<double> r (x[3],x[4],x[5]);
  return -this->mutual_info(r,tx);
}

double boxm2_ocl_reg_minfo_func2:: error(vgl_rotation_3d<double> rot,vgl_vector_3d<double> trans)
{
    return (-this->mutual_info(rot,trans));
}
