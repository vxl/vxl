// This is contrib/prip/vmap/vmap_2_tmap.h
#ifndef vmap_2_tmap_h_
#define vmap_2_tmap_h_
//:
// \file
// \brief provides the topological map class.
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  06 May 2004 Jocelyn Marchadier
// \endverbatim

#include "vmap_2_map.h"
#include "vmap_2_tmap_elts.h"

//: Enables to explicitly manipulate darts, but also vertices, edges, faces of a topological partition.
template <class V=vmap_2_tmap_vertex, class E=vmap_2_tmap_edge, class F=vmap_2_tmap_face, class D=vmap_2_tmap_dart>
class vmap_2_tmap: public vmap_2_map<D>,
      public vmap_owning_sequence<V>,
      public vmap_owning_sequence<E>,
      public vmap_owning_sequence<F>
{
public:

  //:
  typedef vmap_2_tmap<V,E,F,D> self_type ;  
  //:
  static vmap_2_tmap_tag tag ;

  //:
  typedef vmap_2_map<D> _Base ;

protected:  
	// -- vertex sequence --
  
  //:
  typedef vmap_owning_sequence<V> vertex_sequence ;
  //:
  typedef typename vertex_sequence::iterator vertex_sequence_iterator;
  //:
  typedef typename vertex_sequence::const_iterator const_vertex_sequence_iterator;
  //:
  typedef typename vertex_sequence::pointer vertex_pointer ;  

  // -- edge sequence --
  
  //:
  typedef vmap_owning_sequence<E> edge_sequence ;
  //:
  typedef typename edge_sequence::iterator edge_sequence_iterator;
  //:
  typedef typename edge_sequence::const_iterator const_edge_sequence_iterator;
  //:
  typedef typename edge_sequence::pointer edge_pointer ;
  
  // -- face sequence --
  
  //:
  typedef vmap_owning_sequence<F> face_sequence ;
  //:
  typedef typename face_sequence::iterator face_sequence_iterator;
  //:
  typedef typename face_sequence::const_iterator const_face_sequence_iterator;
  //:
  typedef typename face_sequence::pointer face_pointer ;
  
  // -- dart sequence --
  
  //:
  typedef typename _Base::dart_sequence_iterator dart_sequence_iterator ;  
  //:
  typedef typename _Base::const_dart_sequence_iterator const_dart_sequence_iterator ;  
  //:
  typedef typename _Base::dart_pointer dart_pointer ;  

public:  
  
	// -- Vertex types --
  
  //: The type of vertices of the map.
  typedef V vertex_type ;
  //: reference of a vertex.
  typedef vertex_type& vertex_reference ;
  //: const reference of a vertex.
  typedef const vertex_type& const_vertex_reference ;
  //: A vertex_iterator iterates on a sequence of vertices.
  typedef vmap_ptr_iterator_wrapper<vertex_type,vertex_type&,vertex_type*,vertex_sequence_iterator> vertex_iterator ;
  //: A const_vertex_iterator iterates on a read-only sequence of vertices.
  typedef vmap_ptr_iterator_wrapper<vertex_type,const vertex_type&,const vertex_type*,const_vertex_sequence_iterator> const_vertex_iterator ;

  // -- Edge types --
  
  //:
  typedef E edge_type ;
  //: reference of an edge.
  typedef edge_type& edge_reference ;
  //: const reference of an edge.
  typedef const edge_type& const_edge_reference ;
  //: An edge_iterator iterates on a sequence of edges.
  typedef vmap_ptr_iterator_wrapper<edge_type,edge_type&,edge_type*,edge_sequence_iterator> edge_iterator ;
  //: A const_edge_iterator iterates on a read-only sequence of edges.
  typedef vmap_ptr_iterator_wrapper<edge_type,const edge_type&,const edge_type*,const_edge_sequence_iterator> const_edge_iterator ;
  
  // -- Face types --
  
  //:The type of face
  typedef F face_type ;
  //: reference of a face.
  typedef face_type& face_reference ;
  //: const reference of a face.
  typedef const face_type& const_face_reference ;
  //: A face_iterator iterates on the sequence of faces.
  typedef vmap_ptr_iterator_wrapper<face_type,face_type&,face_type*,face_sequence_iterator> face_iterator ;
  //: A const_face_iterator iterates on the read-only sequence of faces.
  typedef vmap_ptr_iterator_wrapper<face_type,const face_type&,const face_type*,const_face_sequence_iterator> const_face_iterator ;

  // -- Dart types --
  
  //: the type of dart of this map.
  typedef typename _Base::dart_type dart_type ;
  //: reference on a dart.
  typedef dart_type& dart_reference ;
  //: const reference on a dart.
  typedef const dart_type& const_dart_reference ;
  //:An iterator iterates on a sequence of darts and on the topology.
  typedef typename _Base::dart_iterator dart_iterator ;
  //:An iterator iterates on a sequence of darts and on the topology.
  typedef typename _Base::const_dart_iterator const_dart_iterator ;
  
  //: Kernel class for contraction.
  typedef vmap_permutation_kernel< vmap_sigma_permutation<self_type> > contraction_kernel ;
  //: Kernel class for contraction.
  typedef vmap_permutation_kernel< vmap_phi_permutation<self_type> > removal_kernel ;

	friend class vmap_kernel<self_type> ;
	friend class vmap_sigma_permutation<self_type> ;
	friend class vmap_phi_permutation<self_type> ;
	

  
	// -- casts --
  
  //:
  static vertex_type& cast(vmap_2_tmap_vertex & a)
  {
    return (vertex_type&) a ;
  }

  //:
  static edge_type& cast(vmap_2_tmap_edge & a)
  {
    return (edge_type&) a ;
  }

  //:
  static face_type& cast(vmap_2_tmap_face & a)
  {
    return (face_type&) a ;
  }

  //:
  static const vertex_type& cast(const vmap_2_tmap_vertex & a)
  {
    return (const vertex_type&) a ;
  }

  //:
  static const edge_type& cast(const vmap_2_tmap_edge & a)
  {
    return (const edge_type&) a ;
  }

  //:
  static const face_type& cast(const vmap_2_tmap_face & a)
  {
    return (const face_type&) a ;
  }

  //:
  static dart_type& cast(vmap_2_tmap_dart & a)
  {
    return (dart_type&) a ;
  }

  //:
  static const dart_type& cast(const vmap_2_tmap_dart & a)
  {
    return (const dart_type&) a ;
  }


public:
  
  //:
  vmap_2_tmap()
  {}

  //:
  vmap_2_tmap(const self_type &right) ;

  //:
  virtual ~vmap_2_tmap();

  //:
  self_type & operator=(const self_type &right);


  //:
  template <class M>
  void set_structure(const M &right)
  {
    set_structure(right,M::tag) ;
  }

  //: Returns the index of the vertex "arg".
  vmap_dart_index index (const vmap_2_tmap_dart & arg) const
  {
    return arg.sequence_index();
  }

	//: Returns the index of a dart in the initial sequence
  int position(const vmap_2_tmap_dart & arg) const
	{
		return _Base::position(arg) ;
	}

	//: Returns the index of a dart in the initial sequence
  int dart_position(vmap_dart_index i) const
	{
		return _Base::position(dart(i)) ;
	}

  //: Returns the index of the vertex "arg".
  vmap_vertex_index index (const vmap_2_tmap_vertex & arg) const
  {
    return arg.sequence_index();
  }

	//: Returns the index of a vertex in the initial sequence
	int position(const vmap_2_tmap_vertex & arg) const
	{
		return vertex_sequence::position(arg) ;
	}

	//: Returns the index of a vertex in the initial sequence
	int vertex_position(vmap_vertex_index i) const
	{
		return vertex_sequence::position(vertex(i)) ;
	}

  /*edge_iterator find_edge(const vertex_type &arg1,const vertex_type & arg2) ;
  edge_iterator find_edge(int arg_vertex1,int arg_vertex2)
  {
  return find_edge(vertex(arg_vertex1),vertex(arg_vertex2));
  }*/

  //: Returns the index of the edge "arg".
  vmap_edge_index index (const vmap_2_tmap_edge & arg) const
  {
    return arg.sequence_index();
  }

	//: Returns the index of an edge in the initial sequence
	int position(const vmap_2_tmap_edge & arg) const
	{
		return edge_sequence::position(arg) ;
	}

	//: Returns the index of an edge in the initial sequence
	int edge_position(vmap_edge_index i) const
	{
		return edge_sequence::position(edge(i)) ;
	}


  //: Returns the index of the face "arg".
  vmap_face_index index (const vmap_2_tmap_face & arg) const
  {
    return arg.sequence_index();
  }
	
	//: Returns the index of a face in the initial sequence
	int position(const vmap_2_tmap_face & arg) const
	{
		return face_sequence::position(arg) ;
	}

	//: Returns the index of a face in the initial sequence
	int face_position(vmap_face_index i) const
	{
		return face_sequence::position(face(i)) ;
	}

  //: Returns an iterator on the first vertex.
  const_vertex_iterator begin_vertex () const
  {
    return begin_vertex_sequence() ;
  }

  //:
  vertex_iterator begin_vertex ()
  {
    return begin_vertex_sequence() ;
  }

  //: Returns an iterator after the end vertex.
  const_vertex_iterator end_vertex() const
  {
    return end_vertex_sequence() ;
  }

  //:
  vertex_iterator end_vertex()
  {
    return end_vertex_sequence() ;
  }

  //: Returns an iterator on the first edge.
  const_edge_iterator begin_edge () const
  {
    return begin_edge_sequence() ;
  }

  //:
  edge_iterator begin_edge ()
  {
    return begin_edge_sequence() ;
  }

  //: Returns an iterator after the end edge.
  const_edge_iterator end_edge() const
  {
    return end_edge_sequence() ;
  }

  //:
  edge_iterator end_edge()
  {
    return end_edge_sequence() ;
  }

  //: Returns an iterator on the first edge.
  const_face_iterator begin_face () const
  {
    return begin_face_sequence() ;
  }

  //:
  face_iterator begin_face ()
  {
    return begin_face_sequence() ;
  }

  //: Returns an iterator after the end edge of level at least "i".
  const_face_iterator end_face() const
  {
    return end_face_sequence() ;
  }

  //:
  face_iterator end_face()
  {
    return end_face_sequence() ;
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
    return *get_vertex_pointer(arg) ;
  }

  //: Returns the vertex of index "arg".
  vertex_reference vertex(vmap_vertex_index arg)
  {
    return *get_vertex_pointer(arg) ;
  }

  //: Returns the edge of index "arg".
  const_edge_reference edge(vmap_edge_index arg) const
  {
    return *get_edge_pointer(arg) ;
  }

  //: Returns the edge of index "arg".
  edge_reference edge(vmap_edge_index arg)
  {
    return *get_edge_pointer(arg) ;
  }

  /*edge_iterator find_edge(const vertex_type &arg1,const vertex_type & arg2) ;
  edge_iterator find_edge(int arg_vertex1,int arg_vertex2)
  {
  return find_edge(vertex(arg_vertex1),vertex(arg_vertex2));
  }
  */

  //: Returns the face of index "arg".
  const_face_reference face(vmap_face_index arg) const
  {
    return *get_face_pointer(arg) ;
  }

  //: Returns the face of index "arg".
  face_reference face(vmap_face_index arg)
  {
    return *get_face_pointer(arg) ;
  }

  //:
  virtual void contraction(const contraction_kernel &arg_kernel) ;

  //:
  virtual void removal(const removal_kernel &arg_kernel);

  //: Reserves the memory with minimum initialization,
  //  without linking elements.
  void initialise(int arg_nb_vertices, int arg_nb_edges, int arg_nb_faces) ;

  //: Reserves only vertices.
  void initialise_vertices(int arg_nb_vertices) ;

  //: Reserves only edges.
  void initialise_edges(int arg_nb_edges) ;

  //: Reserves faces.
  void initialise_faces(int arg_nb_faces);

  //: Returns the index of the vertex associated to the dart "arg".
  vmap_vertex_index dart_associated_vertex(vmap_dart_index arg) const
  {
    return index(dart(arg).vertex()) ;
  }

  //: Returns the index of the dart associated to the vertex "arg".
  vmap_dart_index vertex_associated_dart(vmap_vertex_index arg) const
  {
    return index(*vertex(arg).begin()) ;
  }

  //: Returns the index of the edge associated to the dart "arg".
  vmap_edge_index dart_associated_edge(vmap_dart_index arg) const
  {
    return index(dart(arg).edge()) ;
  }
  
  //: Returns the index of the dart associated to the edge "arg".
  vmap_dart_index edge_associated_dart(vmap_edge_index arg) const
  {
    return index(*edge(arg).begin()) ;
  }

  //: Returns the index of the vertex associated to the dart "arg".
  vmap_face_index dart_associated_face(vmap_dart_index arg) const
  {
    return index(dart(arg).face()) ;
  }
  
  //: Returns the index of the dart associated to the face "arg".
  vmap_dart_index face_associated_dart(vmap_face_index arg) const
  {
    return index(*face(arg).begin()) ;
  }

  //: Sets "arg_vertex" as the vertex of "arg".
  void set_vertex (vmap_dart_index arg, vmap_vertex_index arg_vertex) ;

  //: Sets "arg_edge" as the edge of "arg".
  void set_edge(vmap_dart_index arg, vmap_edge_index arg_edge) ;

  //: Sets "arg_face" as the face of "arg".
  void set_face (vmap_dart_index arg, vmap_face_index arg_face) ;

  //: One dart. Hard to use : alpha dart = nb_darts()-1-arg.
  void set_dart(vmap_dart_index arg, vmap_dart_index arg_sigma, vmap_vertex_index arg_vertex, vmap_face_index arg_face) ;

  //: One dart. Hard to use : alpha dart = nb_darts()-1-arg.
  void set_dart(vmap_dart_index arg, vmap_dart_index arg_sigma, vmap_dart_index arg_alpha, vmap_vertex_index arg_vertex, vmap_edge_index arg_edge, vmap_face_index arg_face) ;

  //: Initializes the face and vertex permutations from sigma.
  void set_all_cycles() ;

  //: Initializes the vertices from sigma.
  void set_vertex_cycles() ;

  //: Initializes the edges from alpha.
  void set_edge_cycles() ;

  //: Initializes the faces from sigma and alpha.
  void set_face_cycles() ;

  //: Returns true if all the permutations are valid, false otherwise.
  //  May be usefull for testing permutations set by hand...
  bool valid_permutations() const ;
  
  //:
  virtual bool valid()const ;

  //: Deletes everything.
  virtual void clear() ;

  //: 
  void removal(dart_iterator & arg) ;
  
  //:
  void contraction(dart_iterator & arg) ;

  //: Initializes the sturcture of the combinatorial map from "stream".
  void read_structure(vcl_istream & stream) ;

  //: Writes the sturcture of the combinatorial map to "stream".
  void write_structure(vcl_ostream & stream) const ;

protected:

  //:
  template <class M>
  void set_structure(const M &right, vmap_2_tmap_tag tag);

  //:
  template <class M >
  void set_structure(const M &right, vmap_2_map_tag tag);

  //:
  void suppress_from_sequence(vertex_type * v) ;
  
  //:
  void suppress_from_sequence(edge_type * v) ;
  
  //:
  void suppress_from_sequence(face_type * v) ;

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


#include "vmap_2_tmap.txx"

#endif
