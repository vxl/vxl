// This is contrib/prip/vdtop/vdtop_kernel_h_
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
void vdtop_set_image_border (vil_image_view<T> & arg, int arg_thickness, const T & value)
{
  int i , k ;
  for (i = 0; i < arg.ni() ; i++)
  {
    for (k = 0; k<arg_thickness; k++)
      arg(i,k) = arg (i, arg.nj() -1-k) = value ;
  }
  for ( i = 0; i < arg.nj() ; i++)
  {
    for (k = 0; k<arg_thickness; k++)
      arg(k,i) = arg (arg.ni() -1-k,i) = value ;
  }
}

//: computes the kernel of some transformations based on local criteria tested by predicate "pred".
template <class T, class TPredicate>
void vdtop_kernel(vil_image_view<T> & arg, TPredicate & pred)
{
  TPredicate neighTest(arg) ;
	vil_image_view<vxl_byte> included(arg.ni(),arg.nj()) ;
	included.fill(FALSE_PIXEL) ;
  vdtop_set_image_border(included,1, BORDER_PIXEL) ;
	
  vcl_vector< typename vil_image_view<T>::iterator > current, next ;
	
  vil_image_view<vxl_byte>::iterator ii=included.begin();
    
	for (typename vil_image_view<T>::iterator i = arg.begin() ; i!=arg.end(); ++i, ++ii)
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
					ii=included.top_left_ptr()+(*neigh-arg.top_left_ptr()) ;
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
			*(included.top_left_ptr()+(*i-arg.top_left_ptr()))=FALSE_PIXEL;
		}
		current.swap(next) ;
	}
}

#endif
