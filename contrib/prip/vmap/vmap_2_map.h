// This is prip/vmap/vmap_2_map.h
#ifndef vmap_2_map_h_
#define vmap_2_map_h_
//:
// \file
// \brief Provides a 2-map (combinatorial map) which is the base structure of vmap.
// \author Jocelyn Marchadier
// \date 06 May 2004
//
// \verbatim
//  Modifications
//   06 May 2004 Jocelyn Marchadier
// \endverbatim

#include <iostream>
#include <iosfwd>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "vmap_types.h" // for vmap_dart_index
#include "vmap_ptr_sequences.h"
#include "vmap_kernel.h"

class vmap_2_map_dart ;

//:
template <typename DPtr>
inline DPtr vmap_2_map_alpha(DPtr arg)
{
  return (DPtr) arg->vmap_2_map_dart::alpha() ;
}

//:
template <typename DPtr>
inline DPtr vmap_2_map_sigma(DPtr arg)
{
  return (DPtr) arg->vmap_2_map_dart::sigma() ;
}

//:
template <typename DPtr>
inline DPtr vmap_2_map_phi(DPtr arg)
{
  return (DPtr) arg->vmap_2_map_dart::phi() ;
}

//:
template <typename DPtr>
inline DPtr vmap_2_map_ialpha(DPtr arg)
{
  return (DPtr) arg->vmap_2_map_dart::ialpha() ;
}

//:
template <typename DPtr>
inline DPtr vmap_2_map_isigma(DPtr arg)
{
  return (DPtr) arg->vmap_2_map_dart::isigma() ;
}

//:
template <typename DPtr>
inline DPtr vmap_2_map_iphi(DPtr arg)
{
  return (DPtr) arg->vmap_2_map_dart::iphi() ;
}

//: vmap_2_map_dart implements a dart of a vmap_2_map.
class vmap_2_map_dart
{
 public:
  virtual ~vmap_2_map_dart() ;

  //: Returns true if the dart is a pendant dart.
  bool is_pendant() const
  {
    return this == sigma() ;
  }

  //: Returns true if the dart is a redundant dart.
  bool is_redundant() const
  {
    return this == sigma()->sigma() ;
  }

    //: Returns true if the dart is a self direct loop.
  bool is_self_direct_loop() const
  {
    return this == phi() ;
  }

    //: Returns true if the dart is a double dart.
  bool is_double() const
  {
    return this == phi()->phi() ;
  }

    //: The following functions should not be used by the user
  const vmap_2_map_dart* alpha() const
  {
    return alpha_ ;
#if 0
    return const_cast<vmap_2_map_dart*> (this+(1-2*(index()%2))) ;
#endif // 0
  }

  const vmap_2_map_dart* sigma() const
  {
    return sigma_ ;
  }

  const vmap_2_map_dart* phi() const
  {
    return alpha()->sigma_ ;
  }

  const vmap_2_map_dart* isigma() const
  {
    return isigma_ ;
  }

  const vmap_2_map_dart* iphi() const
  {
    return isigma_->alpha() ;
  }

  vmap_2_map_dart* alpha()
  {
    return alpha_ ;
#if 0
    return const_cast<vmap_2_map_dart*> (this+(1-2*(index()%2))) ;
#endif // 0
  }

  vmap_2_map_dart* sigma()
  {
    return sigma_ ;
  }

  vmap_2_map_dart* phi()
  {
    return alpha()->sigma_ ;
  }

  vmap_2_map_dart* isigma()
  {
    return isigma_ ;
  }

  vmap_2_map_dart* iphi()
  {
    return isigma_->alpha() ;
  }

  //: For construction of the underlying map.
  void set_sigma(vmap_2_map_dart *arg) ;
  void set_alpha(vmap_2_map_dart *arg) ;
  void set_phi(vmap_2_map_dart *arg) ;

  //: Index of the dart in the sequence.
  // Very powerful as it enables the direct order reuse in implicit pyramids,
  // and an efficient dynamic management of darts in the basic CombinatorialMap.
  vmap_dart_index sequence_index() const
  {
    return sequence_index_ ;
  }
  void set_sequence_index(vmap_dart_index arg)
  {
    sequence_index_=arg ;
  }

 protected :
  vmap_dart_index sequence_index_ ;
  vmap_2_map_dart *sigma_ ;
  vmap_2_map_dart *isigma_ ;
  vmap_2_map_dart *alpha_ ;
};

//: The base dart iterator class.
// This class is instantiated in vmap_2_map. There is no need to use it directly.
template< typename Ref, typename Ptr, typename It >
class vmap_2_map_dart_base_iterator
{
 public:
  typedef vmap_2_map_dart_base_iterator< Ref,Ptr,It > self_type ;

  vmap_2_map_dart_base_iterator() = default;

  explicit vmap_2_map_dart_base_iterator(It arg) :it_(arg) {}

#if 0 // This constructor is covered by the next one
  vmap_2_map_dart_base_iterator(self_type const& it) :it_((It)it.reference()) {}
#endif // 0

  template< typename Ref2, typename Ptr2, typename It2 >
  vmap_2_map_dart_base_iterator(vmap_2_map_dart_base_iterator<Ref2,Ptr2,It2> const& it)
  {
    it_=(It)it.reference();
  }

  ~vmap_2_map_dart_base_iterator() = default;

#if 0 // This assignment operator is covered by the next one
  self_type & operator=(self_type const& it)
  {
    it_=it.it_ ;
    return *this ;
  }
#endif // 0

  template< typename Ref2, typename Ptr2, typename It2 >
  self_type & operator=(vmap_2_map_dart_base_iterator<Ref2,Ptr2,It2> const& it)
  {
    it_=(It)it.reference() ;
    return *this ;
  }

  self_type & operator=(It const& it)
  {
    it_=it ;
    return *this ;
  }

  bool operator==(self_type const& it) const
  {
    return it_==it.it_ ;
  }

  bool operator!=(self_type const& it) const
  {
    return it_!=it.it_ ;
  }

  Ref operator*() const
  {
    return (Ref)**it_ ;
  }

  Ptr operator->() const
  {
    return (Ptr)*it_;
  }

  //: Applies alpha.
  self_type & alpha ()
  {
    it_+=offset(vmap_2_map_alpha(*it_)) ;
#if 0 // old implementation
    it_=(Ptr)it_->alpha() ;
#endif // 0
    return *this ;
  }

  //: Applies phi.
  self_type & phi ()
  {
    it_+=offset(vmap_2_map_phi(*it_)) ;
#if 0 // old implementation
    it_=(Ptr)it_->phi() ;
#endif // 0
    return *this ;
  }

  //: Applies sigma.
  self_type & sigma ()
  {
    it_+=offset(vmap_2_map_sigma(*it_)) ;
#if 0 // old implementation
    it_=(Ptr)it_->sigma() ;
#endif // 0
    return *this ;
  }

  //: Applies alpha^-1.
  self_type & ialpha ()
  {
    alpha() ;
    return *this ;
  }

  //: Applies phi^{-1}.
  self_type & iphi ()
  {
    it_+=offset(vmap_2_map_iphi(*it_)) ;
#if 0 // old implementation
    it_=(Ptr)it_->iphi() ;
#endif // 0
    return *this ;
  }

  //: Applies sigma^{-1}.
  self_type & isigma ()
  {
    it_+=offset(vmap_2_map_isigma(*it_)) ;
#if 0 // old implementation
    it_=(Ptr)it_->isigma() ;
#endif // 0
    return *this ;
  }

  //: Next dart of the sequence.
  self_type & operator++()
  {
    it_++ ;
    return *this ;
  }

  //: ith dart following on the sequence.
  self_type operator+(int i)
  {
    it_+=i ;
    return *this ;
  }

  It reference() const
  {
    return it_ ;
  }

 protected:
  int offset(vmap_2_map_dart const* arg) const
  {
    return arg->sequence_index()-(*it_)->sequence_index() ;
  }
  It it_ ;
};

//: The class for handling a 2-map (combinatorial map).
template <class D=vmap_2_map_dart>
class vmap_2_map : public vmap_owning_sequence<D>
{
 public:
  typedef vmap_2_map<D> self_type;
  static vmap_2_map_tag tag ;

  typedef vmap_owning_sequence<D> dart_sequence ;
  typedef typename dart_sequence::iterator dart_sequence_iterator;
  typedef typename dart_sequence::const_iterator const_dart_sequence_iterator;

  typedef typename dart_sequence::pointer dart_pointer ;

  //: dart_type.
  typedef D dart_type ;
  //: reference on a dart.
  typedef dart_type& dart_reference ;
  //: const reference on a dart.
  typedef dart_type const& const_dart_reference ;

  //: casts a dart into the user dart type.
  static dart_reference cast(vmap_2_map_dart & a)
  {
    return (dart_reference) a ;
  }

  //: casts a dart into the user dart type.
  static const_dart_reference cast(vmap_2_map_dart const& a)
  {
    return (const_dart_reference) a ;
  }

  //: An iterator iterates on a sequence of darts and on the topology.
  typedef vmap_2_map_dart_base_iterator< dart_reference,dart_type*,dart_sequence_iterator> dart_iterator ;

  //: An iterator iterates in read-only mode on a sequence of darts and on the topology.
  typedef vmap_2_map_dart_base_iterator< const_dart_reference,dart_type const*,const_dart_sequence_iterator> const_dart_iterator ;

 public:

  vmap_2_map() = default;

  vmap_2_map(self_type const& m) ;

  virtual ~vmap_2_map();

  self_type & operator=(self_type const& m);

  //: Sets the structure of the map identical to the structure of "m".
  // (Inlined the implementation here, to avoid compiler error with MSVC)
  template <class M>
  void set_structure(M const& m)
  {
    if ((self_type const*)&m!=this)
    {
      initialise_darts(m.nb_darts()) ;
      for (vmap_dart_index i=0; i<nb_darts(); ++i)
      {
        set_sigma(i,m.sigma(i));
        set_alpha(i,m.alpha(i));
      }
    }
  }

  //: Returns the number of darts.
  int nb_darts () const
  {
    return dart_sequence::size() ;
  }

  //: Returns "true" if the map is empty.
  bool empty() const
  {
    return dart_sequence::empty() ;
  }

  //: Returns the index of the dart "arg".
  vmap_dart_index index (vmap_2_map_dart const& arg) const
  {
#if 0
    vmap_dart_index tmp=index(*arg.edge_) ;
    return &arg==&arg.edge_->dart_[0]?tmp:alpha(tmp) ;
    return &arg-&dartArray_[0] ;
#endif // 0
    return arg.sequence_index();
  }

  //: Returns the index of a dart in the initial sequence
  int position(vmap_2_map_dart const& arg) const
  {
    return dart_sequence::position(arg) ;
  }

  //: Returns the index of a dart in the initial sequence
  int dart_position(vmap_dart_index i) const
  {
    return dart_sequence::position(dart(i)) ;
  }

  //: Returns an iterator on the first dart.
  const_dart_iterator begin_dart() const
  {
    return const_dart_iterator(begin_dart_sequence()) ;
  }

  //: Returns an iterator on the first dart.
  dart_iterator begin_dart()
  {
    return dart_iterator(begin_dart_sequence()) ;
  }

  //: Returns an iterator after the last dart of the sequence.
  const_dart_iterator end_dart() const
  {
    return const_dart_iterator(end_dart_sequence()) ;
  }

  //: Returns an iterator after the last dart of the sequence.
  dart_iterator end_dart()
  {
    return dart_iterator(end_dart_sequence()) ;
  }

  //: Returns the dart of index "arg".
  const_dart_reference dart(vmap_dart_index arg) const
  {
    return *get_dart_pointer(arg) ;
  }

  //: Returns the dart of index "arg".
  dart_reference dart(vmap_dart_index arg)
  {
    return *get_dart_pointer(arg) ;
  }

  //: Transforms the map into its dual.
  virtual void setDualStructure() ;

  //: Kernel class for contraction.
  typedef vmap_kernel<self_type> contraction_kernel ;

  //: Contract all the dart of the kernel "arg_kernel".
  void contraction(contraction_kernel const& arg_kernel) ;

  //: Kernel class for contraction.
  typedef vmap_kernel<self_type> removal_kernel ;

  //: Remove all the dart of the kernel "arg_kernel".
  void removal(removal_kernel const& arg_kernel);

  //: Returns sigma("arg"), the next dart turning clockwise around the beginning vertex of the dart.
  vmap_dart_index sigma (vmap_dart_index arg) const
  {
    return vmap_2_map_sigma(get_dart_pointer(arg))->sequence_index() ;
  }

  //: Returns alpha("arg"), the opposite dart of the same edge.
  vmap_dart_index alpha (vmap_dart_index arg) const
  {
    return vmap_2_map_alpha(get_dart_pointer(arg))->sequence_index()  ;
  }

  //: Returns phi("arg"), the next dart turning counter-clockwise around the left face of the dart.
  vmap_dart_index phi(vmap_dart_index arg) const
  {
    return vmap_2_map_phi(get_dart_pointer(arg))->sequence_index() ;
  }

#if 0
  //: must be called before initialisation
  virtual void begin_initialise() ;

  //: must be called after ending the initialisation
  virtual void end_initialise() ;
#endif // 0

  //: Initialise "arg" darts.
  void initialise_darts(int arg) ;

  //: Sets sigma("arg")="arg_sigma", the next dart turning clockwise around the first vertex.
  void set_sigma (vmap_dart_index arg, vmap_dart_index arg_sigma) ;

  //: Sets alpha("arg")="arg_alpha", the next dart turning around the supporting edge.
  void set_alpha (vmap_dart_index arg, vmap_dart_index arg_alpha) ;

  //: Sets phi("arg")="arg_phi", the next dart turning counter-clockwise around the left face. Use only when alpha is set.
  void set_phi (vmap_dart_index arg, vmap_dart_index arg_phi) ;

  //: Returns true if all the permutations are valid, false otherwise.
  //  May be useful for testing permutations set by hand...
  virtual bool valid() const ;

  //: Deletes everything.
  virtual void clear() ;

  //: Initializes the structure of the combinatorial map from "stream".
  void read_structure(std::istream & stream) ;

  //: Writes the structure of the combinatorial map to "stream".
  void write_structure(std::ostream & stream) const ;

 protected:

  //: Contracts dart "arg".
  void unchecked_contraction(dart_iterator & arg) ;

  //: Removes dart "arg".
  void unchecked_removal(dart_iterator & arg) ;

  void suppress_from_sequence(dart_type * d) ;

 protected:

  dart_sequence_iterator begin_dart_sequence()
  {
    return dart_sequence::begin() ;
  }

  dart_sequence_iterator end_dart_sequence()
  {
    return dart_sequence::end() ;
  }

  const_dart_sequence_iterator begin_dart_sequence() const
  {
    return dart_sequence::begin() ;
  }

  const_dart_sequence_iterator end_dart_sequence() const
  {
    return dart_sequence::end() ;
  }

  dart_pointer & get_dart_pointer(int arg)
  {
    return dart_sequence::get_pointer(arg) ;
  }

  dart_pointer const& get_dart_pointer(int arg) const
  {
    return dart_sequence::get_pointer(arg) ;
  }
};

#include "vmap_2_map.hxx"

#endif
