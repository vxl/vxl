// This is oxl/vgui/vgui_style.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   18 Oct 99
// \brief  See vgui_style.h for a description of this file.
//
// \verbatim
//  Modifications:
//    18-OCT-1999 P.Pritchett - Initial version.
// \endverbatim

#include "vgui_style.h"

vgui_style::vgui_style() {
  for (int i=0; i<2; ++i)
    rgba[i] = 1;

  rgba[3] = 1;
  line_width = 1;
  point_size = 1;
}

vgui_style::~vgui_style() {
  // to aid in debugging destroyed styles
  for (int i=0; i<2; ++i)
    rgba[i] = -1.0f;

  rgba[3] = -1.0f;
  line_width = -1.0f;
  point_size = -1.0f;
}
