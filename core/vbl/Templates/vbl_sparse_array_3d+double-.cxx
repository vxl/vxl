#include <vbl/vbl_sparse_array_3d.txx>
VBL_SPARSE_ARRAY_3D_INSTANTIATE(double);

int main()
{
  vbl_sparse_array_3d<double> x;

  x(1,2,3) = 1.23;
  x(100,200,3) = 100.2003;

  vcl_cout << "123 = " << x(1,2,3) << vcl_endl;
  vcl_cout << "222 = " << x(2,2,2) << vcl_endl;

  vcl_cout << "333 is full? " << x.fullp(3,3,3) << vcl_endl;

  vcl_cout << x;
  return 1;
}
