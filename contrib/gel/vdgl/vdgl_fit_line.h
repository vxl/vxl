// This is gel/vdgl/vdgl_fit_line.h
#ifndef vdgl_fit_line_h
#define vdgl_fit_line_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Fits a line to a set of 2D points given as a vdgl_edgel_chain

#include <vdgl/vdgl_edgel_chain.h>
#include <vsol/vsol_line_2d_sptr.h>

vsol_line_2d_sptr vdgl_fit_line(vdgl_edgel_chain &chain);

#endif // vdgl_fit_line_h
