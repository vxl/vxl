//-*- c++ -*-------------------------------------------------------------------
#ifndef vnl_double_4x4_h_
#define vnl_double_4x4_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : vnl_double_4x4
//
// .SECTION Description
//    vnl_double_4x4 is a vnl_matrix<double> of fixed size 4x4.  It is
//    merely a typedef for vnl_matrix_fixed<double,4,4>
//
// .NAME        vnl_double_4x4 - 4x4 Matrix of double
// .LIBRARY     vnl
// .HEADER	Numerics package
// .INCLUDE     vnl/vnl_double_4x4.h
// .FILE        vnl/vnl_double_4x4.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 04 Aug 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_matrix_fixed.h>

typedef vnl_matrix_fixed<double,4,4> vnl_double_4x4;

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_double_4x4.
