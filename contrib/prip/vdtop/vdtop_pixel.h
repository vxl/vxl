// This is contrib/prip/vdtop/vdtop_pixel.h
#ifndef vdtop_pixel_h_
#define vdtop_pixel_h_
//:
// \file
// \brief Provides a class for handling topological maniplation on points of an image.
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  06 May 2004 Jocelyn Marchadier
// \endverbatim

#include "vdtop_8_neighborhood_mask.h"
#include <vil/vil_image_view.h>

template <class T>
class vdtop_pixel
{

public:
	typedef vdtop_pixel<T> self_type ;
	typedef typename vil_image_view<T>::iterator image_iterator ;

	vdtop_pixel(vil_image_view<T> & img, image_iterator arg)
		:_position(arg), _img(&img)
	{
	}
	
	vil_image_view<T> & image() const
	{
		return *_img ;
	}
	
	void set_position(image_iterator arg)
	{
		_position=arg ;
	}
	
	image_iterator position() const
	{
		return _position ;
	}
	
	self_type & operator=(const self_type & arg) 
	{
		_position=arg._position; 
		_img=arg._img ;
	}
	
	self_type operator+(vdtop_freeman_code code) const
	{
		return self_type(img,_position+(code.di()*_img->istep()+code.dj()*_img->jstep())) ;
	}

	T& value() 
	{
		return *_position ;
	}
		
	const T& value() const
	{
		return *_position ;
	}
		
	//: Returns the set composed of all points in the neighborhood of this having a value greater or equal.
	vdtop_8_neighborhood_mask upper_neighborhood() const ;
	
	//: Returns the set composed of all points in the neighborhood of this having a value lower or equal.
	vdtop_8_neighborhood_mask lower_neighborhood() const ;
	
	//: Returns true if the point is 4-simple for its section.
	bool is_4_destructible() const
	{
		return upper_neighborhood().is_4_simple() ;
	}
	
	//: Returns true if the point is 4-simple or is isolated for its section.
	bool is_4_destructible_or_peak() const
	{
		return upper_neighborhood().is_4_simple_or_isolated() ;
	}
	
	//: Returns true if the point is 8-simple for its section.
	bool is_8_destructible() const
	{
		return upper_neighborhood().is_8_simple() ;
	}
	
	//: Returns true if the point is 8-simple or is isolated for its section.
	bool is_8_destructible_or_peak() const
	{
		return upper_neighborhood().is_8_simple_or_isolated() ;
	}
	
	//: Returns true if the point is 4-simple for the complementary of the first section not containing this.
	bool is_4_constructible() const
	{
		return lower_neighborhood().is_4_simple() ;
	}
	
	//: Returns true if the point is 8-simple for the complementary of the first section not containing this.
	bool is_8_constructible() const
	{
		return lower_neighborhood().is_8_simple() ;
	}
	
	//: Returns true if the point is 4-simple or is isolated for the complementary of the first section not containing this.
	bool is_4_constructible_or_well() const
	{
		return lower_neighborhood().is_4_simple_or_isolated() ;
	}
	
	//: Returns true if the point is 8-simple or is isolated for the complementary of the first section not containing this.
	bool is_8_constructible_or_well() const
	{
		return lower_neighborhood().is_8_simple_or_isolated() ;
	}
	
	//: Lowers the value of the point so that it has the closest value among its lower 4-neighborhood
	void destruct_4() ;
	
	//: Lowers the value of the point so that it has the closest value among its lower 8-neighborhood
	void destruct_8() ;
	
	//: Raises the value of the point so that it has the closest value among its upper 4-neighborhood
	void construct_4() ;

	//: Raises the value of the point so that it has the closest value among its upper 8-neighborhood
	void construct_8() ;
	
	//: 
	int t8p() const
	{
		return upper_neighborhood().t8p() ;
	}
	
	//:
	int t4p() const 
	{
		return upper_neighborhood().t4p() ;
	}
	
	//:
	int t4mm() const
	{
		return upper_neighborhood().t4mm() ;
	}
	
	//:
	int t8mm() const 
	{
		return upper_neighborhood().t8mm() ;
	}

	//:
	int t8m() const
	{
		return lower_neighborhood().t8p() ;
	}
	
	//:
	int t4m() const 
	{
		return lower_neighborhood().t4p() ;
	}
	
	//:
	int t4pp() const
	{
		return lower_neighborhood().t4mm() ;
	}
	
	//:
	int t8pp() const 
	{
		return lower_neighborhood().t8mm() ;
	}

private:
	image_iterator _position ;
	vil_image_view<T> * _img ;
} ;

#include "vdtop_pixel.txx"

#endif
