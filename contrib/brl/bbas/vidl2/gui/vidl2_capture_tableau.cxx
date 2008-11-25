// This is brl/bbas/vidl2/gui/vidl2_capture_tableau.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief  Tableau for capturing OpenGL rendering to a video
// \author Matt Leotta
// \date   21 Nov 08

#include "vidl2_capture_tableau.h"
#include <vil/vil_image_view.h>
#include <vgui/vgui_utils.h>
#include <vgui/vgui_gl.h>
#include <vidl2/vidl2_convert.h>
#include <vidl2/gui/vidl2_gui_param_dialog.h>


vcl_string vidl2_capture_tableau::type_name() const
{
  return "vidl2_capture_tableau";
}


// Default constructor
vidl2_capture_tableau::vidl2_capture_tableau( vgui_tableau_sptr const& child)
  : vgui_wrapper_tableau( child),
    ostream_(NULL)
{
}


bool vidl2_capture_tableau::handle( vgui_event const &e)
{
  bool handled = false;
  if(child)
  {
    handled = child->handle(e);
    
    if(e.type == vgui_DRAW && ostream_)
    {
      vil_image_view<vxl_byte> image = vgui_utils::colour_buffer_to_view();
      vil_image_view_base_sptr img_sptr = new vil_image_view<vxl_byte>(image);
      ostream_->write_frame(vidl2_convert_to_frame(img_sptr));
    }
    if(e.type == vgui_RESHAPE)
    {
      GLint vp[4]; // x,y,w,h
      glGetIntegerv(GL_VIEWPORT, vp);
      vcl_cout << "window size = "<< vp[2] << ", "<<vp[3]<<vcl_endl;
    }
  }
  return handled;
}


//: Prompt the user to set an ostream with a gui dialog
void vidl2_capture_tableau::prompt_for_ostream()
{
  ostream_ = vidl2_gui_open_ostream_dialog();
}


//: Stop the video capture and close the ostream
void vidl2_capture_tableau::close_ostream()
{
  if(ostream_)
  {
    ostream_->close();
    ostream_ = NULL;
  }
}


