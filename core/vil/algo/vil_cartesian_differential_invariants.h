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
// The results are unscaled. If the range of your input is $r$,
// you can normalise the results by dividing the planes by
// $r, r^{-1}, r^{-1}, r^{-1}, r^{-2}, r^{-2}, r^{-2}, r^{-2}$
//
// See Romeny et al. Proc.IPMI1993, pp77-93. and
// Walker et al. Proc.BMVC1997 pp541-549.
// \param max_kernel_width. Set this value (to an odd number)
// if you want to restrict the size of the kernel. 0 will
// let the function choose an appropriate kernel size for the \p scale.
// 
template <class S, class T>
void vil_cartesian_differential_invariants_3(const vil_image_view<S>& src,
                   vil_image_view<T>& dest, double scale,
                   unsigned max_kernel_width =0);


#endif // vil_cartesian_differential_invariants_h_
