// This is prip/vmap/vmap_oriented_kernel.h
#ifndef vmap_oriented_kernel_h_
#define vmap_oriented_kernel_h_
//:
// \file
// \brief provides a wrapper for constructing oriented kernels.
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  06 May 2004 Jocelyn Marchadier
// \endverbatim

#include "vmap_kernel.h"

//: Wrapper to avoid having non rooted hierarchies while constructing a kernel.
template <class vmap_kernel>
class vmap_oriented_kernel : public vmap_kernel
{
 public:

  //:
  typedef vmap_kernel Base_ ;

  //:
  typedef typename Base_::map_type map_type ;

  //:
  typedef typename Base_::dart_iterator dart_iterator ;
#if 0
  typedef typename map_type::vmap_dart_index vmap_dart_index ;
#endif // 0

  //:
  vmap_oriented_kernel(map_type & arg) : Base_(arg) {}

  //:
  ~vmap_oriented_kernel() {}

  //: Before any addion/test
  void initialise() ;

  //: After addions
  void finalise() {}

  //:
  bool can_insert(const dart_iterator & arg) const
  {
    return !mark_(permutation().index_of(arg)) || !Base_::can_insert(arg) ;
  }

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
    mark_.clear() ;
  }

 private :

  //:
  typedef std::vector<int> Mark ;

  //:
  Mark mark_ ;
};

#include "vmap_oriented_kernel.txx"

#endif
