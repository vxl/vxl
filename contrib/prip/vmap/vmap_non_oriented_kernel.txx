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
   _Base::initialise() ;
   _graph.resize(permutation().nb_cycles()) ;
}

template <class vmap_kernel>
void
vmap_non_oriented_kernel<vmap_kernel>::finalise()
{
  int nbel=permutation().nb_cycles() ;
  _representatives.initialise(nbel) ;
  vcl_vector<int> visited (nbel,false) ;
  for (int i=0; i<nbel;i++)
  {
    if (!_graph[i].empty() && _representatives.representative(i)==i)
    {
      add_from(i, visited) ;
    }
  }
}

template <class vmap_kernel>
bool vmap_non_oriented_kernel<vmap_kernel>::add(const dart_iterator & arg)
{
   element_index e1=permutation().cycle_index_of(arg), 
     e2=permutation().opposite_cycle_index_of(arg) ;

   if (_representatives.representative(e1)==_representatives.representative(e2))
      return false ;

  _representatives.union_of(e2,e1) ;
  _graph[e2].push_back(arg) ;
  dart_iterator a=arg ; a.alpha() ;
  _graph[e1].push_back(a) ;

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
    typename AdjList::iterator it=_graph[act].begin() ;
    for (;it!=_graph[act].end();++it)
    {
      int n=permutation().cycle_index_of(*it) ;
      if (!visited[n])
      {
        visited[n]=true ;
        _Base::add(*it) ;
        fifo.push_back(n) ;
      }
    }
  }
}

#endif // vmap_non_oriented_kernel_txx_
