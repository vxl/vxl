//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation
#endif
//
// .NAME vgui_style
// Author: Philip C. Pritchett, RRG, University of Oxford
// Created: 18 Oct 99
//
//-----------------------------------------------------------------------------

#include "vgui_style.h"
#include <vcl_iostream.h>
#include <vul/vul_sprintf.h>


vgui_style::vgui_style() {
  for (int i=0; i<2; ++i)
    rgba[i] = 1;

  rgba[3] = 1;
  line_width = 1;
  point_size = 1;
}
