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
    _Base::initialise() ;
    _representatives.initialise(_p.nb_cycles()) ;
}

template <class TPermutation>
bool vmap_permutation_kernel<TPermutation>::valid() const
{
  vcl_vector<int> v(_p.nb_cycles(), false) ;
  for (int i=0; i<size(); i++)
  {
    dart_iterator d=dart(i) ;
    element_index e1=_p.cycle_index_of(d),
      e2=_p.opposite_cycle_index_of(d) ;
    if (v[e1] && v[e2])
      return false ;
    v[e1]=true ;
  }
  return true ;
}

template <class TPermutation>
void vmap_permutation_kernel<TPermutation>::add_1_cycles()
{
  vcl_vector<int> mark(_p.nb_elements(),false),
    markV(_p.nb_cycles(),false) ;

  element_iterator v ;

  for (v=_p.begin_cycle() ; v!=_p.end_cycle(); ++v) // Maximum sized walks of redundant darts.
  {
    if (!markV[_p.index_of(v)])
    {
      dart_iterator d=v->begin(), sd=d ;
      _p.next_on_cycle(sd) ;
      while (d==sd)
      {
        int e1 ;
        _Base::add(d) ;
        mark[_p.index_of(d)]=true ;
        markV[e1=_p.cycle_index_of(d)] =true ;
        _p.opposite(d) ;
        _representatives.union_of(_p.cycle_index_of(d),e1) ;
        mark[_p.index_of(d)]=true ;
        do { _p.next_on_cycle(d) ; } while (mark[_p.index_of(d)]) ;
        sd=d ;
        do { _p.next_on_cycle(sd) ; } while (mark[_p.index_of(sd)]) ;
      }
    }
  }
}

template <class TPermutation>
void vmap_permutation_kernel<TPermutation>::add_2_cycles()
{
  vcl_vector<int> mark(_p.nb_elements(),false),
    markV(_p.nb_cycles(),false) ;

  element_iterator v ;

  for (v=_p.begin_cycle() ; v!=_p.end_cycle(); ++v) // Maximum sized walks of redundant darts.
  {
    if (!markV[_p.index_of(v)])
    {
      dart_iterator d=v->begin(), nnd=d ;
      _p.next_on_cycle(nnd) ;
      _p.next_on_cycle(nnd) ;
      if (d!=nnd)
      {
        nnd=d ;
        do
        {
          if (!mark[_p.index_of(d)])
          {
            dart_iterator a=d ;
            _p.opposite(a) ;
            dart_iterator na = a;
            _p.next_on_cycle(na) ;
            dart_iterator nna = na;
            _p.next_on_cycle(nna) ;
            while (a==nna)
            {
              a=na ;
              int e1 ;
              _Base::add(a) ;
              mark[_p.index_of(a)]=true ;
              markV[e1=_p.cycle_index_of(a)] =true ;
              _p.opposite(a) ;
              _representatives.union_of(_p.cycle_index_of(a),e1) ;
              mark[_p.index_of(a)]=true ;
              na = a;
              _p.next_on_cycle(na) ;
              nna = na;
              _p.next_on_cycle(nna) ;
            }
          }
          _p.next_on_cycle(d) ;
        } while (d!=nnd) ;
      }
    }
  }
  for (v=_p.begin_cycle() ; v!=_p.end_cycle(); ++v)  // Isolated Loops composed of redundant darts.
  {
    if (!markV[_p.index_of(v)])
    {
      dart_iterator d=v->begin(), nnd=d ;
      _p.next_on_cycle(nnd) ;
      _p.next_on_cycle(nnd) ;
      if (d==nnd)
      {
        dart_iterator a=d ;
        _p.opposite(a) ;
        _p.previous_on_cycle(d) ;
        dart_iterator na = a;
        _p.next_on_cycle(na) ;
        dart_iterator nna = na;
        _p.next_on_cycle(nna) ;
        while (a==nna && a!=d)
        {
          a=na ;
          int e1 ;
          _Base::add(a) ;
          mark[_p.index_of(a)]=true ;
          markV[e1=_p.cycle_index_of(a)] =true ;
          _p.opposite(a) ;
          _representatives.union_of(_p.cycle_index_of(a),e1) ;
          mark[_p.index_of(a)]=true ;
          na = a;
          _p.next_on_cycle(na) ;
          nna = na;
          _p.next_on_cycle(nna) ;
        }
      }
    }
  }
}

//:
template <class TPermutation>
bool vmap_permutation_kernel<TPermutation>::can_insert(const dart_iterator & arg) const
{
  return _representatives.representative(_p.cycle_index_of(arg))!=
         _representatives.representative(_p.opposite_cycle_index_of(a)) ;
}

//:
template <class TPermutation>
bool vmap_permutation_kernel<TPermutation>::add(const dart_iterator & arg)
{
  element_index e1=_representatives.representative(_p.cycle_index_of(arg)),
      e2=_representatives.representative(_p.opposite_cycle_index_of(arg)) ;

  if (e1==e2) return false ;
  _representatives.union_of(e2,e1) ;
  _Base::add(arg) ;
  _dart_associated_elt.push_back(_p.get_cycle_pointer(e1)) ;
  return true ;
}

#endif // vmap_kernel_txx_
