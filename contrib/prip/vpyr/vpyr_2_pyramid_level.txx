// This is prip/vpyr/vpyr_2_pyramid_level.txx
#ifndef vpyr_2_pyramid_level_txx_
#define vpyr_2_pyramid_level_txx_

#include "vpyr_2_pyramid_level.h"

template <class D>
vmap_2_map_tag vpyr_2_pyramid_level<D>::tag ;

template <class D>
vpyr_2_pyramid_level<D>::vpyr_2_pyramid_level(vmap_level_index arg_level, pyramid_type & pyramid)
  : dart_sequence(pyramid.base_map()),
    level_index_(arg_level),
    pyramid_(&pyramid)
{
  dart_sequence_iterator d=begin_dart_sequence() ;
  for (;d!=end_dart_sequence() ;++d)
  {
    (*d)->set_last_level(vmap_top_level_index()) ;
  }
}

template <class D>
void vpyr_2_pyramid_level<D>::contraction(const contraction_kernel &arg_kernel)
{
  self_type * l=pyramid().level_below(vmap_contraction_type,*this) ;
  if (l!=NULL)
  {
    int level=l->index() ;
    int ld=nb_darts(), id ;

    typename contraction_kernel::const_iterator itk ;
    for (itk=arg_kernel.begin(); itk!=arg_kernel.end(); ++itk)
    {
      dart_pointer d =*itk ;

      d->set_last_level(level) ;
      id=d->sequence_index() ;
      --ld ;
      dart_sequence::swap(id,ld) ;

      id=vmap_2_map_alpha(d)->sequence_index() ;
      --ld ;
      dart_sequence::swap(id,ld) ;
    }
    dart_sequence::resize(ld) ;
  }
}

template <class D>
void vpyr_2_pyramid_level<D>::removal(const removal_kernel &arg_kernel)
{
  self_type * l=pyramid().level_below(vmap_removal_type,*this) ;
  if (l!=NULL)
  {
    int level=l->index() ;
    int ld=nb_darts(), id ;

    typename contraction_kernel::const_iterator itk ;
    for (itk=arg_kernel.begin(); itk!=arg_kernel.end(); ++itk)
    {
      dart_pointer d =*itk ;

      d->set_last_level(level) ;
      id=d->sequence_index() ;
      --ld ;
      dart_sequence::swap(id,ld) ;

      id=vmap_2_map_alpha(d)->sequence_index() ;
      --ld ;
      dart_sequence::swap(id,ld) ;
    }
    dart_sequence::resize(ld) ;
  }
}

#endif // vpyr_2_pyramid_level_txx_
