// This is prip/vpyr/vpyr_2_tpyramid_base.h
#ifndef vpyr_2_tpyramid_base_h_
#define vpyr_2_tpyramid_base_h_
//:
// \file
// \brief provides the elements for the base structure of a topological pyramid.
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  06 May 2004 Jocelyn Marchadier
// \endverbatim

#include "vpyr_2_pyramid_base.h"
#include <vmap/vmap_2_tmap_elts.h>

class vpyr_2_tpyramid_base_dart ;
class vpyr_2_tpyramid_base_vertex ;
class vpyr_2_tpyramid_base_edge ;
class vpyr_2_tpyramid_base_face ;

//: The structure of a dart in the base level.
class vpyr_2_tpyramid_base_dart : public vpyr_2_pyramid_base_dart,
                                  public vmap_2_tmap_tuple< vpyr_2_tpyramid_base_vertex,
                                                            vpyr_2_tpyramid_base_edge,
                                                            vpyr_2_tpyramid_base_face >
{
 public:
  //:
  typedef vmap_2_tmap_tuple< vpyr_2_tpyramid_base_vertex, vpyr_2_tpyramid_base_edge, vpyr_2_tpyramid_base_face > _BaseTuple ;

  //: Associated edge.
  const vpyr_2_tpyramid_base_edge & edge() const
  {
    return (const vpyr_2_tpyramid_base_edge &) _BaseTuple::edge() ;
  }

  //: Associated vertex.
  const vpyr_2_tpyramid_base_vertex & vertex() const
  {
    return (const vpyr_2_tpyramid_base_vertex &) _BaseTuple::vertex() ;
  }

  //: Associated face.
  const vpyr_2_tpyramid_base_face & face() const
  {
    return (const vpyr_2_tpyramid_base_face &) _BaseTuple::face() ;
  }

  //: Associated edge.
  vpyr_2_tpyramid_base_edge & edge()
  {
    return (vpyr_2_tpyramid_base_edge &) _BaseTuple::edge() ;
  }

  //: Associated vertex.
  vpyr_2_tpyramid_base_vertex & vertex()
  {
    return (vpyr_2_tpyramid_base_vertex &) _BaseTuple::vertex() ;
  }

  //: Associated face.
  vpyr_2_tpyramid_base_face & face()
  {
    return (vpyr_2_tpyramid_base_face &) _BaseTuple::face() ;
  }

  //: Associated edge.
  const vpyr_2_tpyramid_base_edge & edge(vmap_level_index level) const ;

  //: Associated vertex.
  const vpyr_2_tpyramid_base_vertex & vertex(vmap_level_index level) const ;

  //: Associated face.
  const vpyr_2_tpyramid_base_face & face(vmap_level_index level) const ;

  //: Associated edge.
  vpyr_2_tpyramid_base_edge & edge(vmap_level_index level) ;

  //: Associated vertex.
  vpyr_2_tpyramid_base_vertex & vertex(vmap_level_index level) ;

  //: Associated face.
  vpyr_2_tpyramid_base_face & face(vmap_level_index level) ;

  //:
  void set_sequence_index(vmap_dart_index arg) ;
};

template <class D>
class vmap_2_tpd_dart : public vpyr_2_tpyramid_base_dart
{
 public:

  //:
  typename D::value_type & data(vmap_level_index arg_level)
  {
    return _d.data(arg_level) ;
  }

  //:
  const typename D::value_type & data(vmap_level_index arg_level) const
  {
    return _d.data(arg_level) ;
  }
 protected :

  //:
  D _d ;
};

//:
typedef vmap_2_map_dart_base_iterator<vpyr_2_tpyramid_base_dart&,
                                      vpyr_2_tpyramid_base_dart*,
                                      vpyr_2_tpyramid_base_dart**> vpyr_2_tpyramid_base_dart_iterator ;

//:
typedef vmap_2_map_dart_base_iterator<const vpyr_2_tpyramid_base_dart&,
                                      const vpyr_2_tpyramid_base_dart*,
                                      vpyr_2_tpyramid_base_dart*const*> vpyr_2_tpyramid_const_base_dart_iterator ;

//: class for vertices on the base.
class vpyr_2_tpyramid_base_vertex : public vmap_2_tmap_vertex
{
 public :

  typedef vpyr_2_tpyramid_base_dart_iterator dart_iterator ;

  //:
  typedef vpyr_2_tpyramid_const_base_dart_iterator const_dart_iterator ;

  //: Returns the corresponding vertex at level "level".
  const vpyr_2_tpyramid_base_vertex* ancestor(vmap_level_index level) const ;

  //: Returns the corresponding vertex at level "level".
  vpyr_2_tpyramid_base_vertex* ancestor(vmap_level_index level) ;

  //: Returns the number of edges adjacent to the vertex, i.e., the cardinal of associated sigma*.
  int degree(vmap_level_index level) const ;

  //: Returns an iterator on the first dart of the associated sigma*.
  dart_iterator begin(vmap_level_index level) ;

  //: Returns an iterator on the first dart of the associated sigma*.
  const_dart_iterator begin(vmap_level_index level) const ;

  //: Returns an iterator on the first dart of the associated phi*.
  dart_iterator begin()
  {
    return vmap_2_tmap_vertex::begin() ;
  }

  //: Returns an iterator on the first dart of the associated phi*.
  const_dart_iterator begin() const
  {
    return vmap_2_tmap_vertex::begin() ;
  }

  //: Returns the index of the last level of existence of the vertex.
  vmap_level_index last_level() const
  {
    return _last_level ;
  }

  //: Sets the index of the last level of existence of the vertex.
  void set_last_level(vmap_level_index arg)
  {
    _last_level = arg ;
  }

 private:

  //:
  vmap_level_index _last_level ;
};

template <class D>
class vmap_2_tpd_vertex : public vpyr_2_tpyramid_base_vertex
{
 public:

  //:
  typename D::value_type & data(vmap_level_index arg_level)
  {
    return _d.data(arg_level) ;
  }

  //:
  const typename D::value_type  & data(vmap_level_index arg_level) const
  {
    return _d.data(arg_level) ;
  }

 protected :

  //:
  D _d ;
};

//: The class for faces of the base structure of a topological pyramid.
class vpyr_2_tpyramid_base_face : public vmap_2_tmap_face
{
 public :

  typedef vpyr_2_tpyramid_base_dart_iterator dart_iterator ;

  //:
  typedef vpyr_2_tpyramid_const_base_dart_iterator const_dart_iterator ;

  //: Returns the corresponding face at level "level".
  const vpyr_2_tpyramid_base_face* ancestor(vmap_level_index level) const ;

  //: Returns the corresponding face at level "level".
  vpyr_2_tpyramid_base_face* ancestor(vmap_level_index level);

  //: Returns the number of edges adjacent to the face, i.e., the cardinal of associated phi*.
  int degree(vmap_level_index level) const ;

  //: Returns an iterator on the first dart of the associated phi*.
  dart_iterator begin(vmap_level_index level) ;

  //: Returns an iterator on the first dart of the associated phi*.
  const_dart_iterator begin(vmap_level_index level) const ;

  //: Returns an iterator on the first dart of the associated phi*.
  dart_iterator begin()
  {
    return vmap_2_tmap_face::begin() ;
  }

  //: Returns an iterator on the first dart of the associated phi*.
  const_dart_iterator begin() const
  {
    return vmap_2_tmap_face::begin() ;
  }

  //: Returns the index of the last level of existence of the face.
  vmap_level_index last_level() const
  {
    return _last_level ;
  }

  //: Sets the index of the last level of existence of the face.
  void set_last_level(vmap_level_index arg)
  {
    _last_level = arg ;
  }

 private:

  //:
  vmap_level_index _last_level ;
};

template <class D>
class vmap_2_tpd_face : public vpyr_2_tpyramid_base_face
{
 public:

  //:
  typename D::value_type & data(vmap_level_index arg_level)
  {
    return _d.data(arg_level) ;
  }

  //:
  const typename D::value_type & data(vmap_level_index arg_level) const
  {
    return _d.data(arg_level) ;
  }

 protected :

  //:
  D _d ;
};

//:
class vpyr_2_tpyramid_base_edge : public vmap_2_tmap_edge
{
 public:

  //:
  typedef vpyr_2_tpyramid_base_dart_iterator dart_iterator ;

  //:
  typedef vpyr_2_tpyramid_const_base_dart_iterator const_dart_iterator ;

  //:
  typedef vpyr_2_tpyramid_const_base_dart_iterator const_dart_iterator ;

  //: Returns the corresponding edge at level "level".
  const vpyr_2_tpyramid_base_edge* ancestor(vmap_level_index level) const ;

  //: Returns the corresponding edge at level "level".
  vpyr_2_tpyramid_base_edge* ancestor(vmap_level_index level);

  //: Returns the cardinal of alpha*, i.e. 2.
  //    This is present for having an homogeneous view of the
  //    elements associated to permutations.
  int degree(vmap_level_index level) const ;

  //: Returns true if the edge is a self-loop, false otherwise.
  bool is_self_loop (vmap_level_index level) const ;

  //: Returns true if the edge is an empty self-loop, false otherwise.
  bool is_empty_self_loop (vmap_level_index level) const ;

  //: Returns true if the edge is an isthmus, false otherwise.
  bool is_isthmus (vmap_level_index level) const ;

  //: Returns true if the edge is a pendant edge, false otherwise.
  bool is_pendant (vmap_level_index level) const ;

  //: Returns true if the edge is an isolated self-loop, false otherwise.
  bool is_isolated_self_loop (vmap_level_index level) const ;

  //: Returns the extremity of the edge corresponding to the direct dart.
  const vpyr_2_tpyramid_base_vertex & first_vertex(vmap_level_index level) const ;

  //: Returns the extremity of the edge corresponding to the inverse dart.
  const vpyr_2_tpyramid_base_vertex & last_vertex(vmap_level_index level) const ;

  //: Returns the right face of the edge.
  const vpyr_2_tpyramid_base_face & right_face(vmap_level_index level) const ;

  //: Returns the left face of the edge.
  const vpyr_2_tpyramid_base_face & left_face(vmap_level_index level) const ;

  //: Returns true if "arg" is adjacent to the edge.
  bool is_adjacent_to (const vpyr_2_tpyramid_base_face & arg,vmap_level_index level) const ;

  //: Returns the direct dart associated to the edge.
  const vpyr_2_tpyramid_base_dart & direct_dart(vmap_level_index level) const ;

  //: Returns the inverse dart associated to the edge.
  const vpyr_2_tpyramid_base_dart & inverse_dart(vmap_level_index level) const ;

  //: Returns an iterator on the first dart of the associated alpha*.
  dart_iterator begin(vmap_level_index level) ;

  //: Returns an iterator on the first dart of the associated alpha*.
  const_dart_iterator begin(vmap_level_index level) const ;

  //: Returns an iterator on the first dart of the associated phi*.
  dart_iterator begin()
  {
    return vmap_2_tmap_edge::begin() ;
  }

  //: Returns an iterator on the first dart of the associated phi*.
  const_dart_iterator begin() const
  {
    return vmap_2_tmap_edge::begin() ;
  }

  //: Returns the index of the last level of existence of the edge.
  vmap_level_index last_level() const ;

  //: Sets the index of the last level of existence of the edge.
  void set_last_level(vmap_level_index arg)
  {}
};

//:
template <class D>
class vmap_2_tpd_edge : public vpyr_2_tpyramid_base_edge
{
 public:

  //:
  typename D::value_type & data(vmap_level_index arg_level)
  {
    return _d.data(arg_level) ;
  }

  //:
  const typename D::value_type & data(vmap_level_index arg_level) const
  {
    return _d.data(arg_level) ;
  }

 protected :

  //:
  D _d ;
};

#endif
