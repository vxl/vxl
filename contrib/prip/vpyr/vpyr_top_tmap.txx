// This is prip/vpyr/vpyr_top_tmap.txx
#ifndef vpyr_top_tmap_txx_
#define vpyr_top_tmap_txx_

#include "vpyr_top_tmap.h"

template < class TPyramid , class V , class E , class F , class D >
vpyr_top_tmap< TPyramid , V , E , F , D >::~vpyr_top_tmap()
{
}

template < class TPyramid , class V , class E , class F , class D >
template <class M >
void vpyr_top_tmap< TPyramid , V , E , F , D >::set_structure(const M & arg)
{
  pyramid_.set_base_structure(arg) ;
  Base_::set_structure(arg) ;
}

template < class TPyramid  , class V , class E , class F , class D >
void vpyr_top_tmap< TPyramid , V , E , F , D >::contraction(const contraction_kernel & arg)
{
  typename pyramid_type::contraction_kernel ck(pyramid_.top_level()) ;
  ck.initialise() ;
#if 0
  contraction_kernel::const_iterator i ;
#endif // 0
  for (unsigned int i=0; i<arg.size(); ++i)
  {
    ck.add(arg.dart(i)->sequence_index()) ;
  }
  ck.finalise() ;
#ifdef DEBUG
  for (unsigned int i=0; i<nb_vertices(); ++i)
  {
    if (pyramid_.base_map().vertex_position(i)!=vertex(i).value())
    {
      vcl_cout<<pyramid_.base_map().vertex_position(i)<<"ii"<<vertex(i).value()<<' ' << vcl_flush;
    }
  }
  for (int i=0; i<arg.size(); ++i)
  {
    if (ck.dart(i)->vertex().sequence_index()!= arg.dart(i)->vertex().sequence_index())
    {
      vcl_cout<<"ICI2"<<pyramid_.base_map().vertex(ck.dart(i)->vertex().sequence_index()).last_level()<<'/'
              <<ck.dart(i)->vertex().sequence_index()<<','<<arg.dart(i)->vertex().sequence_index()<<vcl_endl ;
    }
  }
#endif // 0

#if 0
  if (!ck.valid())
    vcl_cerr<<"error while copying contraction kernel\n" ;
#endif // 0
  pyramid_.top_level().contraction(ck) ;
  Base_::contraction(arg) ;
}

template < class TPyramid  , class V , class E , class F , class D >
void vpyr_top_tmap< TPyramid , V , E , F , D >::removal(const removal_kernel & arg)
{
  typename pyramid_type::removal_kernel ck(pyramid_.top_level()) ;
#if 0
  ck.initialise() ;
#if 0
  removal_kernel::const_iterator i ;
#endif // 0
  for (int i=0; i<arg.size(); ++i)
  {
    ck.add(arg.dart(i)->sequence_index()) ;
  }
  ck.finalise() ;
#endif // 0
  ck=arg ;
#if 0
  if (!ck.valid())
    vcl_cerr<<"error while copying removal kernel\n" ;
#endif // 0
  pyramid_.top_level().removal(ck) ;
  Base_::removal(arg) ;
}

#endif // vpyr_top_tmap_txx_
