#include <vcl_iostream.h>
#include <bugl/bugl_gaussian_point_2d.h>
#include <vnl/vnl_double_2x2.h>

int main()
{

  vnl_double_2x2 s;
  bugl_gaussian_point_2d<double> pt(1, 1, s);
  
  return 0;
}
