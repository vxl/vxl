// This is prip/vmap/vmap_2_tmap_elts.cxx
#include "vmap_2_tmap_elts.h"

void vmap_2_tmap_dart::set_sequence_index(vmap_dart_index arg)
{
  int offset =arg-sequence_index() ;
  vmap_2_map_dart::set_sequence_index(arg) ;

  vmap_2_tmap_dart_iterator it ;
  if (edge_!=NULL)
  {
    it = edge_->begin()+offset ;
    if (&*it==(vmap_2_tmap_dart*)this)
    {
      edge_->set_begin(it) ;
    }
  }
  if (vertex_!=NULL)
  {
    it = vertex_->begin()+offset ;
    if (&*it==(vmap_2_tmap_dart*)this)
    {
      vertex_->set_begin(it) ;
    }
  }
  if (face_!=NULL)
  {
    it = face_->begin()+offset ;
    if (&*it==(vmap_2_tmap_dart*)this)
    {
      face_->set_begin(it) ;
    }
  }
 }

int vmap_2_tmap_face::degree() const
{
  dart_iterator pt=begin_, end=pt ;
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
