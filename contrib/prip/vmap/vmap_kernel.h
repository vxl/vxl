// This is contrib/prip/vmap/vmap_kernel.h
#ifndef vmap_kernel_h_
#define vmap_kernel_h_

//:
// \file
// \brief provides the base kernel classes which can be used to contract/remove edges from any map.
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  06 May 2004 Jocelyn Marchadier
// \endverbatim

#include "vcl_vector.h"
#include "vmap_types.h"
#include "vbl_controlled_partition.h"

//: For manipulating.
template <class TMap>
class vmap_sigma_permutation
{
public:
  
  //:
  typedef TMap map_type ;
  
  //:
  typedef typename map_type::dart_iterator element_iterator ;
  
	//:
  typedef typename map_type::dart_pointer element_pointer ;
  //typedef typename map_type::vmap_dart_index vmap_dart_index ;

	 //:
  typedef //typename map_type::
  vmap_dart_index element_index ;
  
  //:
  typedef typename map_type::vertex_type cycle_type ; // the linked elements
  
  //:  
  typedef typename map_type::vertex_iterator cycle_iterator ; // the linked elements
  
  //:  
  typedef typename map_type::vertex_pointer cycle_pointer ; // the linked elements
  
  //:
  typedef //typename map_type::
  vmap_vertex_index cycle_index ;

	/*element_reference cycle(const dart_iterator & arg)
	{
		return arg->vertex() ;
	}
	
	element_reference opposite(const dart_iterator & arg)
	{
		dart_iterator a=arg ; a.alpha() ;
		return a->vertex() ;
	}
	*/
	vmap_sigma_permutation(map_type & arg)
		:_map(&arg) 
	{}
	
	cycle_index cycle_index_of(const element_iterator & arg) const
	{
		return arg->vertex().sequence_index() ;
	}
	
	element_index index_of(const element_iterator & arg) const
	{
		return arg->sequence_index() ;
	}
	
	cycle_index index_of(const cycle_iterator & arg) const
	{
		return arg->sequence_index() ;
	}
	
	cycle_index opposite_cycle_index_of(const element_iterator & arg) const
	{
		element_iterator a=arg ; a.alpha() ;
		return a->vertex().sequence_index() ;
	}
	
	cycle_pointer cycle_pointer_of(const element_iterator & arg) const
	{
		return get_cycle_pointer(cycle_index_of(arg)) ;
	}
	
	cycle_pointer opposite_cycle_pointer_of(const element_iterator & arg) const
	{
		return get_cycle_pointer(opposite_cycle_index_of(arg)) ;
	}
	
	//:
  void opposite(element_iterator & arg) const
  {
    arg.alpha() ;
  }

	//:
  void next_on_cycle(element_iterator & arg) const
  {
    arg.sigma() ;
  }

  //:
  void previous_on_cycle(element_iterator & arg) const
  {
    arg.isigma() ;
  }
	
	  //:
  cycle_iterator begin_cycle() const
  {
    return _map->begin_vertex() ;
  }

  //:
  cycle_iterator end_cycle() const
  {
    return _map->end_vertex() ;
  }

  //:
  int nb_cycles() const
  {
    return _map->nb_vertices() ;
  }
	
	//:
	cycle_pointer get_cycle_pointer(cycle_index i) const
	{
		return _map->get_vertex_pointer(i) ;
	} 
	
  //:
  int nb_elements() const
  {
    return _map->nb_darts() ;
  }

	//:
	element_pointer get_element_pointer(element_index i) const
	{
		return _map->get_dart_pointer(i) ;
	} 
	
private :
	//:
  map_type *_map ;

} ;
template <class TMap>
class vmap_phi_permutation
{
public:
  
  //:
  typedef TMap map_type ;
  
  //:
  typedef typename map_type::dart_iterator element_iterator ;
  //typedef typename map_type::vmap_dart_index vmap_dart_index ;
  
	//:
  typedef typename map_type::dart_pointer element_pointer ;
	 
	 //:
  typedef //typename map_type::
  vmap_dart_index element_index ;
  
  //:
  typedef typename map_type::face_type cycle_type ; // the linked elements
  
  //:  
  typedef typename map_type::face_pointer cycle_pointer ; // the linked elements
  
	//:  
  typedef typename map_type::face_iterator cycle_iterator ; // the linked elements
  
  //:
  typedef //typename map_type::
  vmap_face_index cycle_index ;

	/*element_reference cycle(const dart_iterator & arg)
	{
		return arg->vertex() ;
	}
	
	element_reference opposite(const dart_iterator & arg)
	{
		dart_iterator a=arg ; a.alpha() ;
		return a->vertex() ;
	}
	*/
	vmap_phi_permutation(map_type & arg)
		:_map(&arg) 
	{}
	
	cycle_index cycle_index_of(const element_iterator & arg) const
	{
		return arg->face().sequence_index() ;
	}
	
	element_index index_of(const element_iterator & arg) const
	{
		return arg->sequence_index() ;
	}
	
	cycle_index index_of(const cycle_iterator & arg) const
	{
		return arg->sequence_index() ;
	}
	
	cycle_index opposite_cycle_index_of(const element_iterator & arg) const
	{
		element_iterator a=arg ; a.alpha() ;
		return a->face().sequence_index() ;
	}
	
	//:
  void opposite(element_iterator & arg) const
  {
    arg.alpha() ;
  }

	//:
  void next_on_cycle(element_iterator & arg) const
  {
    arg.phi() ;
  }

  //:
  void previous_on_cycle(element_iterator & arg) const
  {
    arg.iphi() ;
  }
	
	//:
  cycle_iterator begin_cycle() const
  {
    return _map->begin_face() ;
  }

  //:
  cycle_iterator end_cycle() const
  {
    return _map->end_face() ;
  }

  //:
  int nb_cycles() const
  {
    return _map->nb_faces() ;
  }
	
	//:
	cycle_pointer get_cycle_pointer(cycle_index i) const
	{
		return _map->get_face_pointer(i) ;
	} 
	
  //:
  int nb_elements() const
  {
    return _map->nb_darts() ;
  }

	//:
	element_pointer get_element_pointer(element_index i) const
	{
		return _map->get_dart_pointer(i) ;
	} 
	
	cycle_pointer cycle_pointer_of(const element_iterator & arg) const
	{
		return get_cycle_pointer(cycle_index_of(arg)) ;
	}
	
	cycle_pointer opposite_cycle_pointer_of(const element_iterator & arg) const
	{
		return get_cycle_pointer(opposite_cycle_index_of(arg)) ;
	}
	

private :
	//:
  map_type *_map ;

} ;

//:	Kernel class for contraction/removal.
//	A kernel is a directed tree, that links elements
//	of a permutation with darts such that the element
//	associated to an added dart "d" is contracted to the element
//	associated to alpha(d).
// 
//	This class is used as a base class for all kernels.
//	This is the minimal structure for writing contraction/removal
//	functions. It defines just the interface, and does not
//	check the validity of the added elements.
//	It can be used in algorithms constructing directely trees.
 //
//	For efficiency reasons, none of its fucntions should be virtual.
//	That has for consequence that it must be a fully instanciated
//	type when used or used in template fcts.
template <class M>
class vmap_kernel
{
public :
  //:
  typedef M map_type ;
  
  //:
  typedef typename map_type::dart_iterator dart_iterator ;
  
  //:
  typedef typename map_type::dart_pointer dart_pointer ;
  
	 //:
  typedef //typename map_type::
  vmap_dart_index dart_index ;
  
private:
  //:
  typedef std::vector<dart_pointer> dart_collection ;

public:
  //:
  typedef typename dart_collection::const_iterator const_iterator ;
  
  //:
  typedef typename dart_collection::iterator iterator ;

  //:
  vmap_kernel (map_type & arg)
  {
    _map=&arg ;
  }

  //: To be called before any addion/test. 
  // It initialises internal structures.
  virtual void initialise()
  {}

  //: To be called after all the additions. 
  // For example, it can be used to
  //   give a representative to a non oriented tree.
  //   After the call of this function, the kernel should be a representativeed
  //   tree that can be used for contraction/removal.
  virtual void finalise()
  {}

  ///: Inserts a new dart pointed by "arg".
  bool add(const dart_iterator & arg)
  {
    _dart.push_back(*arg.reference()) ;
    return true ;
  }

  //: Inserts a new dart.
  bool add(vmap_dart_index arg)
  {
    return add(dart_iterator_on(arg)) ;
  }

  //:
  void clear()
  {
    _dart.clear() ;
  }

  //:
  iterator begin()
  {
    return _dart.begin() ;
  }

  //:
  iterator end()
  {
    return _dart.end() ;
  }

  //:
  const_iterator begin() const
  {
    return _dart.begin() ;
  }

  //:
  const_iterator end() const
  {
    return _dart.end() ;
  }

  //:
  dart_iterator dart(int i) const
  {
    return _map->begin_dart()+_dart[i]->sequence_index() ;
  }

   //:
  vmap_dart_index get_dart_index(int i) const
  {
    return _dart[i]->sequence_index() ;
  }

  //:
  int size() const
  {
    return _dart.size() ;
  }
	
protected :
  
	//:
  dart_iterator dart_iterator_on(vmap_dart_index arg) const
  {
    return _map->begin_dart()+arg ;
  }
	
  //:
  map_type *_map ;
  
  //:
  dart_collection _dart;

} ;

//: The base class for forests of vertices.
// It can be defined only on tmaps.
template <class TPermutation>
class vmap_permutation_kernel : public vmap_kernel<typename TPermutation::map_type>
{
public:
  
	//:
  typedef typename TPermutation::map_type map_type ;
  
	//:
  typedef vmap_kernel<map_type> _Base ;
  
  //:
  typedef TPermutation permutation_type ;
  
  //:
  typedef typename permutation_type::element_iterator dart_iterator ;
	 
	//:
  typedef typename permutation_type::element_index dart_index ;
	  
	//:
  typedef typename permutation_type::cycle_type element_type ; // the linked elements
  
  //:
  typedef typename permutation_type::cycle_pointer element_pointer ; // the linked elements
  
  //:  
  typedef typename permutation_type::cycle_iterator element_iterator ; // the linked elements
  
  //:
  typedef typename permutation_type::cycle_index element_index ;
  
	//:
  vmap_permutation_kernel(map_type &arg)
      : _Base(arg),
			  _p(arg)
  {
		_representatives.initialise(_p.nb_cycles()) ;
	}

	//: Before any addion/test
  void initialise() ;

  //: After addions
  void finalise()
  {}

  //:
  bool can_insert(const dart_iterator & arg) const ;
	
	//:
  bool add(const dart_iterator & arg) ;
	
  //:
  bool add(vmap_dart_index arg)
  {
    return add(dart_iterator_on(arg)) ;
  }

  //:
  void clear()
  {
    _Base::clear() ;
    _representatives.clear() ;
		_dart_associated_elt.clear() ;
  }
	
  //: Adds all the (recursive) pendant darts of the current map to the kernel.
  void add_1_cycles() ;
  
  //: Adds all the (recursive) redundant darts of the current map to the kernel.
  void add_2_cycles() ;

	//: Returns true if the kernel is a valid kernel.
	bool valid() const ;
	
  permutation_type & permutation()
	{
    return _p ;
  }

	element_index father_index(element_index arg) const
	{
		return _representatives.representative(arg);
	}

	//:
  element_index get_element_index(int i) const
  {
    return _dart_associated_elt[i]->sequence_index() ;
  }

	template <class TPp>
	void operator=(const vmap_permutation_kernel<TPp> & arg)
	{
		_dart.resize(arg.size()) ;
		_dart_associated_elt.resize(arg.size());
		for (int i=0; i<arg.size(); i++)
		{
			_dart[i]=_p.get_element_pointer(arg.get_dart_index(i)) ;
			_dart_associated_elt[i]=_p.get_cycle_pointer(arg.get_element_index(i)) ;
		}
		_representatives=arg.representatives() ;
		
	}

	const vbl_controlled_partition & representatives() const
	{
		return _representatives;
	}
	
protected :
  
	//:
  permutation_type _p ;

  //: The representative of "root" is the new representative of the set to which "j" is the representative.
  void union_of(element_pointer root, element_pointer j)
  {
     _father[representative(j)->sequence_index()]= _father[representative(root)->sequence_index()] ;
  }
	  
	//:
  vcl_vector<element_pointer> _dart_associated_elt ;
	
	//:
	vbl_controlled_partition _representatives;

} ;


#include "vmap_kernel.txx"

#endif
