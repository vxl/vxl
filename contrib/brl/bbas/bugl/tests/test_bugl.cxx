#include <vcl_iostream.h>
#include <bugl/bugl_gaussian_point_2d.h>
#include <bugl/bugl_gaussian_point_3d.h>
#include <vnl/vnl_double_2x2.h>
#include <vnl/vnl_double_3x3.h>

int main()
{

  vnl_double_2x2 s2;
  bugl_gaussian_point_2d<double> p2d(1, 1, s2);

  vnl_double_3x3 s3;
  bugl_gaussian_point_3d<double> p3d(100, 100, 100, s3);  
  return 0;
}
