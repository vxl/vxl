//:
// \file
//  \brief Compute filter for an exp. distance transform.
// \author Tim Cootes

#include <iostream>
#include <cmath>
#include "vil3d_make_edt_filter.h"
#include "vil3d_make_distance_filter.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Compute filter for an exp. distance transform.
// width_i gives the width at half maxima (in voxels) for the
// exponential filter along i.
void vil3d_make_edt_filter(double width_i,
                           double width_j,
                           double width_k,
                           int r,
                           vil3d_structuring_element& se,
                           std::vector<double>& f)
{
  vil3d_make_distance_filter(1.0/width_i,1.0/width_j,1.0/width_k,r,se,f);
  for (double & a : f)
    a=std::exp(std::log(0.5)*a);
}
