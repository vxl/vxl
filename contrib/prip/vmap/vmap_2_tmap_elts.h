// This is contrib/prip/vmap/vmap_2_tmap_elts.h
#ifndef vmap_2_tmap_elts_h_
#define vmap_2_tmap_elts_h_
//:
// \file
// \brief provides the base dart, vertex, edge and face classes for topological maps.
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  06 May 2004 Jocelyn Marchadier
// \endverbatim

#include "vmap_2_map.h"

class vmap_2_tmap_vertex ;
class vmap_2_tmap_edge ;
class vmap_2_tmap_face ;

//: a triple of incident elements to be attached to the darts.
template <class V, class E, class F>
class vmap_2_tmap_tuple
{
public:
  vmap_2_tmap_tuple()
  {
    _edge=NULL ;
    _vertex=NULL ;
    _face=NULL ;
  }
  
  //: Associated edge.
  const E & edge() const
  {
    return *_edge ;
  }

  //: Associated vertex.
  const V& vertex() const
  {
    return *_vertex ;
  }

  //: Associated face.
  const F& face() const
  {
    return *_face ;
  }

  //: Associated edge.
  E& edge()
  {
    return *_edge ;
  }

  //: Associated vertex.
  V& vertex()
  {
    return *_vertex ;
  }

  //: Associated face.
  F& face()
  {
    return *_face ;
  }

  //: For construction of the underlying map.
  void set_vertex(V*arg)
  {
    _vertex=arg ;
  }

  //: 
  void set_edge(E*arg)
  {
    _edge=arg ;
  }

  //: 
  void set_face(F*arg)
  {
    _face=arg ;
  }

  //: 
  V* get_vertex_pointer() const
  {
    return _vertex ;
  }

  //: 
  E* get_edge_pointer() const
  {
    return _edge ;
  }

  //: 
  F* get_face_pointer() const
  {
    return _face ;
  }

  //: 
  bool valid() const
  {
    return _edge!=NULL && _vertex!=NULL && _face != NULL ;
  }

protected :
  
  //: 
  E * _edge ;
  
  //: 
  V * _vertex ;
  
  //: 
  F * _face ;
} ;

//: the base dart class of a topological map
class vmap_2_tmap_dart : public vmap_2_map_dart, public vmap_2_tmap_tuple< vmap_2_tmap_vertex, vmap_2_tmap_edge, vmap_2_tmap_face >
{
public:
  //: 
  void set_sequence_index(vmap_dart_index arg) ;
} ;


//: Enables to iterate on the topology of a topological map.
typedef vmap_2_map_dart_base_iterator< vmap_2_tmap_dart&,vmap_2_tmap_dart*,vmap_2_tmap_dart**> vmap_2_tmap_dart_iterator ;

//: Enables to iterate on the topology of a topological map.
typedef vmap_2_map_dart_base_iterator< const vmap_2_tmap_dart&,const vmap_2_tmap_dart*,vmap_2_tmap_dart*const*> vmap_2_tmap_const_dart_iterator ;

//: The public vertex_type class. Vertices are the orbits sigma* of the darts.
class vmap_2_tmap_vertex
{
public :
  //: the dart iterator of the associated darts.
  typedef vmap_2_tmap_dart_iterator dart_iterator ;
  
  //: the dart iterator of the associated darts.
  typedef vmap_2_tmap_const_dart_iterator const_dart_iterator ;

  //: Returns the number of edges adjacent to the vertex,
  //  i.e. the cardinal of associated sigma*.
  int degree() const ;

  //: Returns an iterator on the first dart of the associated sigma*.
  dart_iterator begin()
  {
    return _begin ;
  }
  
  //: Returns an iterator on the first dart of the associated sigma*.
  const_dart_iterator begin() const
  {
    return _begin ;
  }

  //: For construction of the underlying map.
  void set_begin(dart_iterator arg)
  {
    _begin = arg ;
  }

  //: An index of the map's vertex sequence.
  vmap_vertex_index sequence_index() const
  {
    return _sequence_index ;
  }
  
  //: 
  void set_sequence_index(vmap_vertex_index arg)
  {
    _sequence_index=arg ;
  }

protected :
  
  //: 
  vmap_vertex_index _sequence_index ;
  
  //: 
  dart_iterator _begin ;
} ;

//: The public face_type class.  face_types are the orbits phi* of the darts.
class vmap_2_tmap_face
{
public :
  //: the dart iterator of the associated darts.
  typedef vmap_2_tmap_dart_iterator dart_iterator ;
  
  //: the dart iterator of the associated darts.
  typedef vmap_2_tmap_const_dart_iterator const_dart_iterator ;

  //: Returns the number of edges adjacent to the face,
  //  i.e. the cardinal of associated phi*.
  int degree() const ;

  //: Returns an iterator on the first dart of the associated phi*.
  dart_iterator begin()
  {
    return _begin ;
  }

  //: Returns an iterator on the first dart of the associated phi*.
  const_dart_iterator begin() const
  {
    return _begin ;
  }

  //: For construction of the underlying map.
  void set_begin(dart_iterator arg)
  {
    _begin = arg ;
  }
  
  //: An index of the map's face sequence.
  vmap_face_index sequence_index() const
  {
    return _sequence_index ;
  }
  
  //: 
  void set_sequence_index(vmap_face_index arg)
  {
    _sequence_index=arg ;
  }

protected :
  //: 
  vmap_face_index _sequence_index ;
  
  //: 
  dart_iterator _begin ;
} ;

//: The public edge class. Edges are the orbits alpha* of the darts.
class vmap_2_tmap_edge
{
public :
  //: 
  typedef vmap_2_tmap_dart_iterator dart_iterator ;
  
  //: 
  typedef vmap_2_tmap_const_dart_iterator const_dart_iterator ;
  
  //: Returns the cardinal of alpha*, i.e. 2.
  //    This is present for having an homogeneous view of the
  //    elements associated to permutations.
  int degree() const
  {
    return 2 ;
  }

  //: Returns true if the edge is a self-loop, false otherwise.
  bool is_self_loop () const
  {
    return &first_vertex()==&last_vertex() ;
  }

  //: Returns true if the edge is an empty self-loop, false otherwise.
  bool is_empty_self_loop () const
  {
    return direct_dart().is_self_direct_loop() || inverse_dart().is_self_direct_loop() ;
  }

  //: Returns true if the edge is an isthmus, false otherwise.
  bool is_isthmus () const
  {
    return &left_face()==&right_face();
  }

  //: Returns true if the edge is a pendant edge, false otherwise.
  bool is_pendant () const
  {
    return direct_dart().is_pendant() || inverse_dart().is_pendant() ;
  }

  //: Returns true if the edge is an isolated self-loop, false otherwise.
  bool is_isolated_self_loop () const
  {
    return direct_dart().is_self_direct_loop() && inverse_dart().is_self_direct_loop() ;
  }

  //: Returns the extremity of the edge corresponding to the direct dart.
  const vmap_2_tmap_vertex & first_vertex() const
  {
    return direct_dart().vertex() ;
  }

  //: Returns the extremity of the edge corresponding to the inverse dart.
  const vmap_2_tmap_vertex & last_vertex() const
  {
    return inverse_dart().vertex() ;
  }

  //: Returns the right face of the edge.
  const vmap_2_tmap_face & right_face() const
  {
    return direct_dart().face() ;
  }

  //: Returns the left face of the edge.
  const vmap_2_tmap_face & left_face() const
  {
    return inverse_dart().face() ;
  }

  //: Returns true if "arg" is adjacent to the edge.
  bool is_adjacent_to (const vmap_2_tmap_face & arg) const
  {
    return &left_face()== &arg || &right_face()==&arg ;
  }

  //: Returns the direct dart associated to the edge.
  const vmap_2_tmap_dart & direct_dart() const
  {
    return *_begin ;
  }

  //: Returns the inverse dart associated to the edge.
  const vmap_2_tmap_dart & inverse_dart() const
  {
    return *vmap_2_map_alpha(&direct_dart()) ;
  }

  //: Returns an iterator on the first dart of the associated alpha*.
  dart_iterator begin()
  {
    return _begin ;
  }

  //: Returns an iterator on the first dart of the associated alpha*.
  const_dart_iterator begin() const
  {
    return _begin ;
  }

  //: For construction of the underlying map.
  void set_begin(dart_iterator arg)
  {
    _begin = arg ;
  }

  //: An index of the map's edge sequence.
  vmap_edge_index sequence_index() const
  {
    return _sequence_index ;
  }

  //: Sets the index of the map's edge sequence.
  void set_sequence_index(vmap_edge_index arg)
  {
    _sequence_index=arg ;
  }

protected :
  //: 
  vmap_edge_index _sequence_index ;
  
  //: 
  dart_iterator _begin ;

} ;

#endif
