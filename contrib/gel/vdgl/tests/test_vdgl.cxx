//:
// \file

#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator_cubic.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_fit_line.h>
#include <vsol/vsol_line_2d.h>
#include <vcl_iostream.h>

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

  vcl_cerr<<"Length: "<< myline->length();

  //--------------------------------------------------
  //: test the generation of a straight edgel_chain defined by two points
  //
  double x0=0, y0=0, x1=10, y1=10;
  vdgl_edgel_chain_sptr ec = new vdgl_edgel_chain(x0, y0, x1, y1);
  int N = ec->size();
  vcl_cout << "Chain has " << N << " edgels\n";
  for (int i = 0; i<N; i++)
    vcl_cout << "edgel[" << i<<"] = (" << (*ec)[i] << ")\n";

  return 0;
}
