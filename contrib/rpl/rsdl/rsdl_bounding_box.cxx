#include "rsdl_bounding_box.h"

#include <vcl_cassert.h>
#include <vcl_ostream.h>

rsdl_bounding_box::rsdl_bounding_box( const rsdl_point& min_point,
                                      const rsdl_point& max_point )
  : min_point_(min_point), max_point_(max_point)
{
  unsigned int Nc = min_point_.num_cartesian();
  unsigned int Na = min_point_.num_angular();

  assert( max_point_.num_cartesian() == Nc );
  assert( max_point_.num_angular() == Na );

  for ( unsigned int i=0; i<Nc; ++i )
    if ( min_point_.cartesian(i) > max_point_.cartesian(i) ) {
      double temp = min_point_.cartesian(i);
      min_point_.cartesian(i) = max_point_.cartesian(i);
      max_point_.cartesian(i) = temp;
    }
}


rsdl_bounding_box::rsdl_bounding_box( const rsdl_bounding_box& old )
  : min_point_(old.min_point_), max_point_(old.max_point_)
{
}


rsdl_bounding_box &
rsdl_bounding_box::operator= ( const rsdl_bounding_box& old )
{
  min_point_ = old.min_point_;
  max_point_ = old.max_point_;
  return *this;
}


vcl_ostream& operator<< ( vcl_ostream& ostr, const rsdl_bounding_box& box )
{
  unsigned int Nc = box.num_cartesian();
  unsigned int Na = box.num_angular();

  unsigned int i;
  for ( i=0; i<Nc; ++i )
    ostr << "Cartesian  " << i << ": [" << box.min_cartesian(i) << "," << box.max_cartesian(i) << "]\n";
  for ( i=0; i<Na; ++i )
    ostr << "Angular  " << i << ": [" << box.min_angular(i) << "," << box.max_angular(i) << "]\n";
  return ostr;
}


