//
// .NAME test_generalized_eigensystem
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 29 Aug 96
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_test.h>
#include <vnl/algo/vnl_generalized_eigensystem.h>

extern "C"
void test_generalized_eigensystem()
{
  double Sdata[36] = {
   30.0000,   -3.4273,   13.9254,   13.7049,   -2.4446,   20.2380,
   -3.4273,   13.7049,   -2.4446,    1.3659,    3.6702,   -0.2282,
   13.9254,   -2.4446,   20.2380,    3.6702,   -0.2282,   28.6779,
   13.7049,    1.3659,    3.6702,   12.5273,   -1.6045,    3.9419,
   -2.4446,    3.6702,   -0.2282,   -1.6045,    3.9419,    2.5821,
   20.2380,   -0.2282,   28.6779,    3.9419,    2.5821,   44.0636,
  };
  double Cdata[36] = {
    0,  0,  0,  0,  0,  0,  
    0,  0,  0,  0,  0,  0,  
    0,  0,  0,  0,  0,  0,  
    0,  0,  0,  0,  0,  2,  
    0,  0,  0,  0, -1,  0,  
    0,  0,  0,  2,  0,  0,  
  };

  vnl_matrix<double> S(Sdata, 6,6);
  vnl_matrix<double> C(Cdata, 6,6);
  
  vnl_generalized_eigensystem gev(C, S);

  cout << "V = " << gev.V << endl;
  cout << "D = " << gev.D << endl;
  cout << "residual = " << C * gev.V - S * gev.V * gev.D << endl;
  double err = (C * gev.V - S * gev.V * gev.D).inf_norm();
  cout << "Recomposition residual = " << err << endl;

  Assert("Recomposition residual < 1e-12", err < 1e-12);
}

TESTMAIN(test_generalized_eigensystem);
