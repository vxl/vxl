#ifndef vsl_conic_fit_h_
#define vsl_conic_fit_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME	vsl_conic_fit
// .LIBRARY	POX
// .HEADER	Oxford Package
// .INCLUDE	oxp/vsl_conic_fit.h
// .FILE	vsl_conic_fit.cxx
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

#endif // vsl_conic_fit_h_
