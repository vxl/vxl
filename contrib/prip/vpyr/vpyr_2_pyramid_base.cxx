// This is prip/vpyr/vpyr_2_pyramid_base.cxx
#include "vpyr_2_pyramid_base.h"

#if 0
vpyr_2_pyramid_base_dart::vpyr_2_pyramid_base_dart()
{
  directAncestor_=toplevel_typeAncestor_=this ;
}
#endif // 0

vpyr_2_pyramid_base_dart* vpyr_2_pyramid_base_dart::ancestor(vmap_level_index level)
{
#if 0
  if (level==::vmap_top_level_index())
  {
    return toplevel_typeAncestor() ;
  }
  vpyr_2_pyramid_base_dart* d=this;
  unsigned dl=d->last_level() ;
  while (dl<level)
  {
    d=d->directAncestor() ;
  }
#endif // 0
  vpyr_2_pyramid_base_dart* d=this;
  unsigned dl=d->last_level() ;
  while (dl<level)
  {
    if (vmap_is_contraction_type(dl))
    {
      d=vmap_2_map_phi(d) ;
    }
    else
    {
      d=vmap_2_map_sigma(d) ;
    }
    dl=d->last_level() ;
  }
  return d ;
}

#if 0 // method commented out
vpyr_2_pyramid_base_dart* vpyr_2_pyramid_base_dart::directAncestor()
{
  if (directAncestor_==this && last_level()!=::vmap_top_level_index())
  {
    unsigned dl=last_level() ;
    do
    {
      if (vmap_is_contraction_type(directAncestor_->last_level()))
      {
        directAncestor_=::phi(directAncestor_) ;
      }
      else
      {
        directAncestor_=::sigma(directAncestor_) ;
      }
    }
    while (directAncestor_->last_level()<=dl) ;
  }
  return directAncestor_ ;
}

vpyr_2_pyramid_base_dart* vpyr_2_pyramid_base_dart::toplevel_typeAncestor()
{
    if (toplevel_typeAncestor_->last_level()!=::vmap_top_level_index())
    {
      vpyr_2_pyramid_base_dart * d=toplevel_typeAncestor_->directAncestor() ;
      while (toplevel_typeAncestor_->last_level()!=::vmap_top_level_index())
      {
        toplevel_typeAncestor_=toplevel_typeAncestor_->directAncestor() ;
      }
      while (d->toplevel_typeAncestor_->last_level()!=::vmap_top_level_index())
      {
        d->toplevel_typeAncestor_ = toplevel_typeAncestor_ ;
        d=d->directAncestor() ;
      }
    }
    return toplevel_typeAncestor_ ;
}
#endif // 0

void vpyr_2_pyramid_base_dart::set_last_level(vmap_level_index arg)
{
  last_level_=(arg*2);
  if (arg==vmap_top_level_index())
  {
    vmap_2_map_alpha(this)->last_level_=last_level_;
  }
  else
  {
#ifdef DEBUG
    vcl_cout<<arg<<'/'<<last_level_<<vcl_endl ;
#endif
    vmap_2_map_alpha(this)->last_level_=(last_level_+1);
  }
#if 0
  directAncestor_=toplevel_typeAncestor_=this ;
#endif // 0
}

const vpyr_2_pyramid_base_dart* vpyr_2_pyramid_base_dart::ancestor(vmap_level_index level) const
{
  vpyr_2_pyramid_base_dart& d=const_cast<vpyr_2_pyramid_base_dart&> (*this);
  return d.ancestor(level) ;
}
