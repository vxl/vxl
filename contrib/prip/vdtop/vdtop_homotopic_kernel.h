// This is contrib/prip/vdtop/vdtop_homotopic_kernel_h_
#ifndef vdtop_homotopic_kernel_h_
#define vdtop_homotopic_kernel_h_

//:
// \file
// \brief Provides a 2-map (combinatorial map) which is the base structure of vmap.
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  06 May 2004 Jocelyn Marchadier
// \endverbatim

#include "vdtop_neighborhood.h"

enum vdtop_pixel_type { border_pixel, false_pixel, true_pixel} ;

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
	
	typedef vdtop_8_neighborhood::const_iterator iterator ;
	
	void set_position(vil_image_view<T>::iterator arg)
	{
		_pixel.set_position(arg) ;
	}
	
	bool can_remove()
	{
		return _pixel.is_8_constructible() ;
	}
	
	void remove()
	{
		_pixel.construct8() ;
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

template <class T>
class vdtop_8_lower_homotopic
{
protected:
	vdtop_pixel<T> _pixel ;
	vdtop_8_neighborhood<T> _neighborhood ;
public:	
	vdtop_8_lower_homotopic(vil_image_view<T> & arg) 
		:_pixel(arg, arg.begin()) 
	{}
	
	typedef vdtop_8_neighborhood::const_iterator iterator ;
	
	void set_position(vil_image_view<T>::iterator arg)
	{
		_pixel.set_position(arg) ;
	}
	
	bool can_remove()
	{
		return _pixel.is_8_destructible() ;
	}
	
	void remove()
	{
		_pixel.destruct8() ;
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

template <class T>
class vdtop_4_upper_homotopic
{
protected:
	vdtop_pixel<T> _pixel ;
	vdtop_4_neighborhood<T> _neighborhood ;
public:	
	vdtop_4_upper_homotopic(vil_image_view<T> & arg) 
		:_pixel(arg, arg.begin()) 
	{}
	
	typedef vdtop_4_neighborhood::const_iterator iterator ;
	
	void set_position(vil_image_view<T>::iterator arg)
	{
		_pixel.set_position(arg) ;
	}
	
	bool can_remove()
	{
		return _pixel.is_4_constructible() ;
	}
	
	void remove()
	{
		_pixel.construct4() ;
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

template <class T>
class vdtop_4_lower_homotopic
{
protected:
	vdtop_pixel<T> _pixel ;
	vdtop_4_neighborhood<T> _neighborhood ;
public:	
	vdtop_4_lower_homotopic(vil_image_view<T> & arg) 
		:_pixel(arg, arg.begin()) 
	{}
	
	typedef vdtop_4_neighborhood::const_iterator iterator ;
	
	void set_position(vil_image_view<T>::iterator arg)
	{
		_pixel.set_position(arg) ;
	}
	
	bool can_remove()
	{
		return _pixel.is_4_destructible() ;
	}
	
	void remove()
	{
		_pixel.destruct4() ;
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

template <class T, class TPredicate>
void vdtop_kernel(vil_image_view<T> & arg, TPredicate & pred)
{
  TPredicate neighTest(arg) ;
	vil_image_view mark<vdtop_pixel_type> included(arg.ni(),arg.nj()) ;
	vil_image_view mark<vdtop_pixel_type>::iterator ii;
	included.fill(false_pixel) ;
  vdtop_set_image_border(included,1, border_pixel) ;
	
  vcl_vector< vil_image_view<T>::iterator > current, next ;
	
  for (vil_image_view<T>::iterator i = arg.begin(), ii=included.begin() ; i!=arg.end(); ++i, ++ii)
  {
      if (*ii==pixel_false)
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
			pred.set_position(current.last()) ;
	  	if (pred.can_remove())
	    {	
	      do {
					pred.remove() ;
				} while (pred.can_remove()) ;
	      
				typename TPredicate::iterator neigh ;
				for (neigh=pred.begin_next(); neigh!=pred.end_next(); neigh++)
				{
					ii=included.upper_left_ptr()+(*neigh-arg.upper_left_ptr()) ;
					neighTest.set_position(*neigh) ;
					if (*ii==false_pix && neighTest.can_remove())
					{
						next.push_back(*neigh) ;
						*ii=true_pix ;
					}
				}
			}
			current.pop_back() ;
		}
		for (vcl_vector< vil_image_view<T>::iterator >::iterator i=next.begin() ; i!=next.end(); ++i)
		{
			*(included.upper_left_ptr()+(*i-arg.upper_left_ptr()))=false_pixel ;
		}
		current.swap(next) ;
	}
}

#endif
