// This is contrib/prip/vmap/vmap_oriented_kernel.h
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

#include <vector>
#include "vmap_kernel.h"

//: Wrapper to avoid having non rooted hierarchies while constructing a kernel.
template <class vmap_kernel>
class vmap_oriented_kernel : public vmap_kernel
{
public:

  //:
  typedef vmap_kernel _Base ;
  
  //:
  typedef typename _Base::map_type map_type ;
 
  //:
  typedef typename _Base::dart_iterator dart_iterator ;
  //typedef typename map_type::vmap_dart_index vmap_dart_index ;

  //:
  vmap_oriented_kernel(map_type & arg)
      : _Base(arg)
  {}
  
  //:
  ~vmap_oriented_kernel()
  {}

  //: Before any addion/test
  void initialise() ;

  //: After addions
  void finalise()
  {}

  //:
  bool can_insert(const dart_iterator & arg) const
  {
    return !_mark(permutation().index_of(arg)) || !_Base::can_insert(arg) ;
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
    _Base::clear() ;
    _mark.clear() ;
  }

private :

  //:
  typedef std::vector<int> Mark ;
  
  //:
  Mark _mark ;
};


#include "vmap_oriented_kernel.txx"

#endif
