// This is prip/vmap/vmap_kernel.h
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

#include <vcl_vector.h>
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
#if 0
  typedef typename map_type::vmap_dart_index vmap_dart_index ;
#endif // 0

  //:
  typedef vmap_dart_index element_index ;
#if 0
  typedef typename map_type::vmap_dart_index element_index ;
#endif // 0

  //:
  typedef typename map_type::vertex_type cycle_type ; // the linked elements

  //:
  typedef typename map_type::vertex_iterator cycle_iterator ; // the linked elements

  //:
  typedef typename map_type::vertex_pointer cycle_pointer ; // the linked elements

  //:
  typedef vmap_vertex_index cycle_index ;
#if 0
  typedef typename map_type::vmap_vertex_index cycle_index ;
#endif // 0

#if 0
  element_reference cycle(const dart_iterator & arg)
  {
    return arg->vertex() ;
  }

  element_reference opposite(const dart_iterator & arg)
  {
    dart_iterator a=arg ; a.alpha() ;
    return a->vertex() ;
  }
#endif // 0

  vmap_sigma_permutation(map_type & arg) :map_(&arg) {}

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
    return map_->begin_vertex() ;
  }

  //:
  cycle_iterator end_cycle() const
  {
    return map_->end_vertex() ;
  }

  //:
  int nb_cycles() const
  {
    return map_->nb_vertices() ;
  }

  //:
  cycle_pointer get_cycle_pointer(cycle_index i) const
  {
    return map_->get_vertex_pointer(i) ;
  }

  //:
  int nb_elements() const
  {
    return map_->nb_darts() ;
  }

  //:
  element_pointer get_element_pointer(element_index i) const
  {
    return map_->get_dart_pointer(i) ;
  }

 private :
  //:
  map_type *map_ ;
};

template <class TMap>
class vmap_phi_permutation
{
 public:

  //:
  typedef TMap map_type ;

  //:
  typedef typename map_type::dart_iterator element_iterator ;
#if 0
  typedef typename map_type::vmap_dart_index vmap_dart_index ;
#endif // 0

  //:
  typedef typename map_type::dart_pointer element_pointer ;

  //:
  typedef vmap_dart_index element_index ;
#if 0
  typedef typename map_type::vmap_dart_index element_index ;
#endif // 0

  //:
  typedef typename map_type::face_type cycle_type ; // the linked elements

  //:
  typedef typename map_type::face_pointer cycle_pointer ; // the linked elements

  //:
  typedef typename map_type::face_iterator cycle_iterator ; // the linked elements

  //:
  typedef vmap_face_index cycle_index ;
#if 0
  typedef typename map_type::vmap_face_index cycle_index ;
#endif // 0

#if 0
  element_reference cycle(const dart_iterator & arg)
  {
    return arg->vertex() ;
  }

  element_reference opposite(const dart_iterator & arg)
  {
    dart_iterator a=arg ; a.alpha() ;
    return a->vertex() ;
  }
#endif // 0

  vmap_phi_permutation(map_type & arg) :map_(&arg) {}

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
    return map_->begin_face() ;
  }

  //:
  cycle_iterator end_cycle() const
  {
    return map_->end_face() ;
  }

  //:
  int nb_cycles() const
  {
    return map_->nb_faces() ;
  }

  //:
  cycle_pointer get_cycle_pointer(cycle_index i) const
  {
    return map_->get_face_pointer(i) ;
  }

  //:
  int nb_elements() const
  {
    return map_->nb_darts() ;
  }

  //:
  element_pointer get_element_pointer(element_index i) const
  {
    return map_->get_dart_pointer(i) ;
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
  map_type *map_ ;
};

//:  Kernel class for contraction/removal.
//  A kernel is a directed tree, that links elements
//  of a permutation with darts such that the element
//  associated to an added dart "d" is contracted to the element
//  associated to alpha(d).
//
//  This class is used as a base class for all kernels.
//  This is the minimal structure for writing contraction/removal
//  functions. It defines just the interface, and does not
//  check the validity of the added elements.
//  It can be used in algorithms constructing directely trees.
//
//  For efficiency reasons, none of its functions should be virtual.
//  That has for consequence that it must be a fully instantiated
//  type when used or used in template fcts.
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
  typedef vmap_dart_index dart_index ;
#if 0
  typedef typename map_type::vmap_dart_index dart_index ;
#endif // 0

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
    map_=&arg ;
  }

  //: To be called before any addition/test.
  // It initialises internal structures.
  void initialise() {}

  //: To be called after all the additions.
  // For example, it can be used to
  //   give a representative to a non oriented tree.
  //   After the call of this function, the kernel should be a representativeed
  //   tree that can be used for contraction/removal.
  void finalise() {}

  //: Inserts a new dart pointed by "arg".
  bool add(const dart_iterator & arg)
  {
    dart_.push_back(*arg.reference()) ;
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
    dart_.clear() ;
  }

  //:
  iterator begin()
  {
    return dart_.begin() ;
  }

  //:
  iterator end()
  {
    return dart_.end() ;
  }

  //:
  const_iterator begin() const
  {
    return dart_.begin() ;
  }

  //:
  const_iterator end() const
  {
    return dart_.end() ;
  }

  //:
  dart_iterator dart(int i) const
  {
    return map_->begin_dart()+dart_[i]->sequence_index() ;
  }

  //:
  vmap_dart_index get_dart_index(int i) const
  {
    return dart_[i]->sequence_index() ;
  }

  //:
  unsigned int size() const
  {
    return dart_.size() ;
  }

 protected :

  //:
  dart_iterator dart_iterator_on(vmap_dart_index arg) const
  {
    return map_->begin_dart()+arg ;
  }

  //:
  map_type *map_ ;

  //:
  dart_collection dart_;
};

//: The base class for forests of vertices.
// It can be defined only on tmaps.
template <class TPermutation>
class vmap_permutation_kernel : public vmap_kernel<typename TPermutation::map_type>
{
 public:

  //:
  typedef typename TPermutation::map_type map_type ;

  //:
  typedef vmap_kernel<map_type> Base_ ;

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
    : Base_(arg),
      p_(arg)
  {
    representatives_.initialise(p_.nb_cycles()) ;
  }

  //: Before any addition/test
  void initialise() ;

  //: After additions
  void finalise() {}

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
    Base_::clear() ;
    representatives_.clear() ;
    dart_associated_elt_.clear() ;
  }

  //: Adds all the (recursive) pendant darts of the current map to the kernel.
  void add_1_cycles() ;

  //: Adds all the (recursive) redundant darts of the current map to the kernel.
  void add_2_cycles() ;

  //: Returns true if the kernel is a valid kernel.
  bool valid() const ;

  permutation_type & permutation()
  {
    return p_ ;
  }

  element_index father_index(element_index arg) const
  {
    return representatives_.representative(arg);
  }

  //:
  element_index get_element_index(int i) const
  {
    return dart_associated_elt_[i]->sequence_index() ;
  }

  template <class TPp>
  void operator=(const vmap_permutation_kernel<TPp> & arg)
  {
    dart_.resize(arg.size()) ;
    dart_associated_elt_.resize(arg.size());
    for (unsigned int i=0; i<arg.size(); ++i)
    {
      dart_[i]=p_.get_element_pointer(arg.get_dart_index(i)) ;
      dart_associated_elt_[i]=p_.get_cycle_pointer(arg.get_element_index(i)) ;
    }
    representatives_=arg.representatives() ;
  }

  const vbl_controlled_partition & representatives() const
  {
    return representatives_;
  }

 protected :

  //:
  permutation_type p_ ;

  //: The representative of "root" is the new representative of the set to which "j" is the representative.
  void union_of(element_pointer root, element_pointer j)
  {
    father_[representative(j)->sequence_index()]= father_[representative(root)->sequence_index()] ;
  }

  //:
  vcl_vector<element_pointer> dart_associated_elt_;

  //:
  vbl_controlled_partition representatives_;
};

#include "vmap_kernel.txx"

#endif
