// This is prip/vdtop/vdtop_8_upper_leveling_kernel.h
#ifndef vdtop_8_upper_leveling_kernel_h_
#define vdtop_8_upper_leveling_kernel_h_
// :
// \file
// \brief Provides a function for computing a 8-connected upper leveling kernel of Bertrand and al
// ( G. Bertrand, J. C. Everat and M. Couprie, "Image segmentation through operators based upon topology",
//   Journal of Electronic Imaging, Vol. 6, N. 4, 395-405, 1997).
// \author Jocelyn Marchadier
// \date 06 May 2004
//
// \verbatim
//  Modifications
//   06 May 2004 Jocelyn Marchadier
// \endverbatim

#include "vdtop_kernel.h"

template <class T>
class vdtop_8_upper_leveling
{
protected:
  vdtop_pixel<T>          pixel_;
  vdtop_8_neighborhood<T> neighborhood_;
public:
  vdtop_8_upper_leveling(vil_image_view<T> & arg) : pixel_(arg, arg.begin() ) {}

  typedef typename vdtop_8_neighborhood<T>::const_iterator iterator;

  void set_position(typename vil_image_view<T>::iterator arg)
  {
    pixel_.set_position(arg);
  }

  bool can_remove()
  {
    return pixel_.is_8_constructible_or_well();
  }

  void remove()
  {
    pixel_.construct_8();
  }

  iterator begin_next()
  {
    neighborhood_.set_center(pixel_);
    return neighborhood_.begin();
  }

  iterator end_next()
  {
    return neighborhood_.end();
  }

};

// : computes the 8 upper leveling.
template <class T>
void vdtop_8_upper_leveling_kernel(vil_image_view<T> & arg)
{
  vdtop_8_upper_leveling<T> predicate(arg);
  vdtop_kernel(arg, predicate);
}

#endif
