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

#if 0 // method commented out
template <class TLevel>
vpyr_2_pyramid<TLevel>::vpyr_2_pyramid(const self_type &p)
{
  operator=(p) ;
}
#endif // 0

template <class TLevel>
vpyr_2_pyramid<TLevel>::~vpyr_2_pyramid()
{
  clear() ;
}

#if 0 // method commented out
template <class TLevel>
vpyr_2_pyramid<TLevel> & vpyr_2_pyramid<TLevel>::operator=(const self_type &p)
{
  if (this!=&p)
  {
    clear() ;
    base_map_.set_structure(p) ;
    initialise_levels(p.nb_levels());
    for (int i=0; i<nb_levels(); ++i)
    {
      level(i).set_nb_darts(p.level(i).nb_darts()) ;
    }
  }
  return *this ;
}
#endif // 0

#if 0 // method commented out
template <class TLevel>
void vpyr_2_pyramid<TLevel>::initialise_levels(int nb_levels)
{
  clear() ;
  for (unsigned int i=0; i+1<level_.size(); ++i)
  {
    level_type tmp(i,(typename level_type::pyramid_type&)*this) ;
    level_.insert(typename level_array_type::value_type(i,tmp)) ;
  }
  (--level_.end())->second.set_level_index(vmap_top_level_index()) ;
}
#endif // 0

template <class TLevel>
TLevel * vpyr_2_pyramid<TLevel>::level_below(vmap_level_type type, const level_type& above)
{
  level_iterator it=level_.find(above.index()) ;
  vmap_level_index level=0;
  if (it!=level_.begin())
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
    it=level_.insert(typename level_array_type::value_type(level,tmp)).first ;
  }
  return &(*it) ;
}

template <class TLevel>
bool vpyr_2_pyramid<TLevel>::valid()
{
  return false;
}

template <class TLevel>
void vpyr_2_pyramid<TLevel>::clear()
{
  level_.clear() ;
  base_map_.clear() ;
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
