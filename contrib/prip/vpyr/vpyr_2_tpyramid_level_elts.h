// This is contrib/prip/vpyr/vpyr_2_tpyramid_level_elts.h
#ifndef vpyr_2_tpyramid_level_elts_h_
#define vpyr_2_tpyramid_level_elts_h_
//:
// \file
// \brief defines the elements of a level of a pyramid.
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  06 May 2004 Jocelyn Marchadier
// \endverbatim

#include "vpyr_2_pyramid_level_elts.h"
#include "vpyr_2_tpyramid_base.h"

class vpyr_2_tpyramid_level_vertex ;
class vpyr_2_tpyramid_level_edge ;
class vpyr_2_tpyramid_level_face ;

//: The public dart_type class.
class vpyr_2_tpyramid_level_dart : public vpyr_2_pyramid_level_dart
{
public:
  //: 
  typedef vpyr_2_tpyramid_base_dart base_type ;

  //: 
  vpyr_2_tpyramid_level_dart()
  {}

  //: 
  vpyr_2_tpyramid_level_dart(const vpyr_2_tpyramid_level_dart & arg)
      :vpyr_2_pyramid_level_dart(arg)
  {}

  //: Associated edge.
  inline const vpyr_2_tpyramid_level_edge edge() const ;
  
  //: 
  inline const vpyr_2_tpyramid_level_vertex vertex() const ;
  
  //: 
  inline const vpyr_2_tpyramid_level_face face() const ;

  //: 
  inline vpyr_2_tpyramid_level_edge edge() ;
  
  //: 
  inline vpyr_2_tpyramid_level_vertex vertex() ;
  
  //: 
  inline vpyr_2_tpyramid_level_face face() ;
  
protected:
  //: 
  vpyr_2_tpyramid_base_dart* link() const {return (vpyr_2_tpyramid_base_dart*)_link ;}

} ;

//: 
template <class D>
class vmap_2_tpdl_dart ;

template <class D>
class vmap_2_tpdl_vertex ;

template <class D>
class vmap_2_tpdl_edge ;

template <class D>
class vmap_2_tpdl_face ;


//: The public dart_type class.
template <class D>
class vmap_2_tpdl_dart : public vpyr_2_tpyramid_level_dart
{
public:
  //: 
  typedef D value_set ;
  
  //: 
  typedef typename value_set::value_type value_type ;
  
  //: 
  typedef vmap_2_tpd_dart< value_set > base_type ;

protected:

  //: 
  value_type & data()
  {
    return link()->data(_level) ;
  }
  
  //: 
  const value_type & data() const
  {
    return link()->data(_level) ;
  }
  
  //: 
  base_type* link() const {return (base_type*)_link ;}

} ;

//: 
template< typename FD, typename Ref, typename Ptr, typename It >
class vpyr_2_tpyramid_level_element_iterator : protected FD
{
public:
  //: 
  typedef vpyr_2_tpyramid_level_element_iterator< FD,Ref,Ptr,It > self_type ;
  
  //: 
  typedef FD element_type ;

  //: 
  vpyr_2_tpyramid_level_element_iterator()
  {}

  //: 
  vpyr_2_tpyramid_level_element_iterator(const self_type &right)
      :_it(right._it)
  {
    _level=right.level() ;
    set_link() ;
  }

  //: 
  ~vpyr_2_tpyramid_level_element_iterator()
  {}

  //: 
  self_type & operator=(const self_type &right)
  {
    _level=right.level() ;
    _it=right.reference() ;
    set_link() ;
    return *this ;
  }

  //: 
  bool operator==(const self_type &right) const
  {
    return _it==right._it ;
  }

  //: 
  bool operator!=(const self_type &right) const
  {
    return _it!=right._it ;
  }

  //: 
  Ref operator * () const
  {
    return (Ref)*this ;
  }

  //: 
  Ptr operator->() const
  {
    return (Ptr) this;
  }

  //: 
  self_type & operator++ ()
  {
    ++_it;
    set_link();
    return *this ;
  }

  //: 
  vmap_level_index level() const
  {
    return FD::level();
  }

  //////////////////////private :
  //: 
  vpyr_2_tpyramid_level_element_iterator(It arg,vmap_level_index l)
      :_it(arg)
  {
    set(*arg,l) ;
  }
  
  //: 
  It reference() const
  {
    return _it ;
  }
private:
  
  //: 
  void set_link()
  {
    _link=*_it ;
  }
  
  //: 
  It _it ;
};

//: 
typedef vpyr_2_pyramid_level_dart_base_iterator<vpyr_2_tpyramid_level_dart,vpyr_2_tpyramid_level_dart&,vpyr_2_tpyramid_level_dart*,vpyr_2_tpyramid_base_dart**> vpyr_2_tpyramid_level_dart_iterator ;

//: 
typedef vpyr_2_pyramid_level_dart_base_iterator<vpyr_2_tpyramid_level_dart,const vpyr_2_tpyramid_level_dart&,const vpyr_2_tpyramid_level_dart*,vpyr_2_tpyramid_base_dart*const*> vpyr_2_tpyramid_level_const_dart_iterator ;

//: The public vertex_type class. Vertices are the orbits sigma* of the darts.
class vpyr_2_tpyramid_level_vertex
{
public :
  //: 
  typedef vpyr_2_tpyramid_base_vertex base_type ;

  //: 
  typedef vpyr_2_tpyramid_level_dart_iterator dart_iterator ;
  
  //: 
  typedef vpyr_2_tpyramid_level_const_dart_iterator const_dart_iterator ;

  //: 
  void set(vpyr_2_tpyramid_base_vertex* arg_link, vmap_level_index arg_level)
  {
    _link= arg_link ;
    _level=arg_level ;
  }

  //:   Returns the number of edges adjacent to the vertex, i.e. the cardinal of associated sigma*.
  int degree() const
  {
    return _link->degree(level()) ;
  }

  //: Returns an iterator on the first dart of the associated sigma-cycle.
  dart_iterator begin()
  {
    return dart_iterator(_link->begin(level()).reference(),level()) ;
  }

  //: Returns an iterator on the first dart of the associated sigma-cycle.
  const_dart_iterator begin() const
  {
    return const_dart_iterator(((const vpyr_2_tpyramid_base_vertex*)_link)->begin(level()).reference(),level()) ;
  }

  //: The vertex' level.
  vmap_level_index level() const
  {
    return _level;
  }

  //: An index of the map's vertex sequence.
  vmap_vertex_index sequence_index() const
  {
    return _link->sequence_index() ;
  }

protected :
  
  //: 
  base_type* link() const {return _link ;}
  
  //: 
  vpyr_2_tpyramid_base_vertex* _link ;
  
  //: 
  vmap_level_index _level ;
} ;

//: 
template <class D>
class vmap_2_tpdl_vertex : public vpyr_2_tpyramid_level_vertex
{
public:
  //:
  typedef D value_set ;
  
  //:
  typedef typename value_set::value_type value_type ;
  
  //:
  typedef vmap_2_tpd_vertex< value_set > base_type ;

protected :
  
  //:
  base_type* link() const {return (base_type*)vpyr_2_tpyramid_level_vertex::link() ;}

  //:
  value_type & data()
  {
    return link()->data(_level) ;
  }
  
  //:
  const value_type & data() const
  {
    return link()->data(_level) ;
  }
} ;

///:  The public face class. Faces are the orbits phi* of the darts.
class vpyr_2_tpyramid_level_face
{
public :
  //:
  typedef vpyr_2_tpyramid_base_face base_type ;

  //:
  typedef vpyr_2_tpyramid_level_dart_iterator dart_iterator ;
  
  //:
  typedef vpyr_2_tpyramid_level_const_dart_iterator const_dart_iterator ;

  //:
  void set(vpyr_2_tpyramid_base_face* arg_link, vmap_level_index arg_level)
  {
    _link= arg_link ;
    _level=arg_level ;
  }

  //: Returns the number of edges adjacent to the face,
  //  i.e. the cardinal of associated phi*.
  int degree() const
  {
    return _link->degree(level()) ;
  }

  //: Returns an iterator on the first dart of the associated phi-cycle.
  dart_iterator begin()
  {
    return dart_iterator(_link->begin(level()).reference(),level()) ;
  }

  //: Returns an iterator on the first dart of the associated phi-cycle.
  const_dart_iterator begin() const
  {
    return const_dart_iterator(((const vpyr_2_tpyramid_base_face*)_link)->begin(level()).reference(),level()) ;
  }
  
  //: The face's level.
  vmap_level_index level() const
  {
    return _level;
  }

  //: An index of the map's vertex sequence.
  vmap_face_index sequence_index() const
  {
    return _link->sequence_index() ;
  }

protected:
  
  //: 
  friend class vpyr_2_tpyramid_level_edge ;
  
  //: 
  base_type* link() const {return _link ;}
  
  //: 
  vpyr_2_tpyramid_base_face* _link ;
  
  //: 
  vmap_level_index _level ;
} ;

//: 
template <class D>
class vmap_2_tpdl_face : public vpyr_2_tpyramid_level_face
{
public:
  //: 
  typedef D value_set ;
  
  //: 
  typedef typename value_set::value_type value_type ;
  
  //: 
  typedef vmap_2_tpd_face< value_set > base_type ;

protected :
  //: 
  base_type* link() const {return (base_type*)vpyr_2_tpyramid_level_face::link() ;}

  //: 
  value_type & data()
  {
    return link()->data(_level) ;
  }
  
  //: 
  const value_type & data() const
  {
    return link()->data(_level) ;
  }
} ;


//: The public edge_type class. Edges are the orbits alpha* of the darts.
class vpyr_2_tpyramid_level_edge
{
public :
  //: 
  typedef vpyr_2_tpyramid_base_edge base_type ;

  //: 
  typedef vpyr_2_tpyramid_level_dart_iterator dart_iterator ;
  
  //: 
  typedef vpyr_2_tpyramid_level_const_dart_iterator const_dart_iterator ;

  //: 
  void set(vpyr_2_tpyramid_base_edge* arg_link, vmap_level_index arg_level)
  {
    _link= arg_link ;
    _level=arg_level ;
  }


  //: Returns the cardinal of alpha*, i.e. 2.
  //  This is present for having an homogeneous view of the
  //  elements associated to permutations.
  int degree() const
  {
    return _link->degree(level()) ;
  }

  //: Returns true if the edge is a self-loop, false otherwise.
  bool is_self_loop () const
  {
    return _link->is_self_loop(level()) ;
  }

  //: Returns true if the edge is an empty self-loop, false otherwise.
  bool is_empty_self_loop () const
  {
    return _link->is_empty_self_loop(level()) ;
  }

  //: Returns true if the edge is an isthmus, false otherwise.
  bool is_isthmus() const
  {
    return _link->is_isthmus(level()) ;
  }

  //: Returns true if the edge is a pendant edge, false otherwise.
  bool is_pendant() const
  {
    return _link->is_pendant(level()) ;
  }

  //: Returns true if the edge is an isolated self-loop, false otherwise.
  bool is_isolated_self_loop() const
  {
    return _link->is_isolated_self_loop(level()) ;
  }

  //: Returns an iterator on the first dart of the associated alpha-cycle.
  dart_iterator begin()
  {
    return dart_iterator(_link->begin(level()).reference(),level()) ;
  }

  //: Returns an iterator on the first dart of the associated alpha-cycle.
  const_dart_iterator begin() const
  {
    return const_dart_iterator(((const vpyr_2_tpyramid_base_edge*)_link)->begin(level()).reference(),level()) ;
  }

  //: 
  const vpyr_2_tpyramid_level_vertex first_vertex() const
  {
    vpyr_2_tpyramid_level_vertex tmp ;
    tmp.set(const_cast<vpyr_2_tpyramid_base_vertex*>(&_link->first_vertex(level())), level());
    return tmp;
  }

  //: 
  const vpyr_2_tpyramid_level_vertex last_vertex() const
  {
    vpyr_2_tpyramid_level_vertex tmp ;
    tmp.set(const_cast<vpyr_2_tpyramid_base_vertex*>(&_link->last_vertex(level())), level());
    return tmp;
  }

  //: 
  const vpyr_2_tpyramid_level_face right_face() const
  {
    vpyr_2_tpyramid_level_face tmp ;
    tmp.set(const_cast<vpyr_2_tpyramid_base_face*>(&_link->right_face(level())),level()) ;
    return tmp ;
  }

  //: 
  const vpyr_2_tpyramid_level_face left_face() const
  {
    vpyr_2_tpyramid_level_face tmp ;
    tmp.set(const_cast<vpyr_2_tpyramid_base_face*>(&_link->left_face(level())),level()) ;
    return tmp ;
  }

  //: 
  bool is_adjacent_to (const vpyr_2_tpyramid_level_face & arg) const
  {
    return _link->is_adjacent_to(*arg.link(),level()) ;
  }

  //: 
  inline const vpyr_2_tpyramid_level_dart direct_dart() const ;

  //: 
  inline const vpyr_2_tpyramid_level_dart inverse_dart() const ;

  //: The edge's level.
  vmap_level_index level() const
  {
    return _level;
  }

  //: An index of the map's vertex sequence.
  vmap_edge_index sequence_index() const
  {
    return _link->sequence_index() ;
  }

protected:
  
  //: 
  base_type* link() const {return _link ;}

  //: 
  vpyr_2_tpyramid_base_edge* _link ;
  
  //: 
  vmap_level_index _level ;
} ;

//: 
template <class D>
class vmap_2_tpdl_edge : public vpyr_2_tpyramid_level_edge
{
public:
  //: 
  typedef D value_set ;
  
  //: 
  typedef typename value_set::value_type value_type ;
  
  //: 
  typedef vmap_2_tpd_edge< value_set > base_type ;

protected :
  //: 
  base_type* link() const {return (base_type*)vpyr_2_tpyramid_level_edge::link() ;}

  //: 
  value_type & data()
  {
    return link()->data(_level) ;
  }
  
  //: 
  const value_type & data() const
  {
    return link()->data(_level) ;
  }
} ;


//: 
inline const vpyr_2_tpyramid_level_edge vpyr_2_tpyramid_level_dart::edge() const
{
  vpyr_2_tpyramid_level_edge tmp ;
  tmp.set(&((vpyr_2_tpyramid_base_dart*)_link)->edge(level()),level()) ;
  return tmp ;
}

inline const vpyr_2_tpyramid_level_vertex vpyr_2_tpyramid_level_dart::vertex() const
{
  vpyr_2_tpyramid_level_vertex tmp ;
  tmp.set(&((vpyr_2_tpyramid_base_dart*)_link)->vertex(level()),level()) ;
  return tmp ;
}

inline const vpyr_2_tpyramid_level_face vpyr_2_tpyramid_level_dart::face() const
{
  vpyr_2_tpyramid_level_face tmp ;
  tmp.set(&((vpyr_2_tpyramid_base_dart*)_link)->face(level()),level()) ;
  return tmp ;
}

inline vpyr_2_tpyramid_level_edge vpyr_2_tpyramid_level_dart::edge()
{
  vpyr_2_tpyramid_level_edge tmp ;
  tmp.set(&((vpyr_2_tpyramid_base_dart*)_link)->edge(level()),level()) ;
  return tmp ;
}

inline vpyr_2_tpyramid_level_vertex vpyr_2_tpyramid_level_dart::vertex()
{
  vpyr_2_tpyramid_level_vertex tmp ;
  tmp.set(&((vpyr_2_tpyramid_base_dart*)_link)->vertex(level()),level()) ;
  return tmp ;
}

inline vpyr_2_tpyramid_level_face vpyr_2_tpyramid_level_dart::face()
{
  vpyr_2_tpyramid_level_face tmp;
  tmp.set(&((vpyr_2_tpyramid_base_dart*)_link)->face(level()),level()) ;
  return tmp ;
}

inline const vpyr_2_tpyramid_level_dart vpyr_2_tpyramid_level_edge::direct_dart() const
{
  vpyr_2_tpyramid_level_dart tmp ;
  //tmp.set(const_cast<vpyr_2_tpyramid_base_dart*>(&_link->direct_dart(level())),level()) ;
  tmp.set(const_cast<vpyr_2_tpyramid_base_dart*>(&_link->direct_dart(level())),level()) ;
  return tmp ;
}

inline const vpyr_2_tpyramid_level_dart vpyr_2_tpyramid_level_edge::inverse_dart() const
{
  vpyr_2_tpyramid_level_dart tmp ;
  tmp.set(const_cast<vpyr_2_tpyramid_base_dart*>(&_link->inverse_dart(level())),level()) ;
  return tmp ;
}

#endif
