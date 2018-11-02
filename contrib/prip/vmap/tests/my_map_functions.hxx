#ifndef my_map_functions_hxx_
#define my_map_functions_hxx_

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class M>
void display_tmap(const M & map)
{
  typename M::const_dart_iterator d, end ;

  std::cout<<"Vertices :"<<std::endl ;
  for (typename M::const_vertex_iterator v=map.begin_vertex(); v!=map.end_vertex(); ++v)
  {
    std::cout<<'\t'//<<map.index(*v)<<'/'
            <<v->id()<<':' ;
    d=v->begin(); end=d ;
    do
    {
      std::cout//<<map.index(d->edge())<<'/'
              <<d->id()<<'|'<<M::cast(d->edge()).id()<<' ' ;
      d.sigma() ;
    }
    while (d!=end) ;
    std::cout<<std::endl ;
  }

  std::cout<<"Edges :"<<std::endl ;
  for (typename M::const_edge_iterator e=map.begin_edge(); e!=map.end_edge(); ++e)
  {
    std::cout<<'\t'//<<map.index(*e)<<'/'
            <<e->id()<<':' ;
    d=e->begin() ;
    std::cout//<<map.index((*d).vertex())<<'/'
            <<d->id()<<'|'<<M::cast((*d).vertex()).id()<<'|'<<M::cast((*e).first_vertex()).id()<<' ' ;
    d.alpha() ;
    std::cout//<<map.index((*d).vertex())<<'/'
            <<d->id()<<'|'<<M::cast((*d).vertex()).id()<<'|'<<M::cast((*e).last_vertex()).id()<<std::endl ;
  }

  std::cout<<"face_types :"<<std::endl ;
  for (typename M::const_face_iterator f=map.begin_face(); f!=map.end_face(); ++f)
  {
    std::cout<<'\t'//<<map.index(*f)<<'/'
            <<f->id()<<':' ;
    d=f->begin();
    end=d ;
    do
    {
      std::cout//<<map.index(d->edge())<<'/'
              <<d->id()<<'|'
      <<M::cast(d->edge()).id()<<' ' ;
      d.phi() ;
    }
    while (d!=end) ;
    std::cout<<std::endl ;
  }
}

template <class M>
typename M::const_dart_iterator find_dart(const M & map, int id)
{
  typename M::const_dart_iterator d=map.begin_dart();
  while (d!=map.end_dart() && M::cast(d->edge()).id()!=id)
  {
    ++d ;
  };
  return d ;
}

template <class K>
void display_kernel(K & kernel)
{
  std::cout<<'{'<<std::flush ;
  if (kernel.size()>0)
    std::cout<<kernel.dart(0)->id()<<std::flush ;
  for (unsigned int i=1; i<kernel.size(); ++i)
  {
    std::cout<<','<<kernel.dart(i)->id()<<std::flush ;
  }
  std::cout<<'}' <<std::flush ;
}

#endif
