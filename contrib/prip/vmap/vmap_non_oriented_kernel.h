// This is prip/vmap/vmap_non_oriented_kernel.h
#ifndef vmap_non_oriented_kernel_h_
#define vmap_non_oriented_kernel_h_
//:
// \file
// \brief provides a wrapper to an existing kernel which manages darts in a non oriented manner.
//        Also avoids having loops in the resulting kernel.
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  06 May 2004 Jocelyn Marchadier
// \endverbatim

#include "vmap_kernel.h"

//: A wrapper to an existing kernel which manage darts in an non oriented mananer and avoids aving loops in the resulting kernel.
// The darts added to the kernel using the function "add" are added if the resulting edge graph is a tree.
// When finalise is called, the non oriented graph is turn into an oriented graph which is composed of rooted trees.
template <class TKernel>
class vmap_non_oriented_kernel : public TKernel
{
 public:

  //:
  typedef TKernel Base_ ;

  //:
  typedef typename Base_::map_type map_type ;

  //:
  typedef typename Base_::dart_iterator dart_iterator ;
#if 0
  typedef typename map_type::vmap_dart_index vmap_dart_index ;
#endif // 0

  //: the linked elements
  typedef typename Base_::element_type element_type ;

  //:
  typedef typename Base_::element_iterator element_iterator ;

  //:
  typedef typename Base_::element_index element_index ;

  //:
  typedef typename Base_::element_pointer element_pointer ;

  //:
  vmap_non_oriented_kernel(map_type & arg) : Base_(arg) {}

  //:
  ~vmap_non_oriented_kernel() {}

  //: Before any addion/test
  void initialise() ;

  //: After addions
  void finalise() ;

  //: Inserts the edge corresponding to "arg". Returns false when the edge cannot be added.
  bool add(const dart_iterator & arg) ;

  //:
  bool add(vmap_dart_index arg)
  {
    return add(dart_iterator_on(arg)) ;
  }

  bool add(typename map_type::edge_type & arg)
  {
    return add(arg.begin()) ;
  }

  //:
  void clear()
  {
    Base_::clear() ;
    graph_.clear() ;
  }

 protected :

  //:
  void add_from(element_index elt, vcl_vector<int> & visited) ;

 private :

  //:
  typedef vcl_vector<dart_iterator> AdjList;

  //:
  typedef vcl_vector<AdjList> Graph;

  //:
  Graph graph_ ;
};

#include "vmap_non_oriented_kernel.txx"

#endif
