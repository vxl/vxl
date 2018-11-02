// This is prip/vmap/vmap_oriented_kernel.h
#ifndef vmap_oriented_kernel_h_
#define vmap_oriented_kernel_h_
//:
// \file
// \brief provides a wrapper for constructing oriented kernels.
// \author Jocelyn Marchadier
// \date 06 May 2004
//
// \verbatim
//  Modifications
//   06 May 2004 Jocelyn Marchadier
// \endverbatim

#include <iostream>
#include <vector>
#include "vmap_types.h" // for vmap_dart_index
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Wrapper to avoid having non rooted hierarchies while constructing a kernel.
template <class TKernel>
class vmap_oriented_kernel : public TKernel
{
 public:

  //:
  typedef TKernel Base_ ;

  //:
  typedef typename Base_::map_type map_type ;

  //:
  typedef typename Base_::dart_iterator dart_iterator ;
#if 0 // vmap_dart_index is typedef'd in vmap_types.h
  typedef typename map_type::vmap_dart_index vmap_dart_index ;
#endif // 0

  //:
  vmap_oriented_kernel(map_type & arg) : Base_(arg) {}

  //:
  ~vmap_oriented_kernel() = default;

  //: Before any addition/test
  void initialise() ;

  //: After additions
  void finalise() {}

  //:
  bool can_insert(const dart_iterator & arg) const
  {
    return !mark_(this->permutation().index_of(arg)) || !Base_::can_insert(arg) ;
  }

  //:
  bool add(const dart_iterator & arg) ;

  //:
  bool add(vmap_dart_index arg)
  {
    return add(this->dart_iterator_on(arg)) ;
  }

  //:
  void clear()
  {
    Base_::clear() ;
    mark_.clear() ;
  }

 private :

  //:
  typedef std::vector<int> Mark ;

  //:
  Mark mark_ ;
};

#include "vmap_oriented_kernel.hxx"

#endif
