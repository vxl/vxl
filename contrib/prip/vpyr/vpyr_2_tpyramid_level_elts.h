// This is prip/vpyr/vpyr_2_tpyramid_level_elts.h
#ifndef vpyr_2_tpyramid_level_elts_h_
#define vpyr_2_tpyramid_level_elts_h_
//:
// \file
// \brief defines the elements of a level of a pyramid.
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  Modifications
//   06 May 2004 Jocelyn Marchadier
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
  vpyr_2_tpyramid_level_dart() {}

  //:
  vpyr_2_tpyramid_level_dart(const vpyr_2_tpyramid_level_dart & arg)
    :vpyr_2_pyramid_level_dart(arg)                                  {}

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
  vpyr_2_tpyramid_base_dart* link() const {return (vpyr_2_tpyramid_base_dart*)this->link_ ;}
};

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
    return link()->data(this->level_) ;
  }

  //:
  const value_type & data() const
  {
    return link()->data(this->level_) ;
  }

  //:
  base_type* link() const {return (base_type*)this->link_ ;}
};

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
  vpyr_2_tpyramid_level_element_iterator() {}

  //:
  vpyr_2_tpyramid_level_element_iterator(const self_type &it)
    :it_(it.it_)
  {
    this->level_=it.level() ;
    set_link() ;
  }

  //:
  ~vpyr_2_tpyramid_level_element_iterator() {}

  //:
  self_type & operator=(const self_type &it)
  {
    this->level_=it.level() ;
    it_=it.reference() ;
    set_link() ;
    return *this ;
  }

  //:
  bool operator==(const self_type &it) const
  {
    return it_==it.it_ ;
  }

  //:
  bool operator!=(const self_type &it) const
  {
    return it_!=it.it_ ;
  }

  //:
  Ref operator*() const
  {
    return (Ref)*this ;
  }

  //:
  Ptr operator->() const
  {
    return (Ptr)this ;
  }

  //:
  self_type & operator++()
  {
    ++it_;
    set_link();
    return *this ;
  }

  //:
  vmap_level_index level() const
  {
    return FD::level();
  }

// private :

  //:
  vpyr_2_tpyramid_level_element_iterator(It arg,vmap_level_index l)
    :it_(arg)
  {
    set(*arg,l) ;
  }

  //:
  It reference() const
  {
    return it_ ;
  }
 private:

  //:
  void set_link()
  {
    this->link_=*it_ ;
  }

  //:
  It it_ ;
};

//:
typedef vpyr_2_pyramid_level_dart_base_iterator<vpyr_2_tpyramid_level_dart,
                                                vpyr_2_tpyramid_level_dart&,
                                                vpyr_2_tpyramid_level_dart*,
                                                vpyr_2_tpyramid_base_dart**> vpyr_2_tpyramid_level_dart_iterator ;

//:
typedef vpyr_2_pyramid_level_dart_base_iterator<vpyr_2_tpyramid_level_dart,
                                                const vpyr_2_tpyramid_level_dart&,
                                                const vpyr_2_tpyramid_level_dart*,
                                                vpyr_2_tpyramid_base_dart*const*> vpyr_2_tpyramid_level_const_dart_iterator ;

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
    link_= arg_link ;
    level_=arg_level ;
  }

  //:   Returns the number of edges adjacent to the vertex, i.e. the cardinal of associated sigma*.
  int degree() const
  {
    return link_->degree(level()) ;
  }

  //: Returns an iterator on the first dart of the associated sigma-cycle.
  dart_iterator begin()
  {
    return dart_iterator(link_->begin(level()).reference(),level()) ;
  }

  //: Returns an iterator on the first dart of the associated sigma-cycle.
  const_dart_iterator begin() const
  {
    return const_dart_iterator(((const vpyr_2_tpyramid_base_vertex*)link_)->begin(level()).reference(),level()) ;
  }

  //: The vertex' level.
  vmap_level_index level() const
  {
    return level_;
  }

  //: An index of the map's vertex sequence.
  vmap_vertex_index sequence_index() const
  {
    return link_->sequence_index() ;
  }

 protected :

  //:
  base_type* link() const {return link_ ;}

  //:
  vpyr_2_tpyramid_base_vertex* link_ ;

  //:
  vmap_level_index level_ ;
};

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
    return link()->data(level_) ;
  }

  //:
  const value_type & data() const
  {
    return link()->data(level_) ;
  }
};

//:  The public face class. Faces are the orbits phi* of the darts.
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
    link_= arg_link ;
    level_=arg_level ;
  }

  //: Returns the number of edges adjacent to the face, i.e., the cardinal of associated phi*.
  int degree() const
  {
    return link_->degree(level()) ;
  }

  //: Returns an iterator on the first dart of the associated phi-cycle.
  dart_iterator begin()
  {
    return dart_iterator(link_->begin(level()).reference(),level()) ;
  }

  //: Returns an iterator on the first dart of the associated phi-cycle.
  const_dart_iterator begin() const
  {
    return const_dart_iterator(((const vpyr_2_tpyramid_base_face*)link_)->begin(level()).reference(),level()) ;
  }

  //: The face's level.
  vmap_level_index level() const
  {
    return level_;
  }

  //: An index of the map's vertex sequence.
  vmap_face_index sequence_index() const
  {
    return link_->sequence_index() ;
  }

 protected:

  //:
  friend class vpyr_2_tpyramid_level_edge ;

  //:
  base_type* link() const {return link_ ;}

  //:
  vpyr_2_tpyramid_base_face* link_ ;

  //:
  vmap_level_index level_ ;
};

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
    return link()->data(level_) ;
  }

  //:
  const value_type & data() const
  {
    return link()->data(level_) ;
  }
};

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
    link_= arg_link ;
    level_=arg_level ;
  }

  //: Returns the cardinal of alpha*, i.e. 2.
  //  This is present for having an homogeneous view of the
  //  elements associated to permutations.
  int degree() const
  {
    return link_->degree(level()) ;
  }

  //: Returns true if the edge is a self-loop, false otherwise.
  bool is_self_loop () const
  {
    return link_->is_self_loop(level()) ;
  }

  //: Returns true if the edge is an empty self-loop, false otherwise.
  bool is_empty_self_loop () const
  {
    return link_->is_empty_self_loop(level()) ;
  }

  //: Returns true if the edge is an isthmus, false otherwise.
  bool is_isthmus() const
  {
    return link_->is_isthmus(level()) ;
  }

  //: Returns true if the edge is a pendant edge, false otherwise.
  bool is_pendant() const
  {
    return link_->is_pendant(level()) ;
  }

  //: Returns true if the edge is an isolated self-loop, false otherwise.
  bool is_isolated_self_loop() const
  {
    return link_->is_isolated_self_loop(level()) ;
  }

  //: Returns an iterator on the first dart of the associated alpha-cycle.
  dart_iterator begin()
  {
    return dart_iterator(link_->begin(level()).reference(),level()) ;
  }

  //: Returns an iterator on the first dart of the associated alpha-cycle.
  const_dart_iterator begin() const
  {
    return const_dart_iterator(((const vpyr_2_tpyramid_base_edge*)link_)->begin(level()).reference(),level()) ;
  }

  //:
  const vpyr_2_tpyramid_level_vertex first_vertex() const
  {
    vpyr_2_tpyramid_level_vertex tmp ;
    tmp.set(const_cast<vpyr_2_tpyramid_base_vertex*>(&link_->first_vertex(level())), level());
    return tmp;
  }

  //:
  const vpyr_2_tpyramid_level_vertex last_vertex() const
  {
    vpyr_2_tpyramid_level_vertex tmp ;
    tmp.set(const_cast<vpyr_2_tpyramid_base_vertex*>(&link_->last_vertex(level())), level());
    return tmp;
  }

  //:
  const vpyr_2_tpyramid_level_face right_face() const
  {
    vpyr_2_tpyramid_level_face tmp ;
    tmp.set(const_cast<vpyr_2_tpyramid_base_face*>(&link_->right_face(level())),level()) ;
    return tmp ;
  }

  //:
  const vpyr_2_tpyramid_level_face left_face() const
  {
    vpyr_2_tpyramid_level_face tmp ;
    tmp.set(const_cast<vpyr_2_tpyramid_base_face*>(&link_->left_face(level())),level()) ;
    return tmp ;
  }

  //:
  bool is_adjacent_to (const vpyr_2_tpyramid_level_face & arg) const
  {
    return link_->is_adjacent_to(*arg.link(),level()) ;
  }

  //:
  inline const vpyr_2_tpyramid_level_dart direct_dart() const ;

  //:
  inline const vpyr_2_tpyramid_level_dart inverse_dart() const ;

  //: The edge's level.
  vmap_level_index level() const
  {
    return level_;
  }

  //: An index of the map's vertex sequence.
  vmap_edge_index sequence_index() const
  {
    return link_->sequence_index() ;
  }

 protected:

  //:
  base_type* link() const {return link_ ;}

  //:
  vpyr_2_tpyramid_base_edge* link_ ;

  //:
  vmap_level_index level_ ;
};

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
    return link()->data(level_) ;
  }

  //:
  const value_type & data() const
  {
    return link()->data(level_) ;
  }
};

//:
inline const vpyr_2_tpyramid_level_edge vpyr_2_tpyramid_level_dart::edge() const
{
  vpyr_2_tpyramid_level_edge tmp ;
  tmp.set(&((vpyr_2_tpyramid_base_dart*)this->link_)->edge(level()),level()) ;
  return tmp ;
}

inline const vpyr_2_tpyramid_level_vertex vpyr_2_tpyramid_level_dart::vertex() const
{
  vpyr_2_tpyramid_level_vertex tmp ;
  tmp.set(&((vpyr_2_tpyramid_base_dart*)this->link_)->vertex(level()),level()) ;
  return tmp ;
}

inline const vpyr_2_tpyramid_level_face vpyr_2_tpyramid_level_dart::face() const
{
  vpyr_2_tpyramid_level_face tmp ;
  tmp.set(&((vpyr_2_tpyramid_base_dart*)this->link_)->face(level()),level()) ;
  return tmp ;
}

inline vpyr_2_tpyramid_level_edge vpyr_2_tpyramid_level_dart::edge()
{
  vpyr_2_tpyramid_level_edge tmp ;
  tmp.set(&((vpyr_2_tpyramid_base_dart*)this->link_)->edge(level()),level()) ;
  return tmp ;
}

inline vpyr_2_tpyramid_level_vertex vpyr_2_tpyramid_level_dart::vertex()
{
  vpyr_2_tpyramid_level_vertex tmp ;
  tmp.set(&((vpyr_2_tpyramid_base_dart*)this->link_)->vertex(level()),level()) ;
  return tmp ;
}

inline vpyr_2_tpyramid_level_face vpyr_2_tpyramid_level_dart::face()
{
  vpyr_2_tpyramid_level_face tmp;
  tmp.set(&((vpyr_2_tpyramid_base_dart*)this->link_)->face(level()),level()) ;
  return tmp ;
}

inline const vpyr_2_tpyramid_level_dart vpyr_2_tpyramid_level_edge::direct_dart() const
{
  vpyr_2_tpyramid_level_dart tmp ;
  tmp.set(const_cast<vpyr_2_tpyramid_base_dart*>(&this->link_->direct_dart(level())),level()) ;
  return tmp ;
}

inline const vpyr_2_tpyramid_level_dart vpyr_2_tpyramid_level_edge::inverse_dart() const
{
  vpyr_2_tpyramid_level_dart tmp ;
  tmp.set(const_cast<vpyr_2_tpyramid_base_dart*>(&this->link_->inverse_dart(level())),level()) ;
  return tmp ;
}

#endif
