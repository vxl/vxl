#ifdef __GNUC__
#pragma implementation
#endif

#include "vdgl_digital_region.h"

vdgl_digital_region::vdgl_digital_region()
{
}

vdgl_digital_region::~vdgl_digital_region()
{
}

bool vdgl_digital_region::add_point( const vgl_point_2d<double> &p)
{
  ps_.push_back( p);

  return true;
}

vcl_ostream& operator<<(vcl_ostream& s, const vdgl_digital_region& d)
{
  s << "<vdgl_digital_region (";
  for( unsigned int i=0; i< d.ps_.size(); i++)
    {
      s << "( " << d.ps_[i].x() << ", " << d.ps_[i].y() << ")";
      if( i!= (d.ps_.size()-1))
        s << ", ";
    }

  return s << ")>";
}
