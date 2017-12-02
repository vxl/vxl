// This is prip/vpyr/vpyr_2_tpyramid_level.hxx
#ifndef vpyr_2_tpyramid_level_hxx_
#define vpyr_2_tpyramid_level_hxx_

#include "vpyr_2_tpyramid_level.h"

template <class V, class E, class F, class D>
vpyr_2_tpyramid_level<V,E,F,D>::vpyr_2_tpyramid_level(vmap_level_index arg_level,  pyramid_type & pyramid)
  : Base_(arg_level, (typename Base_::pyramid_type &)pyramid),
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
    int ld=this->nb_darts(),
        lv=nb_vertices(),
        le=nb_edges(), id ;

    typename contraction_kernel::const_iterator itk ;
    for (unsigned int i=0; i<arg_kernel.size(); ++i)//itk=arg_kernel.begin(); itk!=arg_kernel.end(); ++itk)
    {
#if 0
      dart_pointer d =*itk ;
#endif // 0

      id=dart_associated_edge(arg_kernel.get_dart_index(i)) ;
      edge_pointer e=get_edge_pointer(id) ;
#if 0
      vpyr_2_pyramid_base_ancestor((edge_pointer)d->get_edge_pointer(),index()) ;
#endif // 0
      e->set_last_level(level) ;
#if 0
      id=e->sequence_index() ;
#endif // 0
      --le ;
      this->edge_sequence::swap(id,le) ;

      id=arg_kernel.get_element_index(i) ;
      vertex_pointer v=get_vertex_pointer(id) ;
#if 0
      vpyr_2_pyramid_base_ancestor((vertex_pointer)d->get_vertex_pointer(),index()) ;
#endif // 0
      v->set_last_level(level) ;
#if 0
      id=v->sequence_index() ;
#endif // 0
      --lv ;
      vertex_sequence::swap(id,lv) ;
    }
    for (itk=arg_kernel.begin(); itk!=arg_kernel.end(); ++itk)
    {
      dart_pointer d =*itk ;
      d->set_last_level(level) ;
      id=d->sequence_index() ;
      --ld ;
      vpyr_2_pyramid_level<D>::dart_sequence::swap(id,ld) ;

      id=vmap_2_map_alpha(d)->sequence_index() ;
      --ld ;
      vpyr_2_pyramid_level<D>::dart_sequence::swap(id,ld) ;
    }
    vpyr_2_pyramid_level<D>::dart_sequence::resize(ld) ;
    this->edge_sequence::resize(le) ;
    this->vertex_sequence::resize(lv) ;
  }
}

template <class V, class E, class F, class D>
void vpyr_2_tpyramid_level<V,E,F,D>::removal(const removal_kernel &arg_kernel)
{
  self_type * l=pyramid().level_below(vmap_removal_type,*this) ;
  if (l!=NULL)
  {
    int level=l->index() ;
    int ld=this->nb_darts(),
        lv=nb_faces(),
        le=nb_edges(), id ;

    typename contraction_kernel::const_iterator itk ;
    for (unsigned int i=0; i<arg_kernel.size(); ++i)//itk=arg_kernel.begin(); itk!=arg_kernel.end(); ++itk)
    {
#if 0
      dart_pointer d =*itk ;
#endif // 0

      id=dart_associated_edge(arg_kernel.get_dart_index(i)) ;
      edge_pointer e=get_edge_pointer(id) ;
#if 0
      vpyr_2_pyramid_base_ancestor((edge_pointer)d->get_edge_pointer(),index()) ;
#endif // 0
      e->set_last_level(level) ;
#if 0
      id=e->sequence_index() ;
#endif // 0
      --le ;
      this->edge_sequence::swap(id,le) ;

      id=arg_kernel.get_element_index(i) ;
      face_pointer v=get_face_pointer(id) ;
#if 0
      vpyr_2_pyramid_base_ancestor((face_pointer)d->get_face_pointer(),index()) ;
#endif // 0
      v->set_last_level(level) ;
#if 0
      id=v->sequence_index() ;
#endif // 0
      --lv ;
      this->face_sequence::swap(id,lv) ;
    }
    for (itk=arg_kernel.begin(); itk!=arg_kernel.end(); ++itk)
    {
      dart_pointer d =*itk, ad=vmap_2_map_alpha(d) ;
      d->set_last_level(level) ;
      id=d->sequence_index() ;
      --ld ;
      vpyr_2_pyramid_level<D>::dart_sequence::swap(id,ld) ;

      id=ad->sequence_index() ;
      --ld ;
      vpyr_2_pyramid_level<D>::dart_sequence::swap(id,ld) ;
    }
    for (int i=le; i<nb_edges(); i++)
    {
      get_edge_pointer(i)->set_last_level(level) ;
    }
    for (int i=lv; i<nb_faces(); i++)
    {
      get_face_pointer(i)->set_last_level(level) ;
    }

    vpyr_2_pyramid_level<D>::dart_sequence::resize(ld) ;
    this->edge_sequence::resize(le) ;
    this->face_sequence::resize(lv) ;
  }
}

template <class V, class E, class F, class D>
vmap_2_tmap_tag vpyr_2_tpyramid_level<V,E,F,D>::tag ;

#endif // vpyr_2_tpyramid_level_hxx_
