#ifndef vil3d_make_edt_filter_h_
#define vil3d_make_edt_filter_h_
//:
//  \file
//  \brief Compute filter for an exp. distance transform.
//  \author Tim Cootes

#include <vil3d/algo/vil3d_structuring_element.h>

//: Compute filter for an exp. distance transform.
// width_i gives the width at half maxima (in voxels) for the
// exponential filter along i.
void vil3d_make_edt_filter(double width_i,
                           double width_j,
                           double width_k,
                           int r,
                           vil3d_structuring_element& se,
                           std::vector<double>& f);

#endif // vil3d_make_edt_filter_h_
