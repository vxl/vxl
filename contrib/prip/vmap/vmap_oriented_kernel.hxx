// This is prip/vmap/vmap_oriented_kernel.hxx
#ifndef vmap_oriented_kernel_hxx_
#define vmap_oriented_kernel_hxx_

#include "vmap_oriented_kernel.h"

template <class TKernel>
void vmap_oriented_kernel<TKernel>::initialise()
{
  int elts = this->permutation().nb_elements() ;
  clear() ;
  Base_::initialise() ;
  mark_.reserve(elts) ;
  for (int i=0;i<elts; ++i)
  {
    mark_.push_back(false) ;
  }
}

template <class TKernel>
bool vmap_oriented_kernel<TKernel>::add(const dart_iterator & arg)
{
  if (mark_[this->permutation().index_of(arg)] || !Base_::add(arg))
  {
    return false ;
  }

  dart_iterator tmp=arg ;
  do
  {
    mark_[this->permutation().index_of(tmp)]=true ;
    this->permutation().next_on_cycle(tmp) ;
  }
  while (tmp!=arg) ;
  tmp=arg; this->permutation().opposite(tmp) ;
  mark_[this->permutation().index_of(tmp)]=true ;

  return true ;
}

#endif // vmap_oriented_kernel_hxx_
