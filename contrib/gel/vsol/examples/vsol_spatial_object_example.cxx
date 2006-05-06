// Example use of some vsol_spatial_object_2d classes
//
// Author: Peter Vanroose, March 2006

#include <vsol/vsol_conic_2d.h>
#include <vsol/vsol_conic_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vcl_ostream.h>

int main()
{
  // Construct an ellipse with centre (0,0), horizontal main axis of length 20,
  // vertical axis of length 5, i.e., equation $(x/20)^2+(y/5)^2=1$:
  vsol_conic_2d_sptr ellipse = new vsol_conic_2d(vsol_point_2d(0,0), 20, 5, 0);

  // Distance to ellipse:
  vcl_cout << "The closest distance from (10,10) to the ellipse is " << ellipse->distance(new vsol_point_2d(10,10)) << '\n';
  return 0;
}
