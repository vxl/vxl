// This is contrib/prip/vpyr/vpyr_2_pyramid_base.cxx

#include "vpyr_2_pyramid_base.h"
/*vpyr_2_pyramid_base_dart::vpyr_2_pyramid_base_dart()
{
		_directAncestor=_toplevel_typeAncestor=this ;
}*/

vpyr_2_pyramid_base_dart* vpyr_2_pyramid_base_dart::ancestor(vmap_level_index level)
{
  /*    if (level==::vmap_top_level_index())
  		{
  			return toplevel_typeAncestor() ;
  		}
  		vpyr_2_pyramid_base_dart* d=this;
      unsigned dl=d->last_level() ;
      while (dl<level)
      {
  			d=d->directAncestor() ;
      }*/
  vpyr_2_pyramid_base_dart* d=this;
  unsigned dl=d->last_level() ;
  while (dl<level)
  {
    if (vmap_is_contraction_type(dl))
      d=vmap_2_map_phi(d) ;
    else
      d=vmap_2_map_sigma(d) ;
    dl=d->last_level() ;
  }
  return d ;
}
/*
vpyr_2_pyramid_base_dart* vpyr_2_pyramid_base_dart::directAncestor()
{
		if (_directAncestor==this && last_level()!=::vmap_top_level_index()) 
		{
			unsigned dl=last_level() ;
			do
			{
				if (vmap_is_contraction_type(_directAncestor->last_level()))
							_directAncestor=::phi(_directAncestor) ;
					else
							_directAncestor=::sigma(_directAncestor) ;
			}
			while (_directAncestor->last_level()<=dl) ;
		}
    return _directAncestor ;
}
 
vpyr_2_pyramid_base_dart* vpyr_2_pyramid_base_dart::toplevel_typeAncestor()
{
		if (_toplevel_typeAncestor->last_level()!=::vmap_top_level_index())
		{
			vpyr_2_pyramid_base_dart * d=_toplevel_typeAncestor->directAncestor() ;
			while (_toplevel_typeAncestor->last_level()!=::vmap_top_level_index()) 
				_toplevel_typeAncestor=_toplevel_typeAncestor->directAncestor() ;
			while (d->_toplevel_typeAncestor->last_level()!=::vmap_top_level_index()) 
			{
				d->_toplevel_typeAncestor = _toplevel_typeAncestor ;
				d=d->directAncestor() ;
			}
		}
		return _toplevel_typeAncestor ;
}
*/

void vpyr_2_pyramid_base_dart::set_last_level(vmap_level_index arg)
{
  _last_level=(arg*2);
  if (arg==vmap_top_level_index())
  {
    vmap_2_map_alpha(this)->_last_level=_last_level;
  }
  else
  {
    //std::cout<<arg<<'/'<<_last_level<<std::endl ;
    vmap_2_map_alpha(this)->_last_level=(_last_level+1);
  }
  //_directAncestor=_toplevel_typeAncestor=this ;

}

const vpyr_2_pyramid_base_dart* vpyr_2_pyramid_base_dart::ancestor(vmap_level_index level) const
{
  vpyr_2_pyramid_base_dart& d=const_cast<vpyr_2_pyramid_base_dart&> (*this);
  return d.ancestor(level) ;
}



