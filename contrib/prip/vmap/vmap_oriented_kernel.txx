// This is prip/vmap/vmap_oriented_kernel.txx
#ifndef vmap_oriented_kernel_txx_
#define vmap_oriented_kernel_txx_

#include "vmap_oriented_kernel.h"

template <class vmap_kernel>
void vmap_oriented_kernel<vmap_kernel>::initialise()
{
  int elts = permutation().nb_elements() ;
  clear() ;
  Base_::initialise() ;
  mark_.reserve(elts) ;
  for (int i=0;i<elts; ++i)
  {
    mark_.push_back(false) ;
  }
}

template <class vmap_kernel>
bool vmap_oriented_kernel<vmap_kernel>::add(const dart_iterator & arg)
{
  if (mark_[permutation().index_of(arg)] || !Base_::add(arg))
  {
    return false ;
  }

  dart_iterator tmp=arg ;
  do
  {
    mark_[permutation().index_of(tmp)]=true ;
    permutation().next_on_cycle(tmp) ;
  }
  while (tmp!=arg) ;
  tmp=arg; permutation().opposite(tmp) ;
  mark_[permutation().index_of(tmp)]=true ;

  return true ;
}

#endif // vmap_oriented_kernel_txx_
