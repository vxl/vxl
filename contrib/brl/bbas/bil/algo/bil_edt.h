// This is bil_edt.h ATTENTION: MOVED TO vil/algo/vil_exact_distance_transform.h
// THIS IS OBSOLETE - DO NOT MODIFY THIS FILE
#ifndef bil_edt_h
#define bil_edt_h
//:
// \file
// \brief 2D Euclidean Distance Transform algorithms
// \author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
// \date March 16, 2005
//
// \todo
// Maybe define these in a class; use of inheritance may be useful e.g. by
// defining a base "distance_tranform" compute class which abstracts all EDT
// algorithms (independent of the author) and imposes uniformity to their interfaces.
// Otherwise we might end up with multiple DT implementations, each with a
// different interface and different assumptions on the input image (e.g. 0
// pixels as interest or not).

#include <iostream>
#include <vector>
#include <vil/vil_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: Euclidean Signed Distance Transform
// Regions on input image are pixels where intensity > 0;
// Distance Transform is > 0 INSIDE the regions amd
//                       < 0 OUTSIDE the regions.
bool bil_edt_signed( vil_image_view<unsigned int> &input_image,
                     vil_image_view<float> &signed_edt_image);

//: "Maurer's algorithm": Linear-time, 2D Exact Euclidean Distance Transform
//  Paper: Calvin Maurer et. al. PAMI feb. 2003
//
// Squared Distances are computed for every pixel relative to the zero-pixels.
//
// \remarks Non-contiguous images not currently supported.
//
//  Implementation influenced by the LTI lib, which is licensed under LGPL:
//    http://ltilib.sourceforge.net
//
bool bil_edt_maurer(vil_image_view<vxl_uint_32> &im);

//: "Saito's algorithm": Cubic-time, 3D (and 2d) Exact Euclidean Distance Transform
// This algorithm is empirically linear-time, but it is cubic-time in the worst
// case.
//
// Squared Distances are computed for every pixel relative to the zero-pixels.
//
// \remarks Non-contiguous images not currently supported.
//
bool bil_edt_saito(vil_image_view<vxl_uint_32> &im, unsigned plane_idx=0);
bool bil_edt_saito(vil_image_view<vxl_uint_32> &im, unsigned plane_idx, const std::vector<unsigned> &sq);
bool bil_edt_saito_3D(vil_image_view<vxl_uint_32> &im);

bool bil_edt_brute_force(vil_image_view<vxl_uint_32> &im);
bool bil_edt_brute_force_with_list(vil_image_view<vxl_uint_32> &im);


//: Row-wise 1D EDT
bool bil_edt_1d_horizontal(vil_image_view<vxl_uint_32> &im);

#endif // bil_edt_h
