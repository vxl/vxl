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

#endif
