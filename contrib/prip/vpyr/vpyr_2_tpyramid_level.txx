// This is prip/vpyr/vpyr_2_tpyramid_level.txx
#ifndef vpyr_2_tpyramid_level_txx_
#define vpyr_2_tpyramid_level_txx_

#include "vpyr_2_tpyramid_level.h"

template <class V, class E, class F, class D>
vpyr_2_tpyramid_level<V,E,F,D>::vpyr_2_tpyramid_level(vmap_level_index arg_level,  pyramid_type & pyramid)
  : _Base(arg_level, (typename _Base::pyramid_type &)pyramid),
    vertex_sequence(pyramid.base_map()),
    edge_sequence(pyramid.base_map()),
    face_sequence(pyramid.base_map())
{
    vertex_sequence_iterator v;
    for (v=begin_vertex_sequence(); v!=end_vertex_sequence();++v)
    {
      (*v)->set_last_level(index()) ;
    }

     edge_sequence_iterator e;
    for (e=begin_edge_sequence(); e!=end_edge_sequence();++e)
    {
      (*e)->set_last_level(index()) ;
    }

    face_sequence_iterator f;
    for (f=begin_face_sequence(); f!=end_face_sequence();++f)
    {
      (*f)->set_last_level(index()) ;
    }
}

template <class V, class E, class F, class D>
void vpyr_2_tpyramid_level<V,E,F,D>::contraction(const contraction_kernel &arg_kernel)
{
  self_type * l=pyramid().level_below(vmap_contraction_type,*this) ;
  if (l!=NULL)
  {
    int level=l->index() ;
    int ld=nb_darts(),
        lv=nb_vertices(),
        le=nb_edges(), id ;

    typename contraction_kernel::const_iterator itk ;
    for (int i=0; i<arg_kernel.size(); ++i)//itk=arg_kernel.begin(); itk!=arg_kernel.end(); ++itk)
    {
      //dart_pointer d =*itk ;

      id=dart_associated_edge(arg_kernel.get_dart_index(i)) ;
      edge_pointer e=get_edge_pointer(id) ;////vpyr_2_pyramid_base_ancestor((edge_pointer)d->get_edge_pointer(),index()) ;
      e->set_last_level(level) ;
      //id=e->sequence_index() ;
      --le ;
      edge_sequence::swap(id,le) ;

      id=arg_kernel.get_element_index(i) ;
      vertex_pointer v=get_vertex_pointer(id) ;////vpyr_2_pyramid_base_ancestor((vertex_pointer)d->get_vertex_pointer(),index()) ;
      v->set_last_level(level) ;
      //id=v->sequence_index() ;
      --lv ;
      vertex_sequence::swap(id,lv) ;
    }
    for (itk=arg_kernel.begin(); itk!=arg_kernel.end(); ++itk)
    {
      dart_pointer d =*itk ;
      d->set_last_level(level) ;
      id=d->sequence_index() ;
      --ld ;
      dart_sequence::swap(id,ld) ;

      id=vmap_2_map_alpha(d)->sequence_index() ;
      --ld ;
      dart_sequence::swap(id,ld) ;
    }
    dart_sequence::resize(ld) ;
    edge_sequence::resize(le) ;
    vertex_sequence::resize(lv) ;
  }
}

template <class V, class E, class F, class D>
void vpyr_2_tpyramid_level<V,E,F,D>::removal(const removal_kernel &arg_kernel)
{
  self_type * l=pyramid().level_below(vmap_removal_type,*this) ;
  if (l!=NULL)
  {
    int level=l->index() ;
    int ld=nb_darts(),
        lv=nb_faces(),
        le=nb_edges(), id ;

    typename contraction_kernel::const_iterator itk ;
    for (int i=0; i<arg_kernel.size(); ++i)//itk=arg_kernel.begin(); itk!=arg_kernel.end(); ++itk)
    {
      //dart_pointer d =*itk ;

      id=dart_associated_edge(arg_kernel.get_dart_index(i)) ;
      edge_pointer e=get_edge_pointer(id) ;////vpyr_2_pyramid_base_ancestor((edge_pointer)d->get_edge_pointer(),index()) ;
      e->set_last_level(level) ;
      //id=e->sequence_index() ;
      --le ;
      edge_sequence::swap(id,le) ;

      id=arg_kernel.get_element_index(i) ;
      face_pointer v=get_face_pointer(id) ;////vpyr_2_pyramid_base_ancestor((face_pointer)d->get_face_pointer(),index()) ;
      v->set_last_level(level) ;
      //id=v->sequence_index() ;
      --lv ;
      face_sequence::swap(id,lv) ;
    }
    for (itk=arg_kernel.begin(); itk!=arg_kernel.end(); ++itk)
    {
      dart_pointer d =*itk, ad=vmap_2_map_alpha(d) ;
      d->set_last_level(level) ;
      id=d->sequence_index() ;
      --ld ;
      dart_sequence::swap(id,ld) ;

      id=ad->sequence_index() ;
      --ld ;
      dart_sequence::swap(id,ld) ;
    }
    for (int i=le; i<nb_edges(); i++)
      get_edge_pointer(i)->set_last_level(level) ;
    for (int i=lv; i<nb_faces(); i++)
      get_face_pointer(i)->set_last_level(level) ;

    dart_sequence::resize(ld) ;
    edge_sequence::resize(le) ;
    face_sequence::resize(lv) ;
  }
}

template <class V, class E, class F, class D>
vmap_2_tmap_tag vpyr_2_tpyramid_level<V,E,F,D>::tag ;

#endif // vpyr_2_tpyramid_level_txx_
