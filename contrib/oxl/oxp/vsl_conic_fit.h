//-*- c++ -*-------------------------------------------------------------------
#ifndef vsl_conic_fit_h_
#define vsl_conic_fit_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : vsl_conic_fit
//
// .SECTION Description
//    vsl_conic_fit is a class that awf hasn't documented properly. FIXME
//
// .NAME        vsl_conic_fit - Undocumented class FIXME
// .LIBRARY     POX
// .HEADER	Oxford Package
// .INCLUDE     oxp/vsl_conic_fit.h
// .FILE        vsl_conic_fit.h
// .FILE        vsl_conic_fit.C
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 22 Nov 99
//
//-----------------------------------------------------------------------------

#include <IUE_stlfwd.h>
#include <vnl/vnl_float_2.h>

struct vsl_conic_as_params;
struct vsl_conic_fit {

  vsl_conic_as_params fpf(vnl_float_2 const* points, unsigned int n);
  
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vsl_conic_fit.

