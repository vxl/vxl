// This is core/vgl/vgl_area.cxx
#include "vgl_area.h"
#include <vgl/vgl_polygon.h>

#include <vcl_cmath.h> // for vcl_fabs

float
vgl_area_signed( const vgl_polygon& poly )
{
  // Compute the area using Green's theorem
  //
  float area = 0.0f;
  for ( int s = 0; s < poly.num_sheets(); ++s ) {
    unsigned int j = poly[s].size()-1;
    for ( unsigned int i = 0; i < poly[s].size(); ++i ) {
      area += poly[s][j].x() * poly[s][i].y() - poly[s][i].x() * poly[s][j].y();
      j = i;
    }
  }

  return area/2;
}

// This function is not implemented inline because the cost of a
// single function call is irrelevant compared to the cost of running
// vgl_area_signed. It is therefore better to have fewer dependencies
// in the header file and implement this function here.
float
vgl_area( const vgl_polygon& poly )
{
  return (float)vcl_fabs( vgl_area_signed( poly ) );
}
