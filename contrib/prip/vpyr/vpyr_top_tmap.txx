// This is contrib/prip/vpyr/vpyr_top_tmap.txx
#include "vpyr_top_tmap.h"

template < class TPyramid , class V , class E , class F , class D > 
vpyr_top_tmap< TPyramid , V , E , F , D >::~vpyr_top_tmap() 
{
}


template < class TPyramid , class V , class E , class F , class D > 
template <class M >
void vpyr_top_tmap< TPyramid , V , E , F , D >::set_structure(const M & arg) 
{
	_pyramid.set_base_structure(arg) ;
	_Base::set_structure(arg) ;

}

template < class TPyramid  , class V , class E , class F , class D > 
void vpyr_top_tmap< TPyramid , V , E , F , D >::contraction(const contraction_kernel & arg) 
{
	typename pyramid_type::contraction_kernel ck(_pyramid.top_level()) ;
	ck.initialise() ;
	//contraction_kernel::const_iterator i ;
	for (int i=0; i<arg.size(); ++i)
	{
		ck.add(arg.dart(i)->sequence_index()) ;
	}
	ck.finalise() ;
	/* for (int i=0; i<nb_vertices(); ++i)
  		{
				if (_pyramid.base_map().vertex_position(i)!=vertex(i).value())
				{
					vcl_cout<<_pyramid.base_map().vertex_position(i)<<"ii"<<vertex(i).value()<<" " ;vcl_cout.flush() ;
				}
  	 }
		for (int i=0; i<arg.size(); ++i)
	{
		if (ck.dart(i)->vertex().sequence_index()!= arg.dart(i)->vertex().sequence_index())
		{
			vcl_cout<<"ICI2"<<_pyramid.base_map().vertex(ck.dart(i)->vertex().sequence_index()).last_level()<<'/'<<ck.dart(i)->vertex().sequence_index()<<','<<arg.dart(i)->vertex().sequence_index()<<vcl_endl ;
		}
	}*/

	//if (!ck.valid()) vcl_cerr<<"error while copying contraction kernel"<<vcl_endl ;
	_pyramid.top_level().contraction(ck) ;
	_Base::contraction(arg) ;
}

template < class TPyramid  , class V , class E , class F , class D > 
void vpyr_top_tmap< TPyramid , V , E , F , D >::removal(const removal_kernel & arg)
{
	typename pyramid_type::removal_kernel ck(_pyramid.top_level()) ;
	/*ck.initialise() ;
	//removal_kernel::const_iterator i ;
	for (int i=0; i<arg.size(); ++i)
	{
		ck.add(arg.dart(i)->sequence_index()) ;
	}
	ck.finalise() ;*/
	ck=arg ;
	//if (!ck.valid()) vcl_cerr<<"error while copying removal kernel"<<vcl_endl ;
	_pyramid.top_level().removal(ck) ;
	_Base::removal(arg) ;
}

