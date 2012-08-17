// This is oxl/xcv/xcv_multiview_create_multiview_menu.cxx
#include "xcv_multiview.h"
//:
//  \file
// See xcv_multiview.h for a description of this file.
//
// \author  Andrew Fitzgibbon

#include <vgui/vgui_menu.h>

//-----------------------------------------------------------------------------
//: Creates a menu containing all the functions in xcv_multiview
//-----------------------------------------------------------------------------
vgui_menu xcv_multiview::create_multiview_menu()
{
  vgui_menu mult_menu;
  //---- Two view menu ---------------------
  {
    vgui_menu two_menu;

#if 0  // kym - none of this compute stuff works so I'm commenting it out
       // If you are looking for the versions of F,H and T compute available
       // in the version of xcv on the VXL webpage -  for copyright reasons,
       // the code for these functions is not available to people outside
       // Oxford University
    {
      vgui_menu twocompute_menu;
      twocompute_menu.add("Compute FMatrix", compute_f_matrix);
      twocompute_menu.add("Compute HMatrix2D", compute_h_matrix2d);
      twocompute_menu.add("Compute corner matches (not yet implemented)",
                          compute_corner_matches);
      two_menu.add("Compute", twocompute_menu);
    }
#endif

    {
      vgui_menu twoload_menu;
      twoload_menu.add("Load FMatrix", load_f_matrix);
      twoload_menu.add("Load HMatrix2D", load_h_matrix2d);
      twoload_menu.add("Load corner matches (not yet implemented)",
                       load_corner_matches);
      //twoload_menu.add("Load line matches", twoview);
      two_menu.add("Load", twoload_menu);
    }

    {
      vgui_menu twosave_menu;
      twosave_menu.add("Save FMatrix", save_f_matrix);
      twosave_menu.add("Save HMatrix2D", save_h_matrix2d);
      twosave_menu.add("Save corner matches", save_corner_matches);
      two_menu.add("Save", twosave_menu);
    }

    {
      vgui_menu twodisp_menu;
      twodisp_menu.add("Toggle display FMatrix", toggle_f_matrix);
      twodisp_menu.add("Toggle display HMatrix2D", toggle_h_matrix);
      //twodisp_menu.add("Display corner matches", display_corner_matches);
      twodisp_menu.add("Display corner tracks", display_corner_tracks);
      two_menu.add("Display", twodisp_menu);
    }
    mult_menu.add("Two view", two_menu);
  }

  //---- Three view menu ---------------------
  {
    vgui_menu three_menu;
    {
      vgui_menu threeload_menu;
      threeload_menu.add("Load TriTensor", load_tri_tensor);
      three_menu.add("Load", threeload_menu);
    }
    {
      vgui_menu threesave_menu;
      threesave_menu.add("Save TriTensor", save_tri_tensor);
      three_menu.add("Save", threesave_menu);
    }
    {
      vgui_menu threedisp_menu;
      threedisp_menu.add("Toggle display TriTensor", toggle_tri_tensor);
      threedisp_menu.add("Transfer point", transfer_point);
      threedisp_menu.add("Transfer line", transfer_line);
      three_menu.add("Display", threedisp_menu);
    }
    mult_menu.add("Three view", three_menu);
  }

  return mult_menu;
}
