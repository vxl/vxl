#ifndef vil_cartesian_differential_invariants_h_
#define vil_cartesian_differential_invariants_h_
//:
// \file
// \brief Find Cartesian differential Invariants
// \author Ian Scott

#include <vil/vil_fwd.h>

//: Compute up to 3rd order C.d.i. of an image.
//  Computes both i and j gradients of an nx x ny plane of data
//  grad_ij has twice as many planes as src, with dest plane (2i) being the i-gradient
//  of source plane i and dest plane (2i+1) being the j-gradient.
//  1 pixel border around grad images is set to zero
template <class S, class T>
void vil_cartesian_differential_invariants_3(const vil_image_view<S>& src,
                   vil_image_view<T>& dest, double scale);


#endif // vil_cartesian_differential_invariants_h_
