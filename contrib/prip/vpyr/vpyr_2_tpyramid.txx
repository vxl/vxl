// This is prip/vpyr/vpyr_2_tpyramid.txx
#ifndef vpyr_2_tpyramid_txx_
#define vpyr_2_tpyramid_txx_

#include "vpyr_2_tpyramid.h"
#include <vmap/vbl_controlled_partition.h>
#include <vcl_iostream.h>

template <class TLevel>
vpyr_2_tpyramid<TLevel>::vpyr_2_tpyramid()
{
}

#if 0
template <class TLevel>
vpyr_2_tpyramid<TLevel>::vpyr_2_tpyramid(const self_type &pyr)
{
  operator=(pyr) ;
}
#endif // 0

template <class TLevel>
vpyr_2_tpyramid<TLevel>::~vpyr_2_tpyramid()
{
}

#if 0
template <class TLevel>
vpyr_2_tpyramid<TLevel> & vpyr_2_tpyramid<TLevel>::operator=(const self_type &pyr)
{
  if (this!=&rigth)
  {
    Base_::operator=(pyr) ;
    vertex_sequence_iterator first_vertex=this->base_map().begin_vertex_sequence(),
                             last_vertex=this->base_map().end_vertex_sequence();
    edge_sequence_iterator firstEdge=this->base_map().begin_edge_sequence(),
                           lastEdge=this->base_map().end_edge_sequence();
    face_sequence_iterator firstFace=this->base_map().begin_face_sequence(),
                           lastFace=this->base_map().end_face_sequence();
    for (int i=0; i<level_.size(); ++i)
    {
#if 0
      first_vertex=&this->base_map().vertex((*pyr.level_[i].begin_base_vertex())->index()) ;
#endif // 0
      last_vertex=this->base_map().begin_vertex_sequence()+(pyr.level(i).end_base_vertex()-pyr.base_map().begin_vertex_sequence()) ;
      level(i).set_vertex_sequence(first_vertex,last_vertex) ;
#if 0
      firstEdge=&this->base_map().edge((*pyr.level_[i].beginbase_edge())->index()) ;
#endif // 0
      lastEdge=this->base_map().begin_edge_sequence()+(pyr.level(i).endbase_edge()-pyr.base_map().begin_edge_sequence()) ;
      level(i).set_edge_sequence(firstEdge,lastEdge) ;
#if 0
      firstFace=&this->base_map().face((*pyr.level_[i].beginbase_type())->index()) ;
#endif // 0
      lastFace=this->base_map().begin_face_sequence()+(pyr.level(i).endbase_type()-pyr.base_map().begin_face_sequence()) ;
      level(i).set_face_sequence(firstFace,lastFace) ;
    }
  }
  return *this ;
}
#endif // 0

template <class TLevel>
bool vpyr_2_tpyramid<TLevel>::valid()
{
  return false;
}

template <class TLevel>
void vpyr_2_tpyramid<TLevel>::clear()
{
  Base_::clear();
}

template <class TLevel>
void vpyr_2_tpyramid<TLevel>::read_structure(vcl_istream &)
{
  vcl_cerr << "vpyr_2_tpyramid<TLevel>::read_structure NYI\n";
}

template <class TLevel>
void vpyr_2_tpyramid<TLevel>::write_structure(vcl_ostream &) const
{
  vcl_cerr << "vpyr_2_tpyramid<TLevel>::write_structure NYI\n";
}

template <class TLevel>
void vpyr_2_tpyramid<TLevel>::down_projection_vertices(vmap_level_index arg_level,vcl_vector<vmap_vertex_index> & res) const
{
  vbl_controlled_partition h ;
  vmap_vertex_index v ;
  h.initialise(this->base_map().nb_vertices()) ;
  res.resize(this->base_map().nb_vertices()) ;
  for (v=0; v<this->base_map().nb_vertices(); v++)
  {
    const base_vertex_type & rv=this->base_map().vertex(v) ;
    if (rv.last_level()<arg_level)
    {
      vmap_2_map_dart_base_iterator d=rv.begin(), end=d ;
      do
      {
        vmap_level_index l=d->last_level() ;
        if (l<arg_level && vmap_is_contraction_type(l) && d->modified_at_last_level())
        {
          vmap_2_map_dart_base_iterator ad=d ; ad.alpha() ;
          vmap_vertex_index av=h.representative(ad->vertex().sequence_index()),
                            representativev=h.representative(v) ;
          if (this->base_map().vertex(representativev).last_level()>this->base_map().vertex(av).last_level())
          {
            h.union_of(representativev,av) ;
          }
          else
          {
            h.union_of(av,representativev) ;
          }
        }
        d.sigma() ;
      } while (d!=end) ;
    }
  }
  for (v=0; v<this->base_map().nb_vertices(); v++)
  {
    res[v]=h.representative(v) ;
  }
}

template <class TLevel>
void vpyr_2_tpyramid<TLevel>::down_projection_faces(vmap_level_index arg_level,vcl_vector<vmap_face_index> & res) const
{
  vbl_controlled_partition h ;
  vmap_face_index v ;
  h.initialise(this->base_map().nb_faces()) ;
  res.resize(this->base_map().nb_faces()) ;
  for (v=0; v<this->base_map().nb_faces(); v++)
  {
    const base_face_type & rv=this->base_map().face(v) ;
    if (rv.last_level()<arg_level)
    {
      vmap_2_map_dart_base_iterator d=rv.begin(), end=d ;
      do
      {
        vmap_level_index l=d->last_level() ;
        if (l<arg_level && vmap_is_removal_type(l) && d->modified_at_last_level())
        {
          vmap_2_map_dart_base_iterator ad=d ; ad.alpha() ;
          vmap_face_index av=h.representative(ad->face().sequence_index()),
                          representativev=h.representative(v) ;
          if (this->base_map().face(representativev).last_level()>this->base_map().face(av).last_level())
          {
            h.union_of(representativev,av) ;
          }
          else
          {
            h.union_of(av,representativev) ;
          }
        }
        d.phi() ;
      } while (d!=end) ;
    }
  }
  for (v=0; v<this->base_map().nb_faces(); v++)
  {
    res[v]=h.representative(v) ;
  }
}

#endif // vpyr_2_tpyramid_txx_
