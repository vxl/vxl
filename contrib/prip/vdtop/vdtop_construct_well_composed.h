// This is contrib/prip/vdtop/vdtop_construct_well_composed.h
#ifndef vdtop_construct_well_composed_h_
#define vdtop_construct_well_composed_h_

//:
// \file
// \brief Provides a function for computing a 4-connected lower leveling kernel of Bertrand and al ( G. Bertrand, J. C. Everat and M. Couprie, "Image segmentation through operators based upon topology", Journal of Electronic Imaging, Vol. 6, N. 4, 395-405, 1997).
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
	vdtop_pixel<T> _pixel ;
	vdtop_8_neighborhood<T> _neighborhood ;
public:	
	vdtop_well_composed(vil_image_view<T> & arg) 
		:_pixel(arg, arg.begin()) 
	{}
	
	typedef typename vdtop_8_neighborhood<T>::const_iterator iterator ;
	
	void set_position(typename vil_image_view<T>::iterator arg)
	{
		_pixel.set_position(arg) ;
		_neighborhood.set_center(_pixel) ;
	}
	
	bool can_remove()
	{
		return
		(_neighborhood.neighbor_value(7)<=_pixel.value() && _pixel.value()<_neighborhood.neighbor_value(6) && _pixel.value()<_neighborhood.neighbor_value(0))||
	  (_neighborhood.neighbor_value(1)<=_pixel.value() && _pixel.value()<_neighborhood.neighbor_value(0) && _pixel.value()<_neighborhood.neighbor_value(2))||
	  (_neighborhood.neighbor_value(3)<=_pixel.value() && _pixel.value()<_neighborhood.neighbor_value(2) && _pixel.value()<_neighborhood.neighbor_value(4))||
	  (_neighborhood.neighbor_value(5)<=_pixel.value() && _pixel.value()<_neighborhood.neighbor_value(6) && _pixel.value()<_neighborhood.neighbor_value(4)) ;
	}
	
	void remove()
	{
		_pixel.construct_4() ;
	}
	
	iterator begin_next()
	{
		return _neighborhood.begin() ;
	}
	iterator end_next()
	{
		return _neighborhood.end() ;
	}
} ;

//: Construct a well composed image of arg
template <class T>
void vdtop_construct_well_composed(vil_image_view<T> & arg)
{
	vdtop_well_composed<T> predicate(arg) ;
	vdtop_kernel(arg, predicate) ;
}

#endif
