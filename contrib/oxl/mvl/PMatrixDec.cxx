#ifdef __GNUG__
#pragma implementation
#endif

//:
//  \file

#include "PMatrixDec.h"

#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h>

//--------------------------------------------------------------
//: Constructor

PMatrixDec::PMatrixDec(const vnl_matrix<double>& p_matrix)
  : PMatrix(p_matrix), _j_matrix(3,3), _d_matrix(4,4)
{
  if (!((p_matrix.rows() == 3) && (p_matrix.columns() == 4)))
    vcl_cout << "PMatrixDec WARNING: Incorrect size of matrix" << vcl_endl;
  Init();
}


//--------------------------------------------------------------
//: Destructor

PMatrixDec::~PMatrixDec() {}


//--------------------------------------------------------------
//: Auxiliar Constructor

void PMatrixDec::Init()
{
    double u0 = p_matrix_(0,0)*p_matrix_(2,0)
              + p_matrix_(0,1)*p_matrix_(2,1)
              + p_matrix_(0,2)*p_matrix_(2,2);
    double v0 = p_matrix_(1,0)*p_matrix_(2,0)
              + p_matrix_(1,1)*p_matrix_(2,1)
              + p_matrix_(1,2)*p_matrix_(2,2);

    double a_u = p_matrix_(0,0)*p_matrix_(0,0)
               + p_matrix_(0,1)*p_matrix_(0,1)
               + p_matrix_(0,2)*p_matrix_(0,2);
    double a_v = p_matrix_(1,0)*p_matrix_(1,0)
               + p_matrix_(1,1)*p_matrix_(1,1)
               + p_matrix_(1,2)*p_matrix_(1,2);
    a_u = a_u-u0*u0;
    a_v = a_v-v0*v0;
    if ((a_u <= 0.0) || (a_v <= 0.0))
      vcl_cout << "PMatrixDec WARNING: Incorrect projection matrix" << vcl_endl;
    else
    {
      a_u = vcl_sqrt(a_u);
      a_v = vcl_sqrt(a_v);

      // INTRINSIC parameters => J
      _j_matrix(0,0) = a_u;
      _j_matrix(0,1) = 0.0;
      _j_matrix(0,2) = u0;

      _j_matrix(1,0) = 0.0;
      _j_matrix(1,1) = a_v;
      _j_matrix(1,2) = v0;

      _j_matrix(2,0) = 0.0;
      _j_matrix(2,1) = 0.0;
      _j_matrix(2,2) = 1.0; // last diagonal element = 1

      // EXTRINSIC parameters => D
      _d_matrix(0,0) = (p_matrix_(0,0) - u0*p_matrix_(2,0))/a_u;
      _d_matrix(0,1) = (p_matrix_(0,1) - u0*p_matrix_(2,1))/a_u;
      _d_matrix(0,2) = (p_matrix_(0,2) - u0*p_matrix_(2,2))/a_u;
      _d_matrix(0,3) = (p_matrix_(0,3) - u0*p_matrix_(2,3))/a_u;

      _d_matrix(1,0) = (p_matrix_(1,0) - v0*p_matrix_(2,0))/a_v;
      _d_matrix(1,1) = (p_matrix_(1,1) - v0*p_matrix_(2,1))/a_v;
      _d_matrix(1,2) = (p_matrix_(1,2) - v0*p_matrix_(2,2))/a_v;
      _d_matrix(1,3) = (p_matrix_(1,3) - v0*p_matrix_(2,3))/a_v;

      _d_matrix(2,0) = p_matrix_(2,0);
      _d_matrix(2,1) = p_matrix_(2,1);
      _d_matrix(2,2) = p_matrix_(2,2);
      _d_matrix(2,3) = p_matrix_(2,3);

      _d_matrix(3,0) = 0.0;
      _d_matrix(3,1) = 0.0;
      _d_matrix(3,2) = 0.0;
      _d_matrix(3,3) = 1.0; // last diagonal element = 1
    }
}


//---------------------------------------------------------------
//: Print to vcl_ostream
vcl_ostream& operator<<(vcl_ostream& s, const PMatrixDec& P)
{
  s << "PROJECTION MATRIX = [" << vcl_endl << P.get_matrix() << "]" << vcl_endl
    << "DECOMPOSITION: " << vcl_endl
    << "Intrinsic Parameters = [" << vcl_endl << P._j_matrix << "]" << vcl_endl
    << "Extrinsic Parameters = [" << vcl_endl << P._d_matrix << "]" << vcl_endl;
  return s;
}


//---------------------------------------------------------------
//: Test --

void PMatrixDec::Test()
{
  vnl_matrix<double> matrix1(3,4);
  matrix1(0,0)= 568.051; matrix1(1,0)= -83.1082;matrix1(2,0)= -0.274578;
  matrix1(0,1)= 9.18145; matrix1(1,1)= 689.13;  matrix1(2,1)= -0.0458282;
  matrix1(0,2)= 552.506; matrix1(1,2)= 194.806; matrix1(2,2)= 0.960472;
  matrix1(0,3)= -596.353;matrix1(1,3)= 92.4159; matrix1(2,3)= 0.228363;

  vcl_cout << "Correct Matrix: " << vcl_endl;
  vcl_cout << matrix1 << vcl_endl;
  PMatrixDec pmat1(matrix1);
  vcl_cout << pmat1;
  vnl_matrix<double> J(3,4);
  J.update(pmat1.IntrinsicParameters(), 0, 0); // Copy columns 0,1 and 2
  J(0,3) = 0.0; // Last column = 0
  J(1,3) = 0.0;
  J(2,3) = 0.0;
  vcl_cout << "P = [J O_3]*D = [" << vcl_endl
       << J * pmat1.ExtrinsicParameters() << "]" << vcl_endl;
  vcl_cout << "AlphaU=" << pmat1.GetAlphaU() << vcl_endl;
  vcl_cout << "AlphaV=" << pmat1.GetAlphaV() << vcl_endl;
  vcl_cout << "U0=" << pmat1.GetU0() << vcl_endl;
  vcl_cout << "V0=" << pmat1.GetV0() << vcl_endl;

  vnl_matrix<double> matrix2(4, 4);
  int i,j;
  for (j=0; j<4; j++)
    for (i=0; i<4; i++)
      matrix2(i,j)= 1.0;

  vcl_cout << "Another Incorrect Matrix: " << vcl_endl;
  vcl_cout << matrix2 << vcl_endl;
  PMatrixDec pmat2(matrix2);
  vcl_cout << pmat2;

  vnl_matrix<double> matrix3(2, 2);
  for (j=0; j<2; j++)
    for (i=0; i<2; i++)
      matrix3(i,j)= 2.0;

  vcl_cout << "Incorrect Matrix: " << vcl_endl;
  vcl_cout << matrix3 << vcl_endl;
  PMatrixDec pmat3(matrix3);
  vcl_cout << pmat3;
}
