#ifndef my_map_functions_txx_
#define my_map_functions_txx_

#include <vcl_iostream.h>

template <class M>
void display_tmap(const M & map)
{
  typename M::const_dart_iterator d, end ;

  vcl_cout<<"Vertices :"<<vcl_endl ;
  for (typename M::const_vertex_iterator v=map.begin_vertex(); v!=map.end_vertex(); ++v)
  {
    vcl_cout<<'\t'//<<map.index(*v)<<'/'
            <<v->id()<<':' ;
    d=v->begin(); end=d ;
    do
    {
      vcl_cout//<<map.index(d->edge())<<'/'
              <<d->id()<<'|'<<M::cast(d->edge()).id()<<' ' ;
      d.sigma() ;
    }
    while (d!=end) ;
    vcl_cout<<vcl_endl ;
  }

  vcl_cout<<"Edges :"<<vcl_endl ;
  for (typename M::const_edge_iterator e=map.begin_edge(); e!=map.end_edge(); ++e)
  {
    vcl_cout<<'\t'//<<map.index(*e)<<'/'
            <<e->id()<<':' ;
    d=e->begin() ;
    vcl_cout//<<map.index((*d).vertex())<<'/'
            <<d->id()<<'|'<<M::cast((*d).vertex()).id()<<'|'<<M::cast((*e).first_vertex()).id()<<' ' ;
    d.alpha() ;
    vcl_cout//<<map.index((*d).vertex())<<'/'
            <<d->id()<<'|'<<M::cast((*d).vertex()).id()<<'|'<<M::cast((*e).last_vertex()).id()<<vcl_endl ;
  }

  vcl_cout<<"face_types :"<<vcl_endl ;
  for (typename M::const_face_iterator f=map.begin_face(); f!=map.end_face(); ++f)
  {
    vcl_cout<<'\t'//<<map.index(*f)<<'/'
            <<f->id()<<':' ;
    d=f->begin();
    end=d ;
    do
    {
      vcl_cout//<<map.index(d->edge())<<'/'
              <<d->id()<<'|'
      <<M::cast(d->edge()).id()<<' ' ;
      d.phi() ;
    }
    while (d!=end) ;
    vcl_cout<<vcl_endl ;
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
  vcl_cout<<'{'<<vcl_flush ;
  if (kernel.size()>0)
    vcl_cout<<kernel.dart(0)->id()<<vcl_flush ;
  for (unsigned int i=1; i<kernel.size(); ++i)
  {
    vcl_cout<<','<<kernel.dart(i)->id()<<vcl_flush ;
  }
  vcl_cout<<'}' <<vcl_flush ;
}

#endif
