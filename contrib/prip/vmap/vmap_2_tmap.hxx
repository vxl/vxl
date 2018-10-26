// This is prip/vmap/vmap_2_tmap.hxx
#ifndef vmap_2_tmap_hxx_
#define vmap_2_tmap_hxx_

#include <iostream>
#include "vmap_2_tmap.h"
#include <vcl_cassert.h>
#include <vcl_compiler.h>

template <class V, class E, class F, class D>
vmap_2_tmap< V,E,F,D >::vmap_2_tmap(self_type const& tmap)
  : Base_(tmap), vertex_sequence(tmap), edge_sequence(tmap), face_sequence(tmap)
{
  operator=(tmap) ;
}

template <class V, class E, class F, class D>
void vmap_2_tmap< V,E,F,D >::set_vertex (vmap_dart_index arg, vmap_vertex_index arg_vertex)
{
  get_vertex_pointer(arg_vertex)->set_begin(this->begin_dart()+arg) ;
  this->dart(arg).set_vertex(get_vertex_pointer(arg_vertex)) ;
}

template <class V, class E, class F, class D>
void vmap_2_tmap< V,E,F,D >::set_edge(vmap_dart_index arg, vmap_edge_index arg_edge)
{
  get_edge_pointer(arg_edge)->set_begin(this->begin_dart()+arg) ;
  this->dart(arg).set_edge(get_edge_pointer(arg_edge)) ;
}

template <class V, class E, class F, class D>
void vmap_2_tmap< V,E,F,D >::set_face(vmap_dart_index arg, vmap_face_index arg_face)
{
  get_face_pointer(arg_face)->set_begin(this->begin_dart()+arg) ;
  this->dart(arg).set_face(get_face_pointer(arg_face)) ;
}

template <class V, class E, class F, class D>
void vmap_2_tmap< V,E,F,D >::set_all_cycles()
{
  set_vertex_cycles() ;
  set_edge_cycles() ;
  set_face_cycles() ;
}

template <class V, class E, class F, class D>
void vmap_2_tmap< V,E,F,D >::clear()
{
  Base_::clear() ;
  vertex_sequence::clear() ;
  edge_sequence::clear() ;
  face_sequence::clear() ;
}

template <class V, class E, class F, class D>
void vmap_2_tmap< V,E,F,D >::initialise(int arg_nb_vertices, int arg_nb_edges, int arg_nb_faces)
{
  this->initialise_darts(2*arg_nb_edges) ;
  initialise_vertices(arg_nb_vertices) ;
  initialise_edges(arg_nb_edges) ;
  initialise_faces(arg_nb_faces) ;
}

template <class V, class E, class F, class D>
void vmap_2_tmap< V,E,F,D >::initialise_edges(int arg_nb_edges)
{
  edge_sequence::resize(arg_nb_edges) ;
}

template <class V, class E, class F, class D>
void vmap_2_tmap< V,E,F,D >::initialise_vertices(int arg_nb_vertices)
{
  vertex_sequence::resize(arg_nb_vertices) ;
}

template <class V, class E, class F, class D>
void vmap_2_tmap< V,E,F,D >::initialise_faces(int arg_nb_faces)
{
  face_sequence::resize(arg_nb_faces) ;
}

template <class V, class E, class F, class D>
vmap_2_tmap< V,E,F,D >::~ vmap_2_tmap() = default;

template <class V, class E, class F, class D>
vmap_2_tmap< V,E,F,D > & vmap_2_tmap< V,E,F,D >::operator=(self_type const& tmap)
{
  if (&tmap!=this)
  {
    Base_::operator=(tmap) ;
    vertex_sequence::operator=(tmap) ;
    edge_sequence::operator=(tmap) ;
    face_sequence::operator=(tmap) ;
    for (int i=0; i<this->nb_darts(); i++)
    {
      set_dart(i,tmap.sigma(i), tmap.alpha(i), tmap.dart_associated_vertex(i),
               tmap.dart_associated_edge(i),tmap.dart_associated_face(i)) ;
    }
    for (int i=0; i<nb_vertices(); i++)
    {
      vertex(i).set_begin(this->begin_dart()+tmap.vertex_associated_dart(i)) ;
    }
    for (int i=0; i<nb_edges(); i++)
    {
      edge(i).set_begin(this->begin_dart()+tmap.edge_associated_dart(i)) ;
    }
    for (int i=0; i<nb_faces(); i++)
    {
      face(i).set_begin(this->begin_dart()+tmap.face_associated_dart(i)) ;
    }
  }
  return *this ;
}

template <class V, class E, class F, class D>
void vmap_2_tmap< V,E,F,D >::set_dart(vmap_dart_index arg, vmap_dart_index arg_sigma, vmap_dart_index arg_alpha,
                                      vmap_vertex_index arg_vertex, vmap_edge_index arg_edge, vmap_face_index arg_face)
{
  this->set_sigma(arg, arg_sigma) ;
  this->set_alpha(arg, arg_alpha) ;
  set_vertex(arg, arg_vertex);
  set_edge(arg, arg_edge);
  set_face(arg, arg_face);
}

template <class V, class E, class F, class D>
void vmap_2_tmap< V,E,F,D >::set_dart(vmap_dart_index arg, vmap_dart_index arg_sigma,
                                      vmap_vertex_index arg_vertex, vmap_face_index arg_face)
{
  set_vertex(arg, arg_vertex);
  set_face(arg, arg_face);
  this->set_sigma(arg, arg_sigma) ;
}

template <class V, class E, class F, class D>
void vmap_2_tmap< V,E,F,D >::set_vertex_cycles()
{
  dart_iterator d ;
  for (d=this->begin_dart(); d!=this->end_dart(); ++d)
  {
    d->set_vertex(NULL) ;
  }
  vmap_vertex_index count_vertex =0 ;
  for (d=this->begin_dart(); d!=this->end_dart(); ++d)
  {
    if (d->get_vertex_pointer() == NULL)
    {
      dart_iterator end=d ;
      do
      {
        d->set_vertex((vmap_2_tmap_vertex*)0xFFFFFFFF) ;
        d.sigma() ;
      }
      while (d!=end) ;
      count_vertex++ ;
    }
  }
  initialise_vertices(count_vertex) ;
#if 0
  for (i=0;i<this->nb_darts();++i)
  {
      this->dart(i).set_vertex(NULL) ;
  }
#endif // 0
  count_vertex =0 ;
  for (d=this->begin_dart(); d!=this->end_dart(); ++d)
  {
    if (d->get_vertex_pointer() == (vmap_2_tmap_vertex*)0xFFFFFFFF)
    {
      vertex(count_vertex).set_begin(d) ;
      dart_iterator end=d ;
      do
      {
        d->set_vertex(get_vertex_pointer(count_vertex)) ;
        d.sigma() ;
      }
      while (d!=end) ;
      count_vertex++ ;
    }
  }
}

template <class V, class E, class F, class D>
void vmap_2_tmap< V,E,F,D >::set_edge_cycles()
{
  initialise_edges(this->nb_darts()/2) ;
  dart_iterator d ;
  for (d=this->begin_dart(); d!=this->end_dart(); ++d)
  {
    d->set_edge(nullptr) ;
  }
  int count_vertex =0 ;
  for (d=this->begin_dart(); d!=this->end_dart(); ++d)
  {
    if (d->get_edge_pointer() == (vmap_2_tmap_edge*)nullptr)
    {
      edge(count_vertex).set_begin(d) ;
      d->set_edge(get_edge_pointer(count_vertex)) ;
      d.alpha() ;
      d->set_edge(get_edge_pointer(count_vertex)) ;
      d.alpha() ;
      count_vertex++ ;
    }
  }
}

template <class V, class E, class F, class D>
void vmap_2_tmap< V,E,F,D >::set_face_cycles()
{
  dart_iterator d ;
  for (d=this->begin_dart(); d!=this->end_dart(); ++d)
  {
    d->set_face(nullptr) ;
  }
  vmap_face_index count_vertex =0 ;
  for (d=this->begin_dart(); d!=this->end_dart(); ++d)
  {
    if (d->get_face_pointer() == nullptr)
    {
      dart_iterator end=d ;
      do
      {
        d->set_face((vmap_2_tmap_face*)0xFFFFFFFF) ;
        d.phi() ;
      }
      while (d!=end) ;
      count_vertex++ ;
    }
  }
  initialise_faces(count_vertex) ;
#if 0
  for (i=0;i<this->nb_darts();++i)
  {
    this->dart(i).set_vertex(NULL) ;
  }
#endif // 0
  count_vertex =0 ;
  for (d=this->begin_dart(); d!=this->end_dart(); ++d)
  {
    if (d->get_face_pointer() == (vmap_2_tmap_face*)0xFFFFFFFF)
    {
      face(count_vertex).set_begin(d) ;
      dart_iterator end=d ;
      do
      {
        d->set_face(get_face_pointer(count_vertex)) ;
        d.phi() ;
      }
      while (d!=end) ;
      count_vertex++ ;
    }
  }
}

template <class V, class E, class F, class D>
bool vmap_2_tmap< V,E,F,D >::valid_permutations() const
{
  const_dart_iterator d ;
  for (d=this->begin_dart(); d!=this->end_dart(); ++d)
  {
    const_dart_iterator  end=d ;
    do
    {
      assert (d->get_vertex_pointer() == end->get_vertex_pointer()) ;
      d.sigma() ;
    }
    while (d!=end) ;
    do
    {
      assert(d->get_edge_pointer() == end->get_edge_pointer()) ;
      d.alpha() ;
    }
    while (d!=end) ;
    do
    {
      assert(d->get_face_pointer() == end->get_face_pointer()) ;
      d.phi() ;
    }
    while (d!=end) ;
  }
  return true ;
}

template <class V, class E, class F, class D>
bool vmap_2_tmap< V,E,F,D >::valid() const
{
  if (!Base_::valid()) return false ;
  for (int i=0; i<this->nb_darts(); i++)
  {
    if (!this->dart(i).valid()) return false ;
  }
  return valid_permutations() ;
}

template <class V, class E, class F, class D>
void vmap_2_tmap< V,E,F,D >::read_structure(std::istream & stream)
{
  int tmp_vertex,tmp_edge,tmp_face ;
  stream>>tmp_vertex>>tmp_edge>>tmp_face ;
  initialise(tmp_vertex,tmp_edge,tmp_face) ;
  set_edge_cycles() ;
  for (int i=0; i<this->nb_darts(); i++)
  {
    stream>>tmp_edge >>tmp_vertex >>tmp_face ;
    set_dart(i,tmp_edge,tmp_vertex,tmp_face) ;
  }
}

template <class V, class E, class F, class D>
void vmap_2_tmap< V,E,F,D >::write_structure(std::ostream & stream) const
{
  stream<<nb_vertices()<<' '<<nb_edges()<<' '<<nb_faces()<<std::endl ;
  for (int i=0; i<this->nb_darts(); i++)
  {
    stream<<this->sigma(i)<<' '
          <<dart_associated_vertex(i)<<' '
          <<dart_associated_face(i)<<"   " ;
  }
  stream<<std::endl ;
}

template <class V, class E, class F, class D>
void vmap_2_tmap< V,E,F,D >::removal(dart_iterator & arg)
{
  vmap_2_tmap_dart * d = &*arg,
                   * a = vmap_2_map_alpha(d) ;
  vmap_2_tmap_face * v = d->get_face_pointer(),
                   * ov = a->get_face_pointer() ;
  if (v!=ov)
  {
    dart_iterator i=arg ; i.phi() ;
    while (i!=arg)
    {
      i->set_face(v) ;
      i.phi() ;
    }
    if (ov->begin() == arg)
    {
      i.phi() ;
      ov->set_begin(i) ;
    }
    if (d->vertex().begin() == arg)
    {
      i=arg ; i.phi() ;
      d->vertex().set_begin(i) ;
    }
    if (a->vertex().begin() == arg)
    {
      i=arg ; i.alpha(); i.phi() ;
      a->vertex().set_begin(i) ;
    }
    edge_type * e=d->get_edge_pointer() ;
    this->base_.unchecked_removal(arg) ;
    face_sequence::remove(v->sequence_index()) ;
    edge_sequence::remove(e->sequence_index()) ;
  }
}

template <class V, class E, class F, class D>
void vmap_2_tmap< V,E,F,D >::contraction(dart_iterator & arg)
{
  vmap_2_tmap_dart * d = &*arg,
                   * a = vmap_2_map_alpha(d) ;
  vmap_2_tmap_vertex * v = d->get_vertex_pointer(),
                     * ov = a->get_vertex_pointer() ;
  if (v!=ov)
  {
    dart_iterator i=arg ; i.sigma() ;
    while (i!=arg)
    {
      i->set_vertex(ov) ;
      i.sigma() ;
    }
    if (ov->begin() == arg)
    {
      i.sigma() ;
      ov->set_begin(i) ;
    }
    if (d->face().begin() == arg)
    {
      i=arg ; i.phi() ;
      d->face().set_begin(i) ;
    }
    if (a->face().begin() == arg)
    {
      i=arg ; i.alpha(); i.phi() ;
      a->face().set_begin(i) ;
    }
    edge_type * e=d->get_edge_pointer() ;
    this->base_.unchecked_contraction(arg) ;
    vertex_sequence::remove(v->sequence_index()) ;
    edge_sequence::remove(e->sequence_index()) ;
  }
}

template <class V, class E, class F, class D>
void vmap_2_tmap< V,E,F,D >::contraction(contraction_kernel const& arg_kernel)
{
  int ld=this->nb_darts(),
      lv=nb_vertices(),
      le=nb_edges(), id ;

  typename contraction_kernel::const_iterator itk ;
  for (itk=arg_kernel.begin(); itk!=arg_kernel.end(); ++itk)
  {
    dart_pointer d =*itk ;
#if 0
    this->get_dart_pointer((*itk)->sequence_index()) ;
#endif // 0
    id=d->sequence_index() ;
    --ld ;
    vmap_2_map<D>::dart_sequence::swap(id,ld) ;

    id=vmap_2_map_alpha(d)->sequence_index() ;
    --ld ;
    vmap_2_map<D>::dart_sequence::swap(id,ld) ;
  }

  for (id=0; id<ld ; ++id )
  {
    vmap_2_tmap_dart* d=(vmap_2_tmap_dart*)this->get_dart_pointer(id) ;
    d->set_vertex(get_vertex_pointer(arg_kernel.father_index(d->vertex().sequence_index()))) ;

    if (d->vertex().begin()->sequence_index()>=ld)
    {
      d->vertex().set_begin(dart_iterator(&this->get_dart_pointer(id))) ;
    }

    if (d->face().begin()->sequence_index()>=ld)
    {
      d->face().set_begin(dart_iterator(&this->get_dart_pointer(id))) ;
    }

    vmap_2_tmap_dart* sd=vmap_2_map_sigma(d) ;
    if (sd->sequence_index()>=ld)
    {
      vmap_2_tmap_dart* ssd=sd ;
      while (ssd->sequence_index()>=ld)
      {
        ssd=vmap_2_map_phi(ssd) ;
      }
      d->set_sigma(ssd) ;
    }
  }

  for (itk=arg_kernel.begin(); itk!=arg_kernel.end(); ++itk)
  {
    dart_pointer d =*itk ;
#if 0
    this->get_dart_pointer((*itk)->sequence_index()) ;
#endif // 0

    id=d->get_edge_pointer()->sequence_index() ;
    --le ;
    edge_sequence::swap(id,le) ;

    id=d->get_vertex_pointer()->sequence_index() ;
    --lv ;
    vertex_sequence::swap(id,lv) ;
  }

  for (id=this->nb_darts()-1; id>=ld ; id-- )
  {
#if 0
    vmap_2_tmap_dart* d=(vmap_2_tmap_dart*)this->get_dart_pointer(id), *bv=&*d->vertex().begin(), *bf=&*d->face().begin() ;

    if (d==bv || d==bf)
    {
      vmap_2_tmap_dart* ssd=vmap_2_map_phi(d) ;
      while (ssd->sequence_index()>=ld) ssd=vmap_2_map_phi(ssd) ;
      dart_iterator it(&this->get_dart_pointer(ssd->sequence_index())) ;
      if (d==bv)
      {
        d->vertex().set_begin(it) ;
      }
      if (d==bf)
      {
        d->face().set_begin(it) ;
      }
    }
#endif // 0
    vmap_2_map<D>::dart_sequence::pop_back() ;
  }

  for (id=nb_edges(); id>le ; id-- )
  {
    this->edge_sequence::pop_back() ;
  }
  for (id=nb_vertices(); id>lv ; id-- )
  {
    this->vertex_sequence::pop_back() ;
  }
}

template <class V, class E, class F, class D>
void vmap_2_tmap< V,E,F,D >::removal(removal_kernel const& arg_kernel)
{
  int ld=this->nb_darts(),
      lv=nb_faces(),
      le=nb_edges(), id ;

  typename contraction_kernel::const_iterator itk ;
  for (itk=arg_kernel.begin(); itk!=arg_kernel.end(); ++itk)
  {
    dart_pointer d =*itk;
#if 0
    this->get_dart_pointer((*itk)->sequence_index()) ;
#endif // 0
    id=d->sequence_index() ;
    --ld ;
    vmap_2_map<D>::dart_sequence::swap(id,ld) ;

    id=vmap_2_map_alpha(d)->sequence_index() ;
    --ld ;
    vmap_2_map<D>::dart_sequence::swap(id,ld) ;
  }

  for (id=0; id<ld ; ++id )
  {
    vmap_2_tmap_dart* d=(vmap_2_tmap_dart*)this->get_dart_pointer(id) ;
    d->set_face(get_face_pointer(arg_kernel.father_index(d->face().sequence_index()))) ;
    if (d->vertex().begin()->sequence_index()>=ld)
    {
      d->vertex().set_begin(dart_iterator(&this->get_dart_pointer(id))) ;
    }
    if (d->face().begin()->sequence_index()>=ld)
    {
      d->face().set_begin(dart_iterator(&this->get_dart_pointer(id))) ;
    }

    vmap_2_tmap_dart* sd=vmap_2_map_sigma(d) ;
    if (sd->sequence_index()>=ld)
    {
      vmap_2_tmap_dart* ssd=sd ;
      while (ssd->sequence_index()>=ld)
      {
        ssd=vmap_2_map_sigma(ssd) ;
      }
      d->set_sigma(ssd) ;
    }
  }
  for (itk=arg_kernel.begin(); itk!=arg_kernel.end(); ++itk)
  {
    dart_pointer d =*itk;
#if 0
    this->get_dart_pointer((*itk)->sequence_index()) ;
#endif // 0
    id=d->get_edge_pointer()->sequence_index() ;
    --le ;
    edge_sequence::swap(id,le) ;

    id=d->get_face_pointer()->sequence_index() ;
    --lv ;
    face_sequence::swap(id,lv) ;
  }

  for (id=this->nb_darts()-1; id>=ld ; id-- )
  {
#if 0
    vmap_2_tmap_dart* d=(vmap_2_tmap_dart*)this->get_dart_pointer(id), *bv=&*d->vertex().begin(), *bf=&*d->face().begin() ;

    if (d==bv || d==bf)
    {
      vmap_2_tmap_dart* ssd=vmap_2_map_sigma(d) ;
      while (ssd->sequence_index()>=ld) ssd=vmap_2_map_sigma(ssd) ;
      dart_iterator it(&this->get_dart_pointer(ssd->sequence_index())) ;
      if (d==bv)
      {
        d->vertex().set_begin(it) ;
      }
      if (d==bf)
      {
        d->face().set_begin(it) ;
      }
    }
#endif // 0
    vmap_2_map<D>::dart_sequence::pop_back() ;
  }

  for (id=nb_edges(); id>le ; id-- )
  {
    this->edge_sequence::pop_back() ;
  }

  for (id=nb_faces(); id>lv ; id-- )
  {
    this->face_sequence::pop_back() ;
  }
}

template <class V, class E, class F, class D>
vmap_2_tmap_tag vmap_2_tmap<V,E,F,D>::tag ;

#if 0 // method commented out
template <class V, class E, class F, class D>
typename vmap_2_tmap< V,E,F,D >::edge_iterator
vmap_2_tmap< V,E,F,D >::find_edge(vertex_type const& arg1,vertex_type const& arg2)
{
  const_dart_iterator dart=arg1.begin(), end=dart ;
  do
  {
    dart.sigma() ;
  } while (dart!=end &&
           &dart->edge().first_vertex()!=&arg2 &&
           &dart->edge().last_vertex()!=&arg2) ;
  if ((&dart->edge().first_vertex() == &arg2)|| (&dart->edge().last_vertex() == &arg2))
  {
    return (get_edge_pointer.begin()+index(cast(dart->edge())));
  }
  return end_edge() ;
}
#endif // 0

#if 0 // method commented out
template <class V, class E, class F, class D>
void  vmap_2_map< V,E,F,D >::set_edge(vmap_edge_index arg,
                                      vmap_edge_index arg_edge1, vmap_vertex_index arg_vertex1, vmap_face_index arg_face1,
                                      vmap_edge_index arg_edge2, vmap_vertex_index arg_vertex2, vmap_face_index arg_face2)
{
  vmap_dart_index tmp1 = edge_first_dart(arg_edge1),
                  tmp2 = edge_first_dart(arg_edge2),
                  tmp12 = edge_first_dart(arg),
                  tmp22 = alpha(tmp12);
  if (dart_associated_vertex(tmp1)!=arg_vertex1)
  {
    tmp1=alpha(tmp1) ;
  }

  if (dart_associated_vertex(tmp2)!=arg_vertex2)
  {
    tmp1=alpha(tmp2) ;
  }

  set_dart(tmp12,tmp1, arg_vertex1, arg_face1) ;
  set_dart(tmp22,tmp2, arg_vertex2, arg_face2) ;
}
#endif // 0

#if 0 // method commented out
template <class V, class E, class F, class D>
typename vmap_2_tmap< V,E,F,D >::edge_iterator
vmap_2_tmap< V,E,F,D >::find_edge(vertex_type const& arg1,vertex_type const& arg2)
{
  const_dart_iterator dart=arg1.begin(), end=dart ;
  do
  {
    dart.sigma() ;
  } while (dart!=end &&
           &dart->edge().first_vertex()!=&arg2 &&
           &dart->edge().last_vertex()!=&arg2) ;
  if ((&dart->edge().first_vertex() == &arg2)|| (&dart->edge().last_vertex() == &arg2))
  {
    return (get_edge_pointer.begin()+index(cast(dart->edge())));
  }
  return end_edge() ;
}
#endif // 0

#endif // vmap_2_tmap_hxx_
