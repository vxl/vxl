// This is prip/vdtop/vdtop_8_upper_homotopic_kernel.h
#ifndef vdtop_8_upper_homotopic_kernel_h_
#define vdtop_8_upper_homotopic_kernel_h_

//:
// \file
// \brief Provides a function for computing a 8-connected upper homotopic kernel of Bertrand and al
// ( G. Bertrand, J. C. Everat and M. Couprie, "Image segmentation through operators based upon topology",
//   Journal of Electronic Imaging, Vol. 6, N. 4, 395-405, 1997).
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  06 May 2004 Jocelyn Marchadier
// \endverbatim

#include "vdtop_kernel.h"


template <class T>
class vdtop_8_upper_homotopic
{
 protected:
  vdtop_pixel<T> _pixel ;
  vdtop_8_neighborhood<T> _neighborhood ;
 public:
  vdtop_8_upper_homotopic(vil_image_view<T> & arg)
    :_pixel(arg, arg.begin())
  {}

  typedef typename vdtop_8_neighborhood<T>::const_iterator iterator ;

  void set_position(typename vil_image_view<T>::iterator arg)
  {
    _pixel.set_position(arg) ;
  }

  bool can_remove()
  {
    return _pixel.is_8_constructible() ;
  }

  void remove()
  {
    _pixel.construct_8() ;
  }

  iterator begin_next()
  {
    _neighborhood.set_center(_pixel) ;
    return _neighborhood.begin() ;
  }
  iterator end_next()
  {
    return _neighborhood.end() ;
  }
} ;

//: 8 upper homotopic kernel.
template <class T>
void vdtop_8_upper_homotopic_kernel(vil_image_view<T> & arg)
{
  vdtop_8_upper_homotopic<T> predicate(arg) ;
  vdtop_kernel(arg, predicate) ;
}

#endif
