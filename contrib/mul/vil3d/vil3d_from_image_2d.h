// This is mul/vil3d/vil3d_from_image_2d.h
#ifndef vil3d_from_image_2d_h_
#define vil3d_from_image_2d_h_

#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif

//:
// \file
// \author Kevin de Souza

#include <vil/vil_image_view.h>
#include <vil3d/vil3d_image_view.h>
#include <vcl_cassert.h>


//: Return a 3D image view containing a single slice obtained from a 2D image.
//  result(x,y,0,p)=im(x,y,p)
// \relates vil3d_image_view
// \relates vil_image_view
// \note Assumes that input image planes (if more than 1) are stored separately,
// i.e. that im.planestep()==im.ni()*im.nj()
template <class T>
inline vil3d_image_view<T> vil3d_from_image_2d(const vil_image_view<T>& im)
{
  if (im.is_contiguous())
  {
    vcl_ptrdiff_t kstep = im.ni()*im.nj();
    unsigned nk = 1;
    vcl_ptrdiff_t pstep = im.planestep();

    // Insist on a particular ordering of input image data
    assert(pstep == kstep);
  
    return vil3d_image_view<T>(im.memory_chunk(),
                               im.top_left_ptr(),
                               im.ni(), im.nj(), nk, im.nplanes(),
                               im.istep(), im.jstep(), kstep, pstep);
  }
  else
  {
    return vil3d_image_view<T>();
  }
}


#endif // vil3d_from_image_2d_h_
