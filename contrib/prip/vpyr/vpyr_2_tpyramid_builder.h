// This is prip/vpyr/vpyr_2_tpyramid_builder.h
#ifndef vpyr_bu_builder_h_
#define vpyr_bu_builder_h_
//:
// \file
// \brief defines a class building pyramids.
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  06 May 2004 Jocelyn Marchadier
// \endverbatim

#include "vpyr_2_tpyramid.h"

//: class for handling arbitrary pyramids.
template < class TPyramid , class TTopLevel >
class vpyr_bu_builder
{
  //typedef Basevpyr_2_pyramid _Base ;
 public:

  //:
  typedef TPyramid pyramid_type ;

  //:
  typedef TTopLevel top_level_type ;

  //:
  typedef typename top_level_type::removal_kernel removal_kernel_type ;

  //:
  typedef typename top_level_type::contraction_kernel contraction_kernel_type ;

  //:
  vpyr_bu_builder();

  //:
  virtual ~vpyr_bu_builder();

  //:
  template <class M >
  void set_base_structure(const M & arg) ;

  //:
  virtual void contraction_top_level(const contraction_kernel_type & arg) ;

  //:
  virtual void removal_top_level(const removal_kernel_type & arg) ;

#if 0
    // build removal kernel
    vmap_non_oriented_kernel<removal_kernel_type> rk(top_level()) ;
    kernel.initialise() ;
    for (top_level_type::edge_iterator i=top_level().begin_edge(); i!=top_level().end_edge(); ++i)
    {
      // if can remove
      rk.add(*i) ;
    }
    rk.finalise() ;
    removal_top_level(rk) ;

    // build_contraction_kernel
    contraction_kernel_type ck
#endif // 0

 protected :

  top_level_type & top_level()
  {
    return top_level_ ;
  }

  pyramid_type & pyramid()
  {
    return pyramid_ ;
  }

 private :

  //:
  pyramid_type pyramid_ ;

  //:
  top_level_type top_level_ ;
};

#endif
