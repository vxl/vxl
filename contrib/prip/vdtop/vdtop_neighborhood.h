// This is contrib/prip/vdtop/vdtop_neighborhood.h
#ifndef vdtop_neighborhood_h_
#define vdtop_neighborhood_h_
//:
// \file
// \brief Provides vdtop_pixel neighborhoods class. Can be yet enhanced.
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  06 May 2004 Jocelyn Marchadier
// \endverbatim

#include "vdtop/vdtop_pixel.h"
#include "vdtop/vdtop_freeman_code.h"
#include <vil/vil_image_view.h>

template <class T>
class vdtop_8_neighborhood
{

public:
	typedef vdtop_8_neighborhood<T> self_type ;
	typedef typename vil_image_view<T>::iterator const * const_iterator ;
	
	void set_center(vdtop_pixel<T> & pix)
	{
		vil_image_view<T> & img=pix.image() ;
		vcl_ptrdiff_t istep=img.istep();
		vcl_ptrdiff_t jstep=img.jstep();
		typename vil_image_view<T>::iterator tmp=pix.position();
		tmp+=istep ;
		_neighbors[0]=tmp ;
		tmp-=jstep ;
		_neighbors[1]=tmp ;
		tmp-=istep ;
		_neighbors[2]=tmp ;
		tmp-=istep ;
		_neighbors[3]=tmp ;
		tmp+=jstep ;
		_neighbors[4]=tmp ;
		tmp+=jstep ;
		_neighbors[5]=tmp ;
		tmp+=istep ;
		_neighbors[6]=tmp ;
		tmp+=istep ;
		_neighbors[7]=tmp ;
	}
		
	const_iterator begin() const
	{
		return _neighbors ;
	}
	const_iterator end() const
	{
		return _neighbors+8 ;
	}
	
	const T& neighbor_value(vdtop_freeman_code arg) const
	{
		return *_neighbors[arg.code()] ;
	}
	
	
private:
	typename vil_image_view<T>::iterator _neighbors[8] ;
} ;

template <class T>
class vdtop_4_neighborhood
{

public:
	typedef vdtop_4_neighborhood<T> self_type ;
	typedef typename vil_image_view<T>::iterator const * const_iterator ;
	
	void set_center(vdtop_pixel<T> & pix)
	{
		vil_image_view<T> & img=pix.image() ;
		typename vil_image_view<T>::iterator tmp=pix.position();
		vcl_ptrdiff_t istep=img.istep();
		vcl_ptrdiff_t jstep=img.jstep();
		tmp+=istep ;
		_neighbors[0]=tmp ;
		tmp-=jstep ;
		tmp-=istep ;
		_neighbors[1]=tmp ;
		tmp-=istep ;
		tmp+=jstep ;
		_neighbors[2]=tmp ;
		tmp+=istep ;
		tmp+=jstep ;
		_neighbors[3]=tmp ;
	}
		
	const_iterator begin() const
	{
		return _neighbors ;
	}
	const_iterator end() const
	{
		return _neighbors+4 ;
	}
	
	
private:
	typename vil_image_view<T>::iterator _neighbors[4] ;
} ;

#endif
