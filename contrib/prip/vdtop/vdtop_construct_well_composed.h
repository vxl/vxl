// This is prip/vdtop/vdtop_construct_well_composed.h
#ifndef vdtop_construct_well_composed_h_
#define vdtop_construct_well_composed_h_
//:
// \file
// \brief Provides a function for computing a 4-connected lower leveling kernel of Bertrand and al
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
class vdtop_well_composed
{
 protected:
  vdtop_pixel<T> pixel_ ;
  vdtop_8_neighborhood<T> neighborhood_ ;
 public:
  vdtop_well_composed(vil_image_view<T> & arg) :pixel_(arg, arg.begin()) {}

  typedef typename vdtop_8_neighborhood<T>::const_iterator iterator ;

  void set_position(typename vil_image_view<T>::iterator arg)
  {
    pixel_.set_position(arg) ;
    neighborhood_.set_center(pixel_) ;
  }

  bool can_remove()
  {
    return
    (neighborhood_.neighbor_value(7)<=pixel_.value() &&
     pixel_.value()<neighborhood_.neighbor_value(6) &&
     pixel_.value()<neighborhood_.neighbor_value(0))||
    (neighborhood_.neighbor_value(1)<=pixel_.value() &&
     pixel_.value()<neighborhood_.neighbor_value(0) &&
     pixel_.value()<neighborhood_.neighbor_value(2))||
    (neighborhood_.neighbor_value(3)<=pixel_.value() &&
     pixel_.value()<neighborhood_.neighbor_value(2) &&
     pixel_.value()<neighborhood_.neighbor_value(4))||
    (neighborhood_.neighbor_value(5)<=pixel_.value() &&
     pixel_.value()<neighborhood_.neighbor_value(6) &&
     pixel_.value()<neighborhood_.neighbor_value(4)) ;
  }

  void remove()
  {
    pixel_.construct_4() ;
  }

  iterator begin_next()
  {
    return neighborhood_.begin() ;
  }
  iterator end_next()
  {
    return neighborhood_.end() ;
  }
};

//: Construct a well composed image of arg
template <class T>
void vdtop_construct_well_composed(vil_image_view<T> & arg)
{
  vdtop_well_composed<T> predicate(arg) ;
  vdtop_kernel(arg, predicate) ;
}

#endif
