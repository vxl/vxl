#ifndef vnl_chi_squared_h_
#define vnl_chi_squared_h_
#ifdef __GNUC__
#pragma interface
#endif

// .NAME	vnl_chi_squared
// .LIBRARY	vnl-algo
// .HEADER	vxl Package
// .INCLUDE	vnl/algo/vnl_chi_squared.h
// .FILE	vnl_chi_squared.cxx
//
// .SECTION Description
//   Name space for various chi-squared distribution functions.  
//
// .SECTION Author
//   Rupert Curwen, GE CRD, August 18th, 1998

//: Compute cumulative distribution function value for chi-squared distribution
extern float vnl_chi_squared_cumulative(float chisq, int dof);

//------------------------------------------------------------

// *DO NOT* add scale factors to these functions or you will break 
// the code written by those who read the documentation. fsm.

//      (A[i] - B[i])^2
// \sum ---------------
//   i       A[i]
double vnl_chi_squared_statistic_1 (int const *A, int const *B, int n);

//      (A[i] - B[i])^2
// \sum ---------------
//   i       B[i]
double vnl_chi_squared_statistic_2 (int const *A, int const *B, int n);

//      (A[i] - B[i])^2
// \sum ---------------
//   i    A[i] + B[i]
double vnl_chi_squared_statistic_12(int const *A, int const *B, int n);

#endif // vnl_chi_squared_h_
