// This is prip/vpyr/vpyr_2_tpyramid_level.h
#ifndef vpyr_2_tpyramid_level_h_
#define vpyr_2_tpyramid_level_h_
//:
// \file
// \brief
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  06 May 2004 Jocelyn Marchadier
// \endverbatim

#include "vpyr_2_pyramid_level.h"
#include <vmap/vmap_2_tmap.h>
#include "vpyr_2_tpyramid_level_elts.h"

template <class _level_type>
class vpyr_2_tpyramid ;

//:
template <class V, class E, class F, class D>
class vpyr_2_tpyramid_level: public vpyr_2_pyramid_level<D>,
      public vmap_ptr_sequence< typename V::base_type >,
      public vmap_ptr_sequence< typename E::base_type >,
      public vmap_ptr_sequence< typename F::base_type >
{
 public:

    //:
  static vmap_2_tmap_tag tag ;
  //:
  typedef vpyr_2_tpyramid_level<V,E,F,D> self_type;

  //:
  typedef vmap_2_tmap< typename V::base_type, typename E::base_type, typename F::base_type ,typename D::base_type > base_map_type ;

  //:
  typedef vpyr_2_tpyramid<self_type> pyramid_type ;

  //:
  typedef vpyr_2_pyramid_level<D> _Base ;

  //:
  typedef typename _Base::base_dart_type base_dart_type ;

 protected:
  //:
  typedef typename _Base::dart_pointer dart_pointer ;
  //:
  typedef typename _Base::dart_sequence_iterator dart_sequence_iterator ;
  //:
  typedef typename _Base::const_dart_sequence_iterator const_dart_sequence_iterator ;

  //:
  typedef vmap_ptr_sequence< typename V::base_type > vertex_sequence ;
  //:
  typedef typename vertex_sequence::iterator vertex_sequence_iterator;
  //:
  typedef typename vertex_sequence::const_iterator const_vertex_sequence_iterator;
  //:
  typedef typename vertex_sequence::pointer vertex_pointer ;

  //:
  typedef vmap_ptr_sequence< typename E::base_type > edge_sequence ;
  //:
  typedef typename edge_sequence::iterator edge_sequence_iterator;
  //:
  typedef typename edge_sequence::const_iterator const_edge_sequence_iterator;
  //:
  typedef typename edge_sequence::pointer edge_pointer ;

  //:
  typedef vmap_ptr_sequence< typename F::base_type > face_sequence ;
  //:
  typedef typename face_sequence::iterator face_sequence_iterator;
  //:
  typedef typename face_sequence::const_iterator const_face_sequence_iterator;
  //:
  typedef typename face_sequence::pointer face_pointer ;

 public:

  // --- Vertex types ---

  //:
  typedef V vertex_type ;
  //: reference of a vertex.
  typedef vertex_type vertex_reference ;
  //: const reference of a vertex.
  typedef const vertex_type const_vertex_reference ;
  //:
  typedef vpyr_2_tpyramid_level_element_iterator< vertex_type, vertex_type& , vertex_type*, vertex_sequence_iterator>
          vertex_iterator ;
  //:
  typedef vpyr_2_tpyramid_level_element_iterator<vertex_type,const vertex_type&,const vertex_type*,const_vertex_sequence_iterator>
          const_vertex_iterator ;

  // --- Edge types ---

  //:
  typedef E edge_type;
  //: reference of an edge.
  typedef edge_type edge_reference ;
  //: const reference of an edge.
  typedef const edge_type const_edge_reference ;
  //:
  typedef vpyr_2_tpyramid_level_element_iterator< edge_type, edge_type& , edge_type*, edge_sequence_iterator> edge_iterator ;
  //:
  typedef vpyr_2_tpyramid_level_element_iterator< edge_type, const edge_type& , const edge_type*, const_edge_sequence_iterator>
          const_edge_iterator ;

  // --- Face types ---

  //:
  typedef F face_type ;
   //: reference of a face.
  typedef face_type face_reference ;
  //: const reference of a face.
  typedef const face_type const_face_reference ;
  //:
  typedef vpyr_2_tpyramid_level_element_iterator< face_type, face_type& , face_type*, face_sequence_iterator> face_iterator ;
  //:
  typedef vpyr_2_tpyramid_level_element_iterator< face_type, const face_type& , const face_type*, const_face_sequence_iterator>
          const_face_iterator ;

  // --- Dart types ---

  //:
  typedef typename _Base::dart_type dart_type ;
  //: reference on a dart.
  typedef dart_type dart_reference ;
  //: const reference on a dart.
  typedef const dart_type const_dart_reference ;
  //:
  typedef typename _Base::dart_iterator dart_iterator ;
  //:
  typedef typename _Base::const_dart_iterator const_dart_iterator ;

  //: Kernel class for contraction.
  typedef vmap_permutation_kernel< vmap_sigma_permutation<self_type> > contraction_kernel ;

  //: Kernel class for contraction.
  typedef vmap_permutation_kernel< vmap_phi_permutation<self_type> > removal_kernel ;

  friend class vmap_kernel<self_type> ;
  friend class vmap_sigma_permutation<self_type> ;
  friend class vmap_phi_permutation<self_type> ;

  //:
  static vertex_type & cast(vpyr_2_tpyramid_level_vertex & a)
  {
    return (vertex_type&) (a) ;
  }

  //:
  static edge_type& cast(vpyr_2_tpyramid_level_edge & a)
  {
    return (edge_type&) (a) ;
  }

  //:
  static face_type& cast(vpyr_2_tpyramid_level_face & a)
  {
    return (face_type&) (a) ;
  }

  //:
  static const vertex_type& cast(const vpyr_2_tpyramid_level_vertex & a)
  {
    return (const vertex_type&) (a) ;
  }

  //:
  static const edge_type& cast(const vpyr_2_tpyramid_level_edge & a)
  {
    return (const edge_type&) (a) ;
  }

  //:
  static const face_type& cast(const vpyr_2_tpyramid_level_face & a)
  {
    return (const face_type&) (a) ;
  }

  //:
  static dart_type& cast(vpyr_2_tpyramid_level_dart & a)
  {
    return (dart_type&) (a) ;
  }

  //:
  static const dart_type& cast(const vpyr_2_tpyramid_level_dart & a)
  {
    return (const dart_type&) (a) ;
  }

  //:
  vpyr_2_tpyramid_level(vmap_level_index arg_level, pyramid_type & pyramid) ;

  //:
  vpyr_2_tpyramid_level(const self_type & arg)
      : _Base(arg),
      vertex_sequence(arg),
      edge_sequence(arg),
      face_sequence(arg)
  {}

  //:
  ~vpyr_2_tpyramid_level()
  {}

  //:
  int index() const
  {
    return _Base::index() ;
  }

  //: Returns an iterator on the first dart.
  const_dart_iterator begin_dart() const
  {
    return _Base::begin_dart() ;
  }

  //: Returns an iterator on the first dart.
  dart_iterator begin_dart()
  {
    return _Base::begin_dart() ;
  }

  //: Returns an iterator after the last dart.
  const_dart_iterator end_dart() const
  {
    return _Base::end_dart() ;
  }

  //: Returns an iterator after the last dart.
  dart_iterator end_dart()
  {
    return _Base::end_dart() ;
  }

  //: Returns an iterator on the first vertex.
  const_vertex_iterator begin_vertex () const
  {
    return const_vertex_iterator(begin_vertex_sequence(),index()) ;
  }

  //:
  vertex_iterator begin_vertex ()
  {
    return vertex_iterator(begin_vertex_sequence(),index()) ;
  }

  //: Returns an iterator after the end vertex.
  const_vertex_iterator end_vertex() const
  {
    return const_vertex_iterator(end_vertex_sequence(),index()) ;
  }

  //:
  vertex_iterator end_vertex()
  {
    return vertex_iterator(end_vertex_sequence(),index()) ;
  }

  //: Returns an iterator on the first edge.
  const_edge_iterator begin_edge () const
  {
    return const_edge_iterator(begin_edge_sequence(),index()) ;
  }

  //:
  edge_iterator begin_edge ()
  {
    return edge_iterator(begin_edge_sequence(),index()) ;
  }

  //: Returns an iterator after the end edge.
  const_edge_iterator end_edge() const
  {
    return const_edge_iterator(end_edge_sequence(),index()) ;
  }

  //:
  edge_iterator end_edge()
  {
    return edge_iterator(end_edge_sequence(),index()) ;
  }

  //: Returns an iterator on the first edge.
  const_face_iterator begin_face () const
  {
    return const_face_iterator(begin_face_sequence(),index()) ;
  }

  //:
  face_iterator begin_face ()
  {
    return face_iterator(begin_face_sequence(),index()) ;
  }

  //: Returns an iterator after the end edge of level at least "i".
  const_face_iterator end_face() const
  {
    return const_face_iterator(end_face_sequence(),index()) ;
  }

  //:
  face_iterator end_face()
  {
    return face_iterator(end_face_sequence(),index()) ;
  }

  //:
  dart_type dart(vmap_dart_index arg)
  {
    dart_type tmp ;
    tmp.set(get_dart_pointer(arg),index()) ;
    return tmp ;
  }

  //:
  const dart_type dart(vmap_dart_index arg) const
  {
    dart_type tmp ;
    tmp.set(get_dart_pointer(arg),index()) ;
    return tmp ;
  }

  //:
  vmap_dart_index index(const vpyr_2_tpyramid_level_dart & arg) const
  {
    return _Base::index((const dart_type &)arg) ;
  }

  //: Returns the number of vertices.
  int nb_vertices () const
  {
    return vertex_sequence::size() ;
  }

  //: Returns the number of edges.
  int nb_edges () const
  {
    return edge_sequence::size() ;
  }

  //: Returns the number of faces.
  int nb_faces () const
  {
    return face_sequence::size() ;
  }

  //: Returns the vertex of index "arg".
  const_vertex_reference vertex(vmap_vertex_index arg) const
  {
    vertex_type tmp;
    tmp.set(get_vertex_pointer(arg),index()) ;
    return tmp ;
  }

  //: Returns the vertex of index "arg".
  vertex_reference vertex(vmap_vertex_index arg)
  {
    vertex_type tmp;
    tmp.set(get_vertex_pointer(arg),index()) ;
    return tmp ;
  }

  //: Returns the index of the vertex "arg".
  vmap_vertex_index index (const vertex_type & arg) const
  {
    return arg.sequence_index();//-(*_begin_vertex)->index() ;
  }

  //: Returns the edge of index "arg".
  const_edge_reference edge(vmap_edge_index arg) const
  {
    edge_type tmp;
    tmp.set(get_edge_pointer(arg),index()) ;
    return tmp ;
  }

  //: Returns the edge of index "arg".
  edge_reference edge(vmap_edge_index arg)
  {
    edge_type tmp;
    tmp.set(get_edge_pointer(arg),index()) ;
    return tmp ;
  }

  //: Returns the index of the edge "arg".
  vmap_edge_index index (const edge_type & arg) const
  {
    return arg.sequence_index();//-(*_begin_edge)->index() ;
  }

  //: Returns the face of index "arg".
  const_face_reference face(vmap_face_index arg) const
  {
    face_type tmp;
    tmp.set(get_face_pointer(arg),index()) ;
    return tmp ;
  }

  //: Returns the face of index "arg".
  face_reference face(vmap_face_index arg)
  {
    face_type tmp;
    tmp.set(get_face_pointer(arg),index()) ;
    return tmp ;
  }

  //: Returns the index of the face "arg".
  vmap_face_index index (const face_type & arg) const
  {
    return arg.sequence_index() ;//-(*_begin_face)->index() ;
  }

  //: Returns the index of the vertex associated to the dart "arg".
  vmap_vertex_index dart_associated_vertex(vmap_dart_index arg) const
  {
    return get_dart_pointer(arg)->vertex(index()).sequence_index() ;
  }

  //: Returns the index of the dart associated to the vertex "arg".
  vmap_dart_index vertex_associated_dart(vmap_vertex_index arg) const
  {
    return get_vertex_pointer(arg)->begin(index())->sequence_index() ;
  }

  //: Returns the index of the edge associated to the dart "arg".
  vmap_edge_index dart_associated_edge(vmap_dart_index arg) const
  {
    return get_dart_pointer(arg)->edge(index()).sequence_index() ;
  }

  //: Returns the index of the dart associated to the edge "arg".
  vmap_dart_index edge_associated_dart(vmap_edge_index arg) const
  {
    return get_edge_pointer(arg)->begin(index())->sequence_index() ;
  }

  //: Returns the index of the vertex associated to the dart "arg".
  vmap_face_index dart_associated_face(vmap_dart_index arg) const
  {
    return get_dart_pointer(arg)->face(index()).sequence_index() ;
  }

  //: Returns the index of the dart associated to the face "arg".
  vmap_dart_index face_associated_dart(vmap_face_index arg) const
  {
    return get_face_pointer(arg)->begin(index())->sequence_index() ;
  }

  //: Returns "true" if the map is empty.
  bool empty() const
  {
    return nb_darts ()==0 ;
  }

  //:
  virtual void contraction(const contraction_kernel &arg_kernel) ;

  //: Removes the darts of arg_kernel. The darts are required beforehand to form a representativeed forest of faces.
  virtual void removal(const removal_kernel &arg_kernel) ;


 protected:

  //:
  pyramid_type & pyramid()
  {
    return (pyramid_type &)*_pyramid ;
  }

 protected:

  //:
  vertex_sequence_iterator begin_vertex_sequence()
  {
    return vertex_sequence::begin() ;
  }

  //:
  vertex_sequence_iterator end_vertex_sequence()
  {
    return vertex_sequence::end() ;
  }

  //:
  const_vertex_sequence_iterator begin_vertex_sequence() const
  {
    return vertex_sequence::begin() ;
  }

  //:
  const_vertex_sequence_iterator end_vertex_sequence() const
  {
    return vertex_sequence::end() ;
  }

  //:
  vertex_pointer & get_vertex_pointer(vmap_vertex_index arg)
  {
    return vertex_sequence::get_pointer(arg) ;
  }

  //:
  const vertex_pointer & get_vertex_pointer(vmap_vertex_index arg) const
  {
    return vertex_sequence::get_pointer(arg) ;
  }

  //:
  edge_sequence_iterator begin_edge_sequence()
  {
    return edge_sequence::begin();
  }

  //:
  edge_sequence_iterator end_edge_sequence()
  {
    return edge_sequence::end() ;
  }

  //:
  const_edge_sequence_iterator begin_edge_sequence() const
  {
    return edge_sequence::begin() ;
  }

  //:
  const_edge_sequence_iterator end_edge_sequence() const
  {
    return edge_sequence::end() ;
  }

  //:
  edge_pointer & get_edge_pointer(vmap_edge_index arg)
  {
    return edge_sequence::get_pointer(arg) ;
  }

  //:
  const edge_pointer & get_edge_pointer(vmap_edge_index arg) const
  {
    return edge_sequence::get_pointer(arg) ;
  }

  //:
  face_sequence_iterator begin_face_sequence()
  {
    return face_sequence::begin() ;
  }

  //:
  face_sequence_iterator end_face_sequence()
  {
    return face_sequence::end() ;
  }

  //:
  const_face_sequence_iterator begin_face_sequence() const
  {
    return face_sequence::begin() ;
  }

  //:
  const_face_sequence_iterator end_face_sequence() const
  {
    return face_sequence::end() ;
  }

  //:
  face_pointer & get_face_pointer(vmap_face_index arg)
  {
    return face_sequence::get_pointer(arg) ;
  }

  //:
  const face_pointer & get_face_pointer(vmap_face_index arg) const
  {
    return face_sequence::get_pointer(arg) ;
  }
};

#include "vpyr_2_tpyramid_level.txx"

#endif
