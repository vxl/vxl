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

#include <vmap/vmap_types.h>
#include "vpyr_2_pyramid_base.h"


//: The dart class of a pyramid.
class vpyr_2_pyramid_level_dart
{
 public:
  typedef vpyr_2_pyramid_base_dart base_type ;

  //:
  vpyr_2_pyramid_level_dart()
  {}

  //:
  void set(base_type* arg_link, vmap_level_index arg_level)
  {
    _link=arg_link;
    _level=arg_level;
  }

  //:
  vpyr_2_pyramid_level_dart(const vpyr_2_pyramid_level_dart & arg)
      :_link(arg._link),_level(arg._level)
  {}

  //: Returns true if the dart is a pendant dart.
  bool is_pendant() const
  {
    return _link->is_pendant(level()) ;
  }

  //: Returns true if the dart is a redundant dart.
  bool is_redundant() const
  {
    return _link->is_redundant(level()) ;
  }

  //: Returns true if the dart is a self direct loop.
  bool is_self_direct_loop() const
  {
    return _link->is_self_direct_loop(level()) ;
  }

  //: Returns true if the dart is a double dart.
  bool is_double() const
  {
    return _link->is_double(level()) ;
  }

  //: The dart's level.
  vmap_level_index level() const
  {
    return _level;
  }

  //: An index of the map's vertex sequence.
  vmap_dart_index sequence_index() const
  {
    return _link->sequence_index() ;
  }

 protected :
  //:
  base_type* link() const {return (base_type*)_link ;}

  //:
  base_type* _link ;

  //:
  vmap_level_index _level ;
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
  vmap_2_pdl_dart()
  {}

  //:
  vmap_2_pdl_dart(const vmap_2_pdl_dart<D> & arg)
      :vpyr_2_pyramid_level_dart(arg)
  {}


 protected :
  //:
  base_type* link() const {return (base_type*)vpyr_2_pyramid_level_dart::link() ;}

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
  vpyr_2_pyramid_level_dart_base_iterator()
  {}

  //:
  vpyr_2_pyramid_level_dart_base_iterator(const self_type &right)
      :element_type(right),_it(right._it)
  {}

  //:
  template< typename FDp, typename Refp, typename Ptrp, typename Itp >
  vpyr_2_pyramid_level_dart_base_iterator(const vpyr_2_pyramid_level_dart_base_iterator<FDp,Refp,Ptrp,Itp> &right)
      :_it((It)right.reference())
  {
    set(*_it, right.level()) ;
  }

  //:
  template< typename FDp, typename Refp, typename Ptrp, typename Itp >
  self_type & operator=(const vpyr_2_pyramid_level_dart_base_iterator<FDp,Refp,Ptrp,Itp> &right)
  {
    _it=(It)right.reference() ;
    set(*_it, right.level()) ;
    return *this ;
  }

  //:
  ~vpyr_2_pyramid_level_dart_base_iterator()
  {}

  //:
  self_type & operator=(const self_type &right)
  {
    _level=right.level() ;
    _it=right.reference() ;
    set_link();
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
  const Ptr operator->() const
  {
    return (const Ptr)this;
  }

  //:  Applies alpha.
  self_type & alpha()
  {
    _it+=offset(vpyr_2_pyramid_base_alpha(link(),level())) ;
    set_link();
    return *this ;
  }

  //:  Applies phi.
  self_type & phi ()
  {
    _it+=offset(vpyr_2_pyramid_base_phi(link(),level())) ;
    set_link();
    return *this ;
  }

  //:  Applies sigma.
  self_type & sigma ()
  {
    _it+=offset(vpyr_2_pyramid_base_sigma(link(),level())) ;
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
    _it+=offset(vpyr_2_pyramid_base_iphi(link(),level())) ;
    set_link();
    return *this ;
  }

  //:  Applies sigma^{-1}.
  self_type & isigma ()
  {
    _it+=offset(vpyr_2_pyramid_base_isigma(link(),level())) ;
    set_link();
    return *this ;
  }

  //:  Next dart.
  self_type & operator++ ()
  {
    ++_it;
    set_link();
    return *this ;
  }

  //:  Next dart.
  self_type & operator+ (int i)
  {
    _it+=i;
    set_link();
    return *this ;
  }

  //:  The dart's level.
  vmap_level_index level() const
  {
    return FD::level();
  }

  //////////////////////private :
  //:
  vpyr_2_pyramid_level_dart_base_iterator(It arg,vmap_level_index l)
      :_it(arg)
  {
    set(*arg,l) ;
  }

  //:
  It reference() const
  {
    return _it ;
  }

 protected:

  //:
  void set_link()
  {
    _link=*_it ;
  }

  //:
  template <typename D>
  int offset(D* arg) const
  {
    return arg->sequence_index()-_link->sequence_index();
  }

 private:

  //:
  It _it ;
};

#endif
