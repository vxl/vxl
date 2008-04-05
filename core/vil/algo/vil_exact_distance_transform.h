// This is core/vil/algo/vil_exact_distance_transform.h
#ifndef vil_exact_distance_transform_h
#define vil_exact_distance_transform_h
//:
// \file
// \brief Euclidean Distance Transform algorithms
// \author Ricardo Fabbri (Brown)
// \date 03/16/2005 11:56:33 AM EST

#include <vil/vil_image_view.h>
#include <vcl_vector.h>


//: Linear-time, 2D exact Euclidean distance transform (Maurer's Algorithm)
//
// This is one of the fastest methods according to my experiments (distance.sourceforge.net)
// Paper: Calvin Maurer et. al. PAMI feb. 2003
//
// Squared Euclidean distances are computed in-place for every pixel relative to the zero-pixels.
//
// \param[in,out] im   The input binary image using vxl_uint_32 storage. The
// squared distance map is output to the same array, since the squared Euclidean
// distances are integers, assuming pixels are unit distance apart.
//
// \remarks Non-contiguous images not currently supported.
//
//  Implementation influenced by the LTI lib, which is licensed under LGPL:
//    http://ltilib.sourceforge.net
//
bool vil_exact_distance_transform_maurer(vil_image_view<vxl_uint_32> &im);

//: Cubic-time, exact Euclidean distance transform (Saito's algorithm)
//
// This algorithm is linear-time on average, but it is cubic-time in the worst
// case. It has reasonably good performance compared to newer algorithms,
// according to my experiments (distance.sourceforge.net).
//
// Paper: T. Saito and J.I. Toriwaki, "New algorithms for Euclidean distance
// transformations of an n-dimensional digitised picture with applications",
// Pattern Recognition, 27(11), pp. 1551-1565, 1994
//
// Squared Distances are computed for every pixel relative to the zero-pixels.
//
// \param[in,out] im   The input binary image using vxl_uint_32 storage. The
// squared distance map is output to the same array, since the squared Euclidean
// distances are integers, assuming pixels are unit distance apart.
//
// \remarks Non-contiguous images not currently supported.
//
// Final implementation by Ricardo Fabbri,
// based on two independent implementations by Olivier Cuisenaire
// and Julio Torelli.
//
bool vil_exact_distance_transform_saito(vil_image_view<vxl_uint_32> &im, unsigned plane_idx=0);

//: Overload that assumes given a Lookup table of integer squares.
// Also assumes the image im already has infinity in all non-zero points.
//
// \sa see documentation of vil_exact_distance_transform_saito(vil_image_view<vxl_uint_32> &im, unsigned plane_idx=0);
//
bool vil_exact_distance_transform_saito(vil_image_view<vxl_uint_32> &im, unsigned plane_idx, const vcl_vector<unsigned> &sq);

//: Exact 3D EDT
//
// \remarks See comment on vil_exact_distance_transform_saito
//
bool vil_exact_distance_transform_saito_3D(vil_image_view<vxl_uint_32> &im);

//: Computes signed Euclidean distance transform by using unsigned EDT of an image and its binary complement.
//
// The input image will be modified as an auxiliary array, so if you want to
// keep the input you are responsible for making a copy before calling this
// function.
//
// Regions on input image are pixels where intensity > 0;
// Distance Transform is > 0 INSIDE the regions amd
//                       < 0 OUTSIDE the regions.
//
// \Remarks The code for combining the interior and exterior distance maps
// has been blindly adapted from a previous implementation; the original code was from
// a legacy repository at Brown University (algo/contourtracing/signed_dt.cpp)
//
bool vil_exact_distance_transform_signed( vil_image_view<unsigned int>& input_image,
                                          vil_image_view<float>& signed_edt_image);


//: Naive implementation of exact Euclidean distance transform (for ground-truth)
//
// Quadratic complexity: O(N^2) for image with N pixels.
// Works for both 3D and 2D.
//
// Squared Euclidean distances are computed in-place for every pixel relative to the zero-pixels.
//
// \param[in,out] im   The input binary image using vxl_uint_32 storage. The
// squared distance map is output to the same array, since the squared Euclidean
// distances are integers, assuming pixels are unit distance apart.
//
//
bool vil_exact_distance_transform_brute_force(vil_image_view<vxl_uint_32> &im);

//: Another Naive implementation of exact Euclidean distance transform (for ground-truth).
//
// This implementation uses about 2x more memory than plain brute-force but is faster in
// most cases. It is O(N^2) if the number of white pixels is about the same as
// the number of black pixels. In general, the complexity is between O(N) and
// O(N^2) depending on the content (N is the total number of pixels).
//
// \sa description of vil_exact_distance_transform_brute_force
//
bool vil_exact_distance_transform_brute_force_with_list(vil_image_view<vxl_uint_32> &im);


//: Row-wise 1D EDT
//
// This is the first step for independent-scanning EDT algorithms.
//
// \param[in,out] im : input image.  The non-zero pixels must have a very large
// value (larger than the number of rows of the image). In the output, non-zero
// pixels will contain minimum distance to the zero-pixels along the rows.
//
// \Remarks
// This particular implementation is based on the 1st part of the following method:
// R. Lotufo and F. Zampirolli, Fast multidimensional parallel euclidean distance
// transform based on mathematical morphology, in T. Wu and D. Borges, editors,
// Proccedings of SIBGRAPI 2001, XIV Brazilian Symposium on Computer Graphics
// and Image Processing, pages 100-105. IEEE Computer Society, 2001.
//
bool vil_exact_distance_transform_1d_horizontal(vil_image_view<vxl_uint_32> &im);


#endif // vil_exact_distance_transform_h
