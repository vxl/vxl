// This is prip/vmap/vmap_non_oriented_kernel.txx
#ifndef vmap_non_oriented_kernel_txx_
#define vmap_non_oriented_kernel_txx_

#include "vmap_non_oriented_kernel.h"
#include <vcl_list.h>

template <class vmap_kernel>
void
vmap_non_oriented_kernel<vmap_kernel>::initialise()
{
  clear() ;
  Base_::initialise() ;
  graph_.resize(this->permutation().nb_cycles()) ;
}

template <class vmap_kernel>
void
vmap_non_oriented_kernel<vmap_kernel>::finalise()
{
  int nbel=this->permutation().nb_cycles() ;
  this->representatives_.initialise(nbel) ;
  vcl_vector<int> visited (nbel,false) ;
  for (int i=0; i<nbel;i++)
  {
    if (!graph_[i].empty() && this->representatives_.representative(i)==i)
    {
      add_from(i, visited) ;
    }
  }
}

template <class vmap_kernel>
bool vmap_non_oriented_kernel<vmap_kernel>::add(const dart_iterator & arg)
{
  element_index e1=this->permutation().cycle_index_of(arg),
                e2=this->permutation().opposite_cycle_index_of(arg) ;

  if (this->representatives_.representative(e1)==this->representatives_.representative(e2))
  {
    return false ;
  }

  this->representatives_.union_of(e2,e1) ;
  graph_[e2].push_back(arg) ;
  dart_iterator a=arg ; a.alpha() ;
  graph_[e1].push_back(a) ;

  return true ;
}

template <class vmap_kernel>
void vmap_non_oriented_kernel<vmap_kernel>::add_from(element_index elt, vcl_vector<int> & visited)
{
  vcl_list<int> fifo ;
  fifo.push_back(elt) ;
  visited[elt]=true ;
  while (!fifo.empty()) // breadth first strategy
  {
    int act=fifo.front() ;
    fifo.pop_front() ;
    typename AdjList::iterator it=graph_[act].begin() ;
    for (;it!=graph_[act].end();++it)
    {
      int n=this->permutation().cycle_index_of(*it) ;
      if (!visited[n])
      {
        visited[n]=true ;
        Base_::add(*it) ;
        fifo.push_back(n) ;
      }
    }
  }
}

#endif // vmap_non_oriented_kernel_txx_
