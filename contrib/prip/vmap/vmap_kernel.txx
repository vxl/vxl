// This is prip/vmap/vmap_kernel.txx
#ifndef vmap_kernel_txx_
#define vmap_kernel_txx_
//:
// \file
#include "vmap_kernel.h"

template <class TPermutation>
void vmap_permutation_kernel<TPermutation>::initialise()
{
  clear() ;
  Base_::initialise() ;
  representatives_.initialise(p_.nb_cycles()) ;
}

template <class TPermutation>
bool vmap_permutation_kernel<TPermutation>::valid() const
{
  vcl_vector<bool> v(p_.nb_cycles(), false) ;
  for (unsigned int i=0; i<this->size(); ++i)
  {
    dart_iterator d=this->dart(i) ;
    element_index e1=p_.cycle_index_of(d),
                  e2=p_.opposite_cycle_index_of(d) ;
    if (v[e1] && v[e2])
    {
      return false ;
    }
    v[e1]=true ;
  }
  return true ;
}

template <class TPermutation>
void vmap_permutation_kernel<TPermutation>::add_1_cycles()
{
  vcl_vector<bool> mark(p_.nb_elements(),false),
                   markV(p_.nb_cycles(),false) ;

  element_iterator v ;

  for (v=p_.begin_cycle() ; v!=p_.end_cycle(); ++v) // Maximum sized walks of redundant darts.
  {
    if (!markV[p_.index_of(v)])
    {
      dart_iterator d=v->begin(), sd=d ;
      p_.next_on_cycle(sd) ;
      while (d==sd)
      {
        int e1 ;
        Base_::add(d) ;
        mark[p_.index_of(d)]=true ;
        markV[e1=p_.cycle_index_of(d)] =true ;
        p_.opposite(d) ;
        representatives_.union_of(p_.cycle_index_of(d),e1) ;
        mark[p_.index_of(d)]=true ;
        do { p_.next_on_cycle(d) ; } while (mark[p_.index_of(d)]) ;
        sd=d ;
        do { p_.next_on_cycle(sd) ; } while (mark[p_.index_of(sd)]) ;
      }
    }
  }
}

template <class TPermutation>
void vmap_permutation_kernel<TPermutation>::add_2_cycles()
{
  vcl_vector<bool> mark(p_.nb_elements(),false),
                   markV(p_.nb_cycles(),false) ;

  element_iterator v ;

  for (v=p_.begin_cycle() ; v!=p_.end_cycle(); ++v) // Maximum sized walks of redundant darts.
  {
    if (!markV[p_.index_of(v)])
    {
      dart_iterator d=v->begin(), nnd=d ;
      p_.next_on_cycle(nnd) ;
      p_.next_on_cycle(nnd) ;
      if (d!=nnd)
      {
        nnd=d ;
        do
        {
          if (!mark[p_.index_of(d)])
          {
            dart_iterator a=d ;
            p_.opposite(a) ;
            dart_iterator na = a;
            p_.next_on_cycle(na) ;
            dart_iterator nna = na;
            p_.next_on_cycle(nna) ;
            while (a==nna)
            {
              a=na ;
              int e1 ;
              Base_::add(a) ;
              mark[p_.index_of(a)]=true ;
              markV[e1=p_.cycle_index_of(a)] =true ;
              p_.opposite(a) ;
              representatives_.union_of(p_.cycle_index_of(a),e1) ;
              mark[p_.index_of(a)]=true ;
              na = a;
              p_.next_on_cycle(na) ;
              nna = na;
              p_.next_on_cycle(nna) ;
            }
          }
          p_.next_on_cycle(d) ;
        } while (d!=nnd) ;
      }
    }
  }
  for (v=p_.begin_cycle() ; v!=p_.end_cycle(); ++v)  // Isolated Loops composed of redundant darts.
  {
    if (!markV[p_.index_of(v)])
    {
      dart_iterator d=v->begin(), nnd=d ;
      p_.next_on_cycle(nnd) ;
      p_.next_on_cycle(nnd) ;
      if (d==nnd)
      {
        dart_iterator a=d ;
        p_.opposite(a) ;
        p_.previous_on_cycle(d) ;
        dart_iterator na = a;
        p_.next_on_cycle(na) ;
        dart_iterator nna = na;
        p_.next_on_cycle(nna) ;
        while (a==nna && a!=d)
        {
          a=na ;
          int e1 ;
          Base_::add(a) ;
          mark[p_.index_of(a)]=true ;
          markV[e1=p_.cycle_index_of(a)] =true ;
          p_.opposite(a) ;
          representatives_.union_of(p_.cycle_index_of(a),e1) ;
          mark[p_.index_of(a)]=true ;
          na = a;
          p_.next_on_cycle(na) ;
          nna = na;
          p_.next_on_cycle(nna) ;
        }
      }
    }
  }
}

//:
template <class TPermutation>
bool vmap_permutation_kernel<TPermutation>::can_insert(const dart_iterator & arg) const
{
  return representatives_.representative(p_.cycle_index_of(arg)) !=
         representatives_.representative(p_.opposite_cycle_index_of(arg)) ;
}

//:
template <class TPermutation>
bool vmap_permutation_kernel<TPermutation>::add(const dart_iterator & arg)
{
  element_index e1=representatives_.representative(p_.cycle_index_of(arg)),
                e2=representatives_.representative(p_.opposite_cycle_index_of(arg)) ;

  if (e1==e2) return false ;
  representatives_.union_of(e2,e1) ;
  Base_::add(arg) ;
  dart_associated_elt_.push_back(p_.get_cycle_pointer(e1)) ;
  return true ;
}

#endif // vmap_kernel_txx_
