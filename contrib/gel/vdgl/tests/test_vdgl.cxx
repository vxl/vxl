//#include <vcl_iostream.h>

#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator_cubic.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_fit_line.h>
#include <vsol/vsol_line_2d.h>

int main()
{
  // Create an edgel chain...
  vdgl_edgel_chain e;

  // ...with some edgels (x,y,gamma,theta)
  vdgl_edgel e1( 1,2,3,4);
  vdgl_edgel e2( 2,6,7,8);
  vdgl_edgel e3( 3,4,3,2);
  vdgl_edgel e4( 4,5,3,4);

  e.add_edgel( e1);
  e.add_edgel( e2);
  e.add_edgel( e3);
  e.add_edgel( e4);


  vsol_line_2d_sptr myline;
  myline=vdgl_fit_line(e);

  //Here check for the length of the line

  cerr<<"Length: "<< myline->length();

  return 0;
}
