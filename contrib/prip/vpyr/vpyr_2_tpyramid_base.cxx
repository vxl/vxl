// This is prip/vpyr/vpyr_2_tpyramid_base.cxx
#include "vpyr_2_tpyramid_base.h"

//////////////////////////////////////// inline

const vpyr_2_tpyramid_base_edge & vpyr_2_tpyramid_base_dart::edge(vmap_level_index level) const
{
  return *_BaseTuple::edge().ancestor(level) ;
}

const vpyr_2_tpyramid_base_vertex & vpyr_2_tpyramid_base_dart::vertex(vmap_level_index level) const
{
  return *_BaseTuple::vertex().ancestor(level) ;
}

const vpyr_2_tpyramid_base_face & vpyr_2_tpyramid_base_dart::face(vmap_level_index level) const
{
  return *_BaseTuple::face().ancestor(level) ;
}

vpyr_2_tpyramid_base_edge & vpyr_2_tpyramid_base_dart::edge(vmap_level_index level)
{
  return *_BaseTuple::edge().ancestor(level) ;
}

vpyr_2_tpyramid_base_vertex & vpyr_2_tpyramid_base_dart::vertex(vmap_level_index level)
{
  return *_BaseTuple::vertex().ancestor(level) ;
}

vpyr_2_tpyramid_base_face & vpyr_2_tpyramid_base_dart::face(vmap_level_index level)
{
  return *_BaseTuple::face().ancestor(level) ;
}

void vpyr_2_tpyramid_base_dart::set_sequence_index(vmap_dart_index arg)
{
   int offset =arg-sequence_index() ;

  vpyr_2_pyramid_base_dart::set_sequence_index(arg) ;

  vmap_2_tmap_dart_iterator it ;
  if (_edge!=NULL)
  {
    it = _edge->begin()+offset ;
    if (&*it==(vmap_2_tmap_dart*)this)
      _edge->set_begin(it) ;
  }
  if (_vertex!=NULL)
  {
    it = _vertex->begin()+offset ;
    if (&*it==(vmap_2_tmap_dart*)this)
      _vertex->set_begin(it) ;
  }
  if (_face!=NULL)
  {
    it = _face->begin()+offset ;
    if (&*it==(vmap_2_tmap_dart*)this)
      _face->set_begin(it) ;
  }
#if 0
  int offset =arg-sequence_index() ;

  if (_edge!=NULL && (vpyr_2_tpyramid_base_dart*)&*(((vmap_2_tmap_edge*)_edge)->begin()+offset)==this)
  {
    _edge->set_begin(((vmap_2_tmap_edge*)_edge)->begin()+offset) ;
  }
  if (_vertex!=NULL && (vpyr_2_tpyramid_base_dart*)&*(((vmap_2_tmap_vertex*)_vertex)->begin()+offset)==this)
  {
    _vertex->set_begin(((vmap_2_tmap_vertex*)_vertex)->begin()+offset) ;
  }
  if (_face!=NULL && (vpyr_2_tpyramid_base_dart*)&*(((vmap_2_tmap_face*)_face)->begin()+offset)==this)
  {
    _face->set_begin(((vmap_2_tmap_face*)_face)->begin()+offset) ;
  }
  vpyr_2_pyramid_base_dart::set_sequence_index(arg) ;
#endif // 0
}

////////////////

vpyr_2_tpyramid_base_dart_iterator vpyr_2_tpyramid_base_vertex::begin(vmap_level_index level)
{
  vpyr_2_tpyramid_base_dart* tmp=*begin().reference() ;
  return dart_iterator(begin().reference()+::offset(vpyr_2_pyramid_base_ancestor(tmp,level),tmp)) ;
}

vpyr_2_tpyramid_const_base_dart_iterator vpyr_2_tpyramid_base_vertex::begin(vmap_level_index level) const
{
  const vpyr_2_tpyramid_base_dart* tmp=*begin().reference() ;
  return const_dart_iterator(begin().reference()+::offset(vpyr_2_pyramid_base_ancestor(tmp,level),tmp)) ;
}

////////////////
vpyr_2_tpyramid_base_dart_iterator vpyr_2_tpyramid_base_face::begin(vmap_level_index level)
{
  vpyr_2_tpyramid_base_dart* tmp=*begin().reference() ;
  return dart_iterator(begin().reference()+::offset(vpyr_2_pyramid_base_ancestor(tmp,level),tmp)) ;
}

vpyr_2_tpyramid_const_base_dart_iterator vpyr_2_tpyramid_base_face::begin(vmap_level_index level) const
{
  const vpyr_2_tpyramid_base_dart* tmp=*begin().reference() ;
  return const_dart_iterator(begin().reference()+::offset(vpyr_2_pyramid_base_ancestor(tmp,level),tmp)) ;
}

////////////////

bool vpyr_2_tpyramid_base_edge::is_self_loop (vmap_level_index level) const
{
  return &first_vertex(level)==&last_vertex(level) ;
}

bool vpyr_2_tpyramid_base_edge::is_empty_self_loop (vmap_level_index level) const
{
  return direct_dart(level).is_self_direct_loop(level) ||
         inverse_dart(level).is_self_direct_loop(level) ;
}

bool vpyr_2_tpyramid_base_edge::is_isthmus (vmap_level_index level) const
{
  return &left_face(level)==&right_face(level);
}

bool vpyr_2_tpyramid_base_edge::is_pendant (vmap_level_index level) const
{
  return direct_dart(level).is_pendant(level) ||
         inverse_dart(level).is_pendant(level) ;
}

bool vpyr_2_tpyramid_base_edge::is_isolated_self_loop (vmap_level_index level) const
{
  return direct_dart(level).is_self_direct_loop(level) &&
         inverse_dart(level).is_self_direct_loop(level) ;
}

const vpyr_2_tpyramid_base_vertex & vpyr_2_tpyramid_base_edge::first_vertex(vmap_level_index level) const
{
  return direct_dart(level).vertex(level) ;
}

const vpyr_2_tpyramid_base_vertex & vpyr_2_tpyramid_base_edge::last_vertex(vmap_level_index level) const
{
  return inverse_dart(level).vertex(level) ;
}

const vpyr_2_tpyramid_base_face & vpyr_2_tpyramid_base_edge::right_face(vmap_level_index level) const
{
  return direct_dart(level).face(level) ;
}

const vpyr_2_tpyramid_base_face & vpyr_2_tpyramid_base_edge::left_face(vmap_level_index level) const
{
  return inverse_dart(level).face(level) ;
}

bool vpyr_2_tpyramid_base_edge::is_adjacent_to (const vpyr_2_tpyramid_base_face & arg,vmap_level_index level) const
{
  return &left_face(level)== &arg || &right_face(level)==&arg ;
}

const vpyr_2_tpyramid_base_dart & vpyr_2_tpyramid_base_edge::direct_dart(vmap_level_index level) const
{
  return ( const vpyr_2_tpyramid_base_dart &) *vmap_2_tmap_edge::begin() ;
}

const vpyr_2_tpyramid_base_dart & vpyr_2_tpyramid_base_edge::inverse_dart(vmap_level_index level) const
{
  return ( const vpyr_2_tpyramid_base_dart &) *direct_dart(level).alpha(level) ;
}

vpyr_2_tpyramid_base_dart_iterator vpyr_2_tpyramid_base_edge::begin(vmap_level_index level)
{
  vpyr_2_tpyramid_base_dart* tmp=*begin().reference() ;
  return dart_iterator(begin().reference()+::offset(vpyr_2_pyramid_base_ancestor(tmp,level),tmp)) ;
}

vpyr_2_tpyramid_const_base_dart_iterator vpyr_2_tpyramid_base_edge::begin(vmap_level_index level) const
{
  const vpyr_2_tpyramid_base_dart* tmp=*begin().reference() ;
  return const_dart_iterator(begin().reference()+::offset(vpyr_2_pyramid_base_ancestor(tmp,level),tmp)) ;
}

vmap_level_index vpyr_2_tpyramid_base_edge::last_level() const
{
  return ((const vpyr_2_tpyramid_base_dart&)vmap_2_tmap_edge::direct_dart()).last_level() ;
}

#if 0
    void vpyr_2_tpyramid_base_edge::set_last_level(vmap_level_index arg)
    {
      ((vpyr_2_tpyramid_base_dart&)vmap_2_tmap_edge::direct_dart()).set_last_level(arg) ;
    }
#endif // 0

////////////////////////////////////////

const vpyr_2_tpyramid_base_vertex* vpyr_2_tpyramid_base_vertex::ancestor(vmap_level_index level) const
{
  vpyr_2_tpyramid_base_vertex& d=const_cast<vpyr_2_tpyramid_base_vertex&> (*this);
  return d.ancestor(level) ;
}

vpyr_2_tpyramid_base_vertex* vpyr_2_tpyramid_base_vertex::ancestor(vmap_level_index level)
{
  vpyr_2_tpyramid_base_vertex* v=this ;
  vmap_level_index l=v->last_level() ;
  while (l<level && vmap_is_contraction_type(l)) // if
  {
    vpyr_2_tpyramid_base_dart* d=*v->begin(l).reference(); //, end=d ;
    // we find the adjacent contracted dart leading to the father
    do
    {
      d=vpyr_2_pyramid_base_sigma(d,l) ;
    }
    while (!d->modified_at_level(l)); // d!=end) ; there is a father !
    v=&vpyr_2_pyramid_base_alpha(d,l)->vertex(l) ;
    l=v->last_level() ;
  }
  return v ;
}

int vpyr_2_tpyramid_base_vertex::degree(vmap_level_index l) const
{
  const vpyr_2_tpyramid_base_dart* d=*begin(l).reference(), *end=d ;
  int count=0;
  do
  {
    count ++ ;
    d=vpyr_2_pyramid_base_sigma(d,l) ;
  }
  while (d!=end) ;
  return count ;
}

const vpyr_2_tpyramid_base_edge* vpyr_2_tpyramid_base_edge::ancestor(vmap_level_index level) const
{
  vpyr_2_tpyramid_base_edge& d=const_cast<vpyr_2_tpyramid_base_edge&> (*this);
  return d.ancestor(level) ;
}

vpyr_2_tpyramid_base_edge* vpyr_2_tpyramid_base_edge::ancestor(vmap_level_index level)
{
  vpyr_2_tpyramid_base_edge* v=this ;
  return v ;
}

const vpyr_2_tpyramid_base_face* vpyr_2_tpyramid_base_face::ancestor(vmap_level_index level) const
{
  vpyr_2_tpyramid_base_face& d=const_cast<vpyr_2_tpyramid_base_face&> (*this);
  return d.ancestor(level) ;
}

vpyr_2_tpyramid_base_face* vpyr_2_tpyramid_base_face::ancestor(vmap_level_index level)
{
  vpyr_2_tpyramid_base_face* v=this ;
  vmap_level_index l=v->last_level() ;
  while (l<level && vmap_is_removal_type(l)) // if
  {
    vpyr_2_tpyramid_base_dart* d=*v->begin(l).reference(); //, end=d ;
    // we find the adjacent contracted dart leading to the father
    do
    {
      d=vpyr_2_pyramid_base_phi(d,l) ;
    }
    while (!d->modified_at_level(l)); // d!=end) ; there is a father !
    v=&vpyr_2_pyramid_base_alpha(d,l)->face(l) ;
    l=v->last_level() ;
  }
  return v ;
}

int vpyr_2_tpyramid_base_face::degree(vmap_level_index l) const
{
  const vpyr_2_tpyramid_base_dart* d=*begin(l).reference(), *end=d ;
  int count=0;
  do
  {
    count ++ ;
    d=vpyr_2_pyramid_base_phi(d,l) ;
  }
  while (d!=end) ;
  return count ;
}
