#include <iostream>
#include <ostream>
#include "rsdl_bounding_box.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

rsdl_bounding_box::rsdl_bounding_box( const rsdl_point& min_point,
                                      const rsdl_point& max_point )
  : min_point_(min_point), max_point_(max_point)
{
  unsigned int Nc = min_point_.num_cartesian();
  assert( max_point_.num_cartesian() == Nc );
  assert( max_point_.num_angular() == min_point_.num_angular() );

  for ( unsigned int i=0; i<Nc; ++i )
    if ( min_point_.cartesian(i) > max_point_.cartesian(i) ) {
      double temp = min_point_.cartesian(i);
      min_point_.cartesian(i) = max_point_.cartesian(i);
      max_point_.cartesian(i) = temp;
    }
}


rsdl_bounding_box::rsdl_bounding_box( const rsdl_bounding_box& old ) = default;


rsdl_bounding_box &
rsdl_bounding_box::operator= ( const rsdl_bounding_box& old ) = default;


std::ostream& operator<< ( std::ostream& ostr, const rsdl_bounding_box& box )
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
