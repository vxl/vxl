//-*- c++ -*-------------------------------------------------------------------
#ifndef vsl_conic_as_matrix_h_
#define vsl_conic_as_matrix_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : vsl_conic_as_matrix
//
// .SECTION Description
//    vsl_conic_as_matrix is a class that awf hasn't documented properly. FIXME
//
// .NAME        vsl_conic_as_matrix - Undocumented class FIXME
// .LIBRARY     POX
// .HEADER	Oxford Package
// .INCLUDE     oxp/vsl_conic_as_matrix.h
// .FILE        vsl_conic_as_matrix.h
// .FILE        vsl_conic_as_matrix.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 12 Nov 99
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_double_3x3.h>

// -- 2D Conic section, stored as 3x3 matrix
// [
//   Axx   Axy/2 Ax/2
//   Axy/2 Ayy   Ay/2
//   Ax/2  Ay/2  Ao
// ]
class vsl_conic_as_matrix {
  vnl_double_3x3 A_;
  
 public:
  vsl_conic_as_matrix(double cx, double cy, double rx, double ry, double theta);
  vsl_conic_as_matrix(const vnl_matrix<double>& A) : A_(A){}
  vnl_double_3x3 A() const { return A_; }
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vsl_conic_as_matrix.

