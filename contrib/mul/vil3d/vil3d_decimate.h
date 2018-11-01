// This is mul/vil3d/vil3d_decimate.h
#ifndef vil3d_decimate_h_
#define vil3d_decimate_h_
//:
// \file
// \author Ian Scott.

#include <vil3d/vil3d_fwd.h>


//: Create a view which is a decimated version of src.
// Doesn't modify underlying data. O(1).
// \relatesalso vil3d_image_view
// The factor describes the number of input rows (or columns)
// that are equivalent to one output.
// If you don't specify the j_factor or k_factor, they will be set equal to i_factor.
template<class T>
inline vil3d_image_view<T> vil3d_decimate(const vil3d_image_view<T> &im,
  unsigned i_factor, unsigned j_factor=0, unsigned k_factor=0)
{
  if (j_factor==0) j_factor=i_factor;
  if (k_factor==0) k_factor=i_factor;
  // use (n+d-1)/n instead of ceil((double)n/d) to calcualte sizes
  return vil3d_image_view<T>(
    im.memory_chunk(), im.origin_ptr(), (im.ni()+i_factor-1u)/i_factor,
    (im.nj()+j_factor-1u)/j_factor,  (im.nk()+k_factor-1u)/k_factor, im.nplanes(),
    im.istep()*i_factor, im.jstep()*j_factor, im.kstep()*k_factor,
    im.planestep() );
}

#endif // vil3d_decimate_h_
