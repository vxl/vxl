// This is prip/vdtop/vdtop_kernel.h
#ifndef vdtop_kernel_h_
#define vdtop_kernel_h_
//:
// \file
// \brief Provides the kernel function, which computes the kernel of some transformations based on local criteria.
// \author Jocelyn Marchadier
// \date 06 May 2004
//
// \verbatim
//  Modifications
//   06 May 2004 Jocelyn Marchadier
// \endverbatim

#include <iostream>
#include <vector>
#include "vdtop_neighborhood.h"
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#define BORDER_PIXEL (vxl_byte)2
#define FALSE_PIXEL (vxl_byte)0
#define TRUE_PIXEL (vxl_byte)1

template <class T>
void vdtop_set_image_border (vil_image_view<T> & img, int img_thickness, const T & value)
{
  for (unsigned int i = 0; i < img.ni() ; i++)
  {
    for (int k = 0; k<img_thickness; k++)
      img(i,k) = img(i, img.nj() -1-k) = value ;
  }
  for (unsigned int j = 0; j < img.nj() ; j++)
  {
    for (int k = 0; k<img_thickness; k++)
      img(k,j) = img(img.ni() -1-k,j) = value ;
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

  typedef typename vil_image_view<T>::iterator It;
  std::vector<It> current, next ;

  vil_image_view<vxl_byte>::iterator ii=included.begin();

  for (It i = img.begin() ; i!=img.end(); ++i, ++ii)
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
    for (typename std::vector<It>::iterator i=next.begin() ; i!=next.end(); ++i)
    {
      *(included.top_left_ptr()+(*i-img.top_left_ptr()))=FALSE_PIXEL;
    }
    current.swap(next) ;
  }
}

#endif
