//-*- c++ -*-------------------------------------------------------------------
#ifndef vsl_conic_as_params_h_
#define vsl_conic_as_params_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : vsl_conic_as_params
//
// .SECTION Description
//    vsl_conic_as_params is a class that awf hasn't documented properly. FIXME
//
// .NAME        vsl_conic_as_params - Undocumented class FIXME
// .LIBRARY     POX
// .HEADER	Oxford Package
// .INCLUDE     oxp/vsl_conic_as_params.h
// .FILE        vsl_conic_as_params.h
// .FILE        vsl_conic_as_params.C
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 12 Nov 99
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_vector_fixed.h>

// -- 2D conic section, stored as a 6-vector [Axx Axy Ayy Ax Ay Ao]
struct vsl_conic_as_params : public vnl_vector_fixed<double, 6> {
  typedef vnl_vector_fixed<double, 6> base;
  
  vsl_conic_as_params(double cx, double cy, double rx, double ry, double theta);
  vsl_conic_as_params(const vnl_vector<double>& a) : base(a) {}
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vsl_conic_as_params.

