#ifndef vil_cartesian_differential_invariants_h_
#define vil_cartesian_differential_invariants_h_
//:
// \file
// \brief Find Cartesian differential Invariants
// \author Ian Scott

#include <vil/vil_fwd.h>

//: Compute up to 3rd order C.d.i. of an image.
// Finds the first 8 Cartesian differential invariants of an image. That is
// 1x1st order, 3x2nd order and 4x3rd order.
// The results are returned in 8 adjacent planes (for each input plane)
//
// See Florak et al. Image and Vision Computing, 10(6) pp376-388, 1992. and
// Walker et al. Proc.BMVC1997 pp541-549.
template <class S, class T>
void vil_cartesian_differential_invariants_3(const vil_image_view<S>& src,
                   vil_image_view<T>& dest, double scale);


#endif // vil_cartesian_differential_invariants_h_
