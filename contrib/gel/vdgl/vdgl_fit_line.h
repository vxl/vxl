// This is gel/vdgl/vdgl_fit_line.h
#ifndef vdgl_fit_line_h
#define vdgl_fit_line_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Fits a line to a set of points defined in vdgl_edgel_chain
//
//  Fits a line to 2D edgel chain

#include <vdgl/vdgl_edgel_chain.h>
#include  <vsol/vsol_line_2d_sptr.h>
#include <vdgl/vdgl_ortho_regress.h>
#include  <vdgl/vdgl_OrthogRegress.h>

vsol_line_2d_sptr vdgl_fit_line(vdgl_edgel_chain &chain);

#endif // vdgl_fit_lines_h
