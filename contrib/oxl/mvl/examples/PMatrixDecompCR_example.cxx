//:
// \file
// \brief PMatrixDecompCR example
// \author Peter Vanroose, November 2002

#include <vcl_iostream.h>
#include <vnl/vnl_double_3x4.h>
#include <mvl/PMatrix.h>
#include <mvl/PMatrixDecompCR.h>

int main()
{
  vnl_double_3x4 m;
  m(0,0)= -568.051; m(1,0)=  83.1082; m(2,0)=  0.274578;
  m(0,1)= -9.18145; m(1,1)= -689.130; m(2,1)=  0.0458282;
  m(0,2)= -552.506; m(1,2)= -194.806; m(2,2)= -0.960472;
  m(0,3)=  596.353; m(1,3)= -92.4159; m(2,3)= -0.228363;
  PMatrix P(m);

  {
    PMatrixDecompCR decomp(P);
    vcl_cout << "Decomposition:\n"
             << "C =\n" << decomp.C << "Rt =\n" << decomp.Po;
  }
  {
    PMatrixDecompCR decomp(P, false);
    vcl_cout << "Decomposition without scaling:\n"
             << "C =\n" << decomp.C << "Rt =\n" << decomp.Po;
  }
  return 0;
}
