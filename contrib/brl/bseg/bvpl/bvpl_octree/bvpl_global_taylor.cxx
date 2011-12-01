//:
// \file
// \author Isabel Restrepo
// \date 11-Apr-2011

#include "bvpl_global_taylor.h"

template <>
boxm_apm_type bvpl_taylor_apm<double, 10>(){return VNL_DOUBLE_10;}

template <>
boxm_apm_type bvpl_taylor_apm<float, 3>(){return VNL_FLOAT_3;}
