// This is prip/vpyr/vpyr_2_pyramid.txx
#ifndef vpyr_2_pyramid_txx_
#define vpyr_2_pyramid_txx_
//:
// \file

#include "vpyr_2_pyramid.h"

template <class TLevel>
vpyr_2_pyramid<TLevel>::vpyr_2_pyramid()
{
}

#if 0
template <class TLevel>
vpyr_2_pyramid<TLevel>::vpyr_2_pyramid(const self_type &right)
{
  operator=(right) ;
}
#endif // 0

template <class TLevel>
vpyr_2_pyramid<TLevel>::~vpyr_2_pyramid()
{
  clear() ;
}

#if 0
template <class TLevel>
vpyr_2_pyramid<TLevel> & vpyr_2_pyramid<TLevel>::operator=(const self_type &right)
{
  if (this!=&right)
  {
    clear() ;
    _base_map.set_structure(right) ;
    initialise_levels(right.nb_levels());
    for (int i=0; i<nb_levels(); ++i)
    {
      level(i).set_nb_darts(right.level(i).nb_darts()) ;
    }
  }
  return *this ;
}

template <class TLevel>
void vpyr_2_pyramid<TLevel>::initialise_levels(int nb_levels)
{
  clear() ;
  for (unsigned int i=0; i+1<_level.size(); ++i)
  {
    level_type tmp(i,(typename level_type::pyramid_type&)*this) ;
    _level.insert(level_array_type::value_type(i,tmp)) ;
  }
  (--_level.end())->second.set_level_index(vmap_top_level_index()) ;
}
#endif // 0

template <class TLevel>
TLevel * vpyr_2_pyramid<TLevel>::level_below(vmap_level_type type, const level_type& above)
{
  level_iterator it=_level.find(above.index()) ;
  vmap_level_index level=0;
  if (it!=_level.begin())
  {
    --it ;
    level=(*it).index() + 1 ; // remove +1 for previous behavior
  }
  while (vmap_get_level_type(level)!=type)
  {
    ++level ;
  }
  if (level>=above.index()) return NULL ;
  if (level!=(*it).index())
  {
    level_type tmp(above) ;
    tmp.set_level_index(level) ;
    it=_level.insert(level_array_type::value_type(level,tmp)).first ;
  }
  return &(*it) ;
}

template <class TLevel>
template <class M>
void vpyr_2_pyramid<TLevel>::set_base_structure(const M & arg)
{
  _base_map.set_structure(arg) ;
  _level.clear() ;
  _level.insert(level_array_type::value_type(::vmap_top_level_index(),
                level_type(::vmap_top_level_index(),(typename level_type::pyramid_type&) *this))) ;
}

template <class TLevel>
bool vpyr_2_pyramid<TLevel>::valid()
{
  return false;
}

template <class TLevel>
void vpyr_2_pyramid<TLevel>::clear()
{
  _level.clear() ;
  _base_map.clear() ;
}

//: Initializes the sturcture of the combinatorial map from "stream".
template <class TLevel>
void vpyr_2_pyramid<TLevel>::read_structure(vcl_istream & stream)
{
}

//: Writes the sturcture of the combinatorial map to "stream".
template <class TLevel>
void vpyr_2_pyramid<TLevel>::write_structure(vcl_ostream & stream) const
{
}

#endif // vpyr_2_pyramid_txx_
