// This is prip/vpyr/vpyr_2_pyramid_level_elts.h
#ifndef vpyr_2_pyramid_level_elts_h_
#define vpyr_2_pyramid_level_elts_h_
//:
// \file
// \brief provides the different elements of a level of a combinatorial pyramid.
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  Modifications
//   6 May 2004 Jocelyn Marchadier
// \endverbatim

#include <vmap/vmap_types.h> // for vmap_dart_index
#include "vpyr_2_pyramid_base.h"

//: The dart class of a pyramid.
class vpyr_2_pyramid_level_dart
{
 public:
  typedef vpyr_2_pyramid_base_dart base_type ;

  //:
  vpyr_2_pyramid_level_dart() {}

  //:
  void set(base_type* arg_link, vmap_level_index arg_level)
  {
    link_=arg_link;
    level_=arg_level;
  }

  //:
  vpyr_2_pyramid_level_dart(const vpyr_2_pyramid_level_dart & arg)
    :link_(arg.link_),level_(arg.level_)                           {}

  //: Returns true if the dart is a pendant dart.
  bool is_pendant() const
  {
    return link_->is_pendant(level()) ;
  }

  //: Returns true if the dart is a redundant dart.
  bool is_redundant() const
  {
    return link_->is_redundant(level()) ;
  }

  //: Returns true if the dart is a self direct loop.
  bool is_self_direct_loop() const
  {
    return link_->is_self_direct_loop(level()) ;
  }

  //: Returns true if the dart is a double dart.
  bool is_double() const
  {
    return link_->is_double(level()) ;
  }

  //: The dart's level.
  vmap_level_index level() const
  {
    return level_;
  }

  //: An index of the map's vertex sequence.
  vmap_dart_index sequence_index() const
  {
    return link_->sequence_index() ;
  }

 protected :
  //:
  base_type* link() const {return (base_type*)link_ ;}

  //:
  base_type* link_ ;

  //:
  vmap_level_index level_ ;
};

//:
template <class D>
class vmap_2_pdl_dart : public vpyr_2_pyramid_level_dart
{
 public:
  //:
  typedef D value_set ;

  //:
  typedef typename value_set::value_type value_type ;

  //:
  typedef vmap_2_pd_dart< value_set > base_type ;

  //:
  vmap_2_pdl_dart() {}

  //:
  vmap_2_pdl_dart(vmap_2_pdl_dart<D> const& arg) :vpyr_2_pyramid_level_dart(arg) {}

 protected :
  //:
  base_type* link() const {return (base_type*)vpyr_2_pyramid_level_dart::link() ;}

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
};

//:
template< typename FD, typename Ref, typename Ptr, typename It >
class vpyr_2_pyramid_level_dart_base_iterator : protected FD
{
 public:
  //:
  typedef vpyr_2_pyramid_level_dart_base_iterator< FD,Ref,Ptr,It > self_type ;

  //:
  typedef FD element_type ;

  //:
  vpyr_2_pyramid_level_dart_base_iterator() {}

#ifndef VCL_VC // already covered by the next, more general, copy constructor
  //:
  vpyr_2_pyramid_level_dart_base_iterator(const self_type &it)
    :element_type(it),it_(it.it_)                              {}
#endif

  //:
  template< typename FDp, typename Refp, typename Ptrp, typename Itp >
  vpyr_2_pyramid_level_dart_base_iterator(const vpyr_2_pyramid_level_dart_base_iterator<FDp,Refp,Ptrp,Itp> &it)
    :it_((It)it.reference())
  {
    set(*it_, it.level()) ;
  }

  //:
  template< typename FDp, typename Refp, typename Ptrp, typename Itp >
  self_type & operator=(const vpyr_2_pyramid_level_dart_base_iterator<FDp,Refp,Ptrp,Itp> &it)
  {
    it_=(It)it.reference() ;
    set(*it_, it.level()) ;
    return *this ;
  }

  //:
  ~vpyr_2_pyramid_level_dart_base_iterator() {}

  //:
  self_type & operator=(const self_type &it)
  {
    this->level_=it.level() ;
    it_=it.reference() ;
    set_link();
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

  //:  Applies alpha.
  self_type & alpha()
  {
    it_+=offset(vpyr_2_pyramid_base_alpha(this->link(),level())) ;
    set_link();
    return *this ;
  }

  //:  Applies phi.
  self_type & phi ()
  {
    it_+=offset(vpyr_2_pyramid_base_phi(this->link(),level())) ;
    set_link();
    return *this ;
  }

  //:  Applies sigma.
  self_type & sigma ()
  {
    it_+=offset(vpyr_2_pyramid_base_sigma(this->link(),level())) ;
    set_link();
    return *this ;
  }

  //:  Applies alpha^-1.
  self_type & ialpha ()
  {
    alpha() ;
    return *this ;
  }

  //:  Applies phi^{-1}.
  self_type & iphi ()
  {
    it_+=offset(vpyr_2_pyramid_base_iphi(this->link(),level())) ;
    set_link();
    return *this ;
  }

  //:  Applies sigma^{-1}.
  self_type & isigma ()
  {
    it_+=offset(vpyr_2_pyramid_base_isigma(this->link(),level())) ;
    set_link();
    return *this ;
  }

  //:  Next dart.
  self_type & operator++ ()
  {
    ++it_;
    set_link();
    return *this ;
  }

  //:  Next dart.
  self_type & operator+ (int i)
  {
    it_+=i;
    set_link();
    return *this ;
  }

  //:  The dart's level.
  vmap_level_index level() const
  {
    return FD::level();
  }

// private :

  //:
  vpyr_2_pyramid_level_dart_base_iterator(It arg,vmap_level_index l)
    :it_(arg)
  {
    set(*arg,l) ;
  }

  //:
  It reference() const
  {
    return it_ ;
  }

 protected:

  //:
  void set_link()
  {
    this->link_=*it_ ;
  }

  //:
  template <typename D>
  int offset(D* arg) const
  {
    return arg->sequence_index()-this->link_->sequence_index();
  }

 private:

  //:
  It it_ ;
};

#endif
