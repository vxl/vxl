#ifdef __GNUG__
#pragma implementation
#endif

#include "PMatrixDec.h"

#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h>

//--------------------------------------------------------------
// -- Constructor

PMatrixDec::PMatrixDec(const vnl_matrix<double>& p_matrix)
  : PMatrix(p_matrix), _j_matrix(3,3), _d_matrix(4,4)
{
  if (!((p_matrix.rows() == 3) && (p_matrix.columns() == 4)))
    cout << "PMatrixDec WARNING: Incorrect size of matrix" << endl;
  Init();
}


//--------------------------------------------------------------
// -- Destructor

PMatrixDec::~PMatrixDec() {}


//--------------------------------------------------------------
// -- Auxiliar Constructor

void PMatrixDec::Init()
{
    double u0 = _p_matrix(0,0)*_p_matrix(2,0) 
              + _p_matrix(0,1)*_p_matrix(2,1)
              + _p_matrix(0,2)*_p_matrix(2,2);
    double v0 = _p_matrix(1,0)*_p_matrix(2,0) 
              + _p_matrix(1,1)*_p_matrix(2,1)
              + _p_matrix(1,2)*_p_matrix(2,2);

    double a_u = _p_matrix(0,0)*_p_matrix(0,0) 
               + _p_matrix(0,1)*_p_matrix(0,1)
               + _p_matrix(0,2)*_p_matrix(0,2);
    double a_v = _p_matrix(1,0)*_p_matrix(1,0) 
               + _p_matrix(1,1)*_p_matrix(1,1)
               + _p_matrix(1,2)*_p_matrix(1,2);
    a_u = a_u-u0*u0;
    a_v = a_v-v0*v0;
    if ((a_u <= 0.0) || (a_v <= 0.0))
      cout << "PMatrixDec WARNING: Incorrect projection matrix" << endl;
    else
    {
      a_u = sqrt(a_u);
      a_v = sqrt(a_v);

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
      _d_matrix(0,0) = (_p_matrix(0,0) - u0*_p_matrix(2,0))/a_u; 
      _d_matrix(0,1) = (_p_matrix(0,1) - u0*_p_matrix(2,1))/a_u; 
      _d_matrix(0,2) = (_p_matrix(0,2) - u0*_p_matrix(2,2))/a_u; 
      _d_matrix(0,3) = (_p_matrix(0,3) - u0*_p_matrix(2,3))/a_u; 
    
      _d_matrix(1,0) = (_p_matrix(1,0) - v0*_p_matrix(2,0))/a_v; 
      _d_matrix(1,1) = (_p_matrix(1,1) - v0*_p_matrix(2,1))/a_v; 
      _d_matrix(1,2) = (_p_matrix(1,2) - v0*_p_matrix(2,2))/a_v; 
      _d_matrix(1,3) = (_p_matrix(1,3) - v0*_p_matrix(2,3))/a_v;
    
      _d_matrix(2,0) = _p_matrix(2,0); 
      _d_matrix(2,1) = _p_matrix(2,1); 
      _d_matrix(2,2) = _p_matrix(2,2); 
      _d_matrix(2,3) = _p_matrix(2,3); 

      _d_matrix(3,0) = 0.0; 
      _d_matrix(3,1) = 0.0; 
      _d_matrix(3,2) = 0.0; 
      _d_matrix(3,3) = 1.0; // last diagonal element = 1
    }
}


//---------------------------------------------------------------
// -- Print to ostream
ostream& operator<<(ostream& s, const PMatrixDec& P) 
{
  s << "PROJECTION MATRIX = [" << endl << P.get_matrix() << "]" << endl 
    << "DECOMPOSITION: " << endl
    << "Intrinsic Parameters = [" << endl << P._j_matrix << "]" << endl 
    << "Extrinsic Parameters = [" << endl << P._d_matrix << "]" << endl;
  return s;
}


//---------------------------------------------------------------
// -- Test --

void PMatrixDec::Test()
{
  vnl_matrix<double> matrix1(3,4);
  matrix1(0,0)= 568.051; matrix1(1,0)= -83.1082;matrix1(2,0)= -0.274578;
  matrix1(0,1)= 9.18145; matrix1(1,1)= 689.13;  matrix1(2,1)= -0.0458282;
  matrix1(0,2)= 552.506; matrix1(1,2)= 194.806; matrix1(2,2)= 0.960472;
  matrix1(0,3)= -596.353;matrix1(1,3)= 92.4159; matrix1(2,3)= 0.228363;

  cout << "Correct Matrix: " << endl;
  cout << matrix1 << endl;
  PMatrixDec pmat1(matrix1);
  cout << pmat1;
  vnl_matrix<double> J(3,4);
  J.update(pmat1.IntrinsicParameters(), 0, 0); // Copy columns 0,1 and 2
  J(0,3) = 0.0; // Last column = 0
  J(1,3) = 0.0;
  J(2,3) = 0.0;  
  cout << "P = [J O_3]*D = [" << endl
       << J * pmat1.ExtrinsicParameters() << "]" << endl;
  cout << "AlphaU=" << pmat1.GetAlphaU() << endl;
  cout << "AlphaV=" << pmat1.GetAlphaV() << endl;
  cout << "U0=" << pmat1.GetU0() << endl;
  cout << "V0=" << pmat1.GetV0() << endl;

  vnl_matrix<double> matrix2(4, 4);
  int i,j;
  for (j=0; j<4; j++)
    for (i=0; i<4; i++)
      matrix2(i,j)= 1.0;  

  cout << "Another Incorrect Matrix: " << endl;
  cout << matrix2 << endl;
  PMatrixDec pmat2(matrix2);
  cout << pmat2;

  vnl_matrix<double> matrix3(2, 2);
  for (j=0; j<2; j++)
    for (i=0; i<2; i++)
      matrix3(i,j)= 2.0;  

  cout << "Incorrect Matrix: " << endl;
  cout << matrix3 << endl;
  PMatrixDec pmat3(matrix3);
  cout << pmat3;
}
