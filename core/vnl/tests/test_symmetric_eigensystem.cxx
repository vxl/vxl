#ifdef __GNUC__
#pragma implementation "test_symmetric_eigensystem.h"
#endif
//
// Class: test_symmetric_eigensystem
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 29 Aug 96
// Modifications:
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_test.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>

extern "C"
void test_symmetric_eigensystem()
{
  double Sdata[36] = {
   30.0000,   -3.4273,   13.9254,   13.7049,   -2.4446,   20.2380,
   -3.4273,   13.7049,   -2.4446,    1.3659,    3.6702,   -0.2282,
   13.9254,   -2.4446,   20.2380,    3.6702,   -0.2282,   28.6779,
   13.7049,    1.3659,    3.6702,   12.5273,   -1.6045,    3.9419,
   -2.4446,    3.6702,   -0.2282,   -1.6045,    3.9419,    2.5821,
   20.2380,   -0.2282,   28.6779,    3.9419,    2.5821,   44.0636,
  };
  vnl_matrix<double> S(Sdata, 6,6);

  {
    vnl_symmetric_eigensystem<double> eig(S);
    vnl_matrix<double> res = eig.recompose() - S;
    cout << "V'*D*V - S = " << res << endl;
    cout << "residual = " << res.inf_norm() << endl;
    Assert("recompose residual",  res.inf_norm() < 1e-12);
  }

  double Cdata[36] = {
    0,  0,  0,  0,  0,  0,  
    0,  0,  0,  0,  0,  0,  
    0,  0,  0,  0,  0,  0,  
    0,  0,  0,  0,  0,  2,  
    0,  0,  0,  0, -1,  0,  
    0,  0,  0,  2,  0,  0,  
  };

  vnl_matrix<double> C(Cdata, 6,6);
  
  {
    vnl_symmetric_eigensystem<double> eig(C);
    vnl_matrix<double> res = eig.recompose() - C;
    cout << "V'*D*V - C = " << res << endl;
    cout << "residual = " << res.inf_norm() << endl;
    Assert("recompose residual", res.inf_norm() < 1e-12);
  }

}

TESTMAIN(test_symmetric_eigensystem);
