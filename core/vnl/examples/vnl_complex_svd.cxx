//-*- c++ -*-------------------------------------------------------------------
// Module: complex-svd
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 09 May 97
// Converted to vxl by Peter Vanroose, February 2000
//-----------------------------------------------------------------------------

#include <vcl/vcl_iostream.h>
#include <vnl/vnl_complex.h>
#include <vnl/vnl_matlab_print.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_svd.h>

int main()
{
  double r[] = {
    2,     7,     5,
    1,     0,     9,
    8,     5,     7,
    4,     0,     7
  };

  double i[] = {
    0,     4,     9,
    1,     4,     7,
    8,     7,     1,
    6,     5,     4
  };

  vcl_double_complex cmplx[12];
  for (int k=0; k<12; ++k) cmplx[k] = vcl_double_complex(r[k],i[k]);

  vnl_matrix<vcl_double_complex> C(cmplx, 4, 3);
  
  cout << "C = " << C << endl;

  vnl_svd<vcl_double_complex> C_svd(C);

  vnl_matlab_print(cout, C_svd.U(), "U");
  vnl_matlab_print(cout, C_svd.W().asMatrix(), "W");
  vnl_matlab_print(cout, C_svd.V(), "V");

  vnl_double_complex rhs[4]; rhs[0]=3; rhs[1]=9; rhs[2]=-2; rhs[3]=-8;
  vnl_vector<vcl_double_complex> b(rhs, 4);

  // From "C x = b" find x:
  cout << "x = " << C_svd.solve(b) << endl;

  return 0;
}
