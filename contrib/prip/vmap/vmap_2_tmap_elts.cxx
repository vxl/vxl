// This is prip/vmap/vmap_2_tmap_elts.cxx
#include "vmap_2_tmap_elts.h"

void vmap_2_tmap_dart::set_sequence_index(vmap_dart_index arg)
{
  int offset =arg-sequence_index() ;
  vmap_2_map_dart::set_sequence_index(arg) ;

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
 }

int vmap_2_tmap_face::degree() const
{
  dart_iterator pt=_begin, end=pt ;
  int count=0;
  do
  {
    count ++ ;
    pt.phi() ;
  }
  while (pt!=end) ;
  return count ;
}

int vmap_2_tmap_vertex::degree() const
{
  const_dart_iterator pt=begin(), end=pt ;
  int count=0;
  do
  {
    count ++ ;
    pt.sigma() ;
  }
  while (pt!=end) ;
  return count ;
}

