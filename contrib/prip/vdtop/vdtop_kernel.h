// This is prip/vdtop/vdtop_kernel.h
#ifndef vdtop_kernel_h_
#define vdtop_kernel_h_

//:
// \file
// \brief Provides the kernel function, which computes the kernel of some transformations based on local criteria.
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  06 May 2004 Jocelyn Marchadier
// \endverbatim

#include "vdtop_neighborhood.h"
#include <vcl_vector.h>

#define BORDER_PIXEL (vxl_byte)2
#define FALSE_PIXEL (vxl_byte)0
#define TRUE_PIXEL (vxl_byte)1

template <class T>
void vdtop_set_image_border (vil_image_view<T> & img, int img_thickness, const T & value)
{
  int i , k ;
  for (i = 0; i < img.ni() ; i++)
  {
    for (k = 0; k<img_thickness; k++)
      img(i,k) = img(i, img.nj() -1-k) = value ;
  }
  for ( i = 0; i < img.nj() ; i++)
  {
    for (k = 0; k<img_thickness; k++)
      img(k,i) = img(img.ni() -1-k,i) = value ;
  }
}

//: computes the kernel of some transformations based on local criteria tested by predicate "pred".
template <class T, class TPredicate>
void vdtop_kernel(vil_image_view<T> & img, TPredicate & pred)
{
  TPredicate neighTest(img) ;
  vil_image_view<vxl_byte> included(img.ni(),img.nj()) ;
  included.fill(FALSE_PIXEL) ;
  vdtop_set_image_border(included,1, BORDER_PIXEL) ;

  vcl_vector< typename vil_image_view<T>::iterator > current, next ;

  vil_image_view<vxl_byte>::iterator ii=included.begin();

  for (typename vil_image_view<T>::iterator i = img.begin() ; i!=img.end(); ++i, ++ii)
  {
      if (*ii==FALSE_PIXEL)
      {
        pred.set_position(i) ;
        if (pred.can_remove())
        {
          current.push_back(i) ;
        }
      }
    }
  while (!current.empty())
  {
    while (!current.empty())
    {
      pred.set_position(current.back()) ;
      if (pred.can_remove())
      {
        do {
          pred.remove() ;
        } while (pred.can_remove()) ;

        typename TPredicate::iterator neigh ;
        for (neigh=pred.begin_next(); neigh!=pred.end_next(); neigh++)
        {
          ii=included.top_left_ptr()+(*neigh-img.top_left_ptr()) ;
          neighTest.set_position(*neigh) ;
          if (*ii==FALSE_PIXEL && neighTest.can_remove())
          {
            next.push_back(*neigh) ;
            *ii=TRUE_PIXEL ;
          }
        }
      }
      current.pop_back() ;
    }
    for (typename vcl_vector< typename vil_image_view<T>::iterator >::iterator i=next.begin() ; i!=next.end(); ++i)
    {
      *(included.top_left_ptr()+(*i-img.top_left_ptr()))=FALSE_PIXEL;
    }
    current.swap(next) ;
  }
}

#endif
