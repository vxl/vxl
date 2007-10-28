// This is brl/bbas/bgui/bgui_image_tableau.cxx
#include "bgui_image_tableau.h"
//:
// \file
// \author J.L. Mundy
// \brief  See bgui_image_tableau.h for a description of this file.

#include <vcl_cmath.h>
#include <vcl_cstdio.h> // sprintf
#include <vgui/vgui_event.h>
#include <vgui/vgui.h>
#include <vgui/vgui_projection_inspector.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_rgba.h>
#include <vil/vil_image_view.h>


//-----------------------------------------------------------------------------
//: The constructor takes a snapshot of the current viewport and scissor areas.
//  The destructor restores that state.


//--------------------------------------------------------------------------------

bgui_image_tableau::bgui_image_tableau()
  { handle_motion_ = true; locked_ = false; show_path_=false;}

bgui_image_tableau::bgui_image_tableau(vil_image_resource_sptr const & img,
                                       vgui_range_map_params_sptr const& rmp)
 : base(img, rmp) { handle_motion_ = true; locked_ = false; show_path_=false;}

bgui_image_tableau::bgui_image_tableau(vil_image_view_base const & img,
                                       vgui_range_map_params_sptr const& rmp)
 : base(img, rmp) { handle_motion_ = true; locked_ = false; show_path_=false;}

bgui_image_tableau::bgui_image_tableau(vil1_image const & img,
                                       vgui_range_map_params_sptr const& rmp)
 : base(img, rmp) { handle_motion_ = true; locked_ = false; show_path_=false;}

bgui_image_tableau::bgui_image_tableau(char const *f,
                                       vgui_range_map_params_sptr const& rmp)
 : base(f, rmp) { handle_motion_ = true; locked_ = false; show_path_=false;}

//--------------------------------------------------------------------------------

vil1_image bgui_image_tableau::get_image() const
{
  return base::get_image();
}

vil_image_resource_sptr bgui_image_tableau::get_image_resource() const
{
  return base::get_image_resource();
}

void bgui_image_tableau::get_pixel_info_from_frame_buffer(const int x, const int y, vgui_event const &e, char* msg)
{
  // It's easier to get the buffer in vil1_rgba format and then convert to
  // RGB, because that avoids alignment problems with glReadPixels.
  glPixelZoom(1,1);
  glPixelTransferi(GL_MAP_COLOR,0);
  glPixelTransferi(GL_RED_SCALE,1);   glPixelTransferi(GL_RED_BIAS,0);
  glPixelTransferi(GL_GREEN_SCALE,1); glPixelTransferi(GL_GREEN_BIAS,0);
  glPixelTransferi(GL_BLUE_SCALE,1);  glPixelTransferi(GL_BLUE_BIAS,0);

  glPixelStorei(GL_PACK_ALIGNMENT,1);   // byte alignment.
  glPixelStorei(GL_PACK_ROW_LENGTH,0);  // use default value (the arg to pixel routine).
  glPixelStorei(GL_PACK_SKIP_PIXELS,0); //
  glPixelStorei(GL_PACK_SKIP_ROWS,0);   //
  vil1_rgba<GLubyte> pixel;
  glReadPixels(e.wx, e.wy,             //
               1, 1,             // height and width (only one pixel)
               GL_RGBA,          // format
               GL_UNSIGNED_BYTE, // type
               &pixel);
  vcl_sprintf(msg, "(%d, %d)   (ubyte)[ R=%d,G=%d,B=%d]", x, y,
              pixel.r, pixel.g, pixel.b);
}

void bgui_image_tableau::
get_pixel_info_from_image(const int x, const int y,
                          vgui_event const &e, char* msg)
{
  //only implemented for vil image resource and common vil types
  //(I don't like macros)
  vil_image_resource_sptr r = this->get_image_resource();
  if (!r)
    if (!this->get_image())
    {
      vcl_sprintf(msg, "(%d, %d)   ?", x, y);
      return;
    }
    else
    {
      this->get_pixel_info_from_frame_buffer(x, y, e, msg);
      return;
    }
  //At this point, we have a vil_image and can proceed with the cases.
  unsigned w = r->ni(), h = r->nj(), n_p = r->nplanes();
  if (x<0||x>=int(w)||y<0||y>=int(h))
  {
    vcl_sprintf(msg, "(%d, %d)   ?", 0, 0);
    return;
  }
  vil_pixel_format type = r->pixel_format();
  switch (type )
  {
   case VIL_PIXEL_FORMAT_BOOL: {
    vil_image_view<bool> v = r->get_view();
    if (!v)
      vcl_sprintf(msg, "Pixel Not Available");
    else
      vcl_sprintf(msg, "(%d, %d)   (bool) %d", x, y, v(x,y));
    return; }
   case  VIL_PIXEL_FORMAT_BYTE: {
    if (n_p==1)
    {
      vil_image_view<vxl_byte> v = r->get_view(x,1,y,1);
      if (!v)
        vcl_sprintf(msg, "Pixel Not Available");
      else
        vcl_sprintf(msg, "(%d, %d)   (ubyte) %d", x, y, v(0,0));
      return;
    }
    else if (n_p==3)
    {
      vil_image_view<vxl_byte > v = r->get_view(x,1,y,1);
      if (!v)
        vcl_sprintf(msg, "Pixel Not Available");
      else
        vcl_sprintf(msg, "(%d, %d)   (ubyte)[ R=%d,G=%d,B=%d]", x, y,
                    v(0,0,0), v(0,0,1), v(0,0,2) );
      return;
    }
    return; }
   case  VIL_PIXEL_FORMAT_SBYTE: {
    if (n_p==1)
    {
      vil_image_view<vxl_sbyte> v = r->get_view(x,1,y,1);
      if (!v)
        vcl_sprintf(msg, "Pixel Not Available");
      else
        vcl_sprintf(msg, "(%d, %d)   (sbyte) %d", x, y, v(0,0));
      return;
    }
    else if (n_p==3)
    {
      vil_image_view<vil_rgb<vxl_sbyte> > v = r->get_view(x,1,y,1);
      if (!v)
        vcl_sprintf(msg, "Pixel Not Available");
      else
        vcl_sprintf(msg, "(%d, %d)   (sbyte)[ R=%d,G=%d,B=%d]", x, y,
                    v(0,0).R(), v(0,0).G(),v(0,0).B() );
      return;
    }
    return; }
   case  VIL_PIXEL_FORMAT_UINT_16: {
    if (n_p==1)
    {
      vil_image_view<vxl_uint_16> v = r->get_view(x,1,y,1);
      if (!v)
        vcl_sprintf(msg, "Pixel Not Available");
      else
        vcl_sprintf(msg, "(%d, %d)   (uint16) %d", x, y, v(0,0));
      return;
    }
    else if (n_p==3)
    {
      vil_image_view<vil_rgb<vxl_uint_16> > v = r->get_view(x,1,y,1);
      if (!v)
        vcl_sprintf(msg, "Pixel Not Available");
      else
        vcl_sprintf(msg, "(%d, %d)   (uint16)[ R=%d,G=%d,B=%d]", x, y,
                    v(0,0).R(), v(0,0).G(),v(0,0).B() );
      return;
    }
    return; }
   case  VIL_PIXEL_FORMAT_INT_16: {
    if (n_p==1)
    {
      vil_image_view<vxl_int_16> v = r->get_view(x,1,y,1);
      if (!v)
        vcl_sprintf(msg, "Pixel Not Available");
      else
        vcl_sprintf(msg, "(%d, %d)   (int16) %d", x, y, v(0,0));
      return;
    }
    else if (n_p==3)
    {
      vil_image_view<vil_rgb<vxl_int_16> > v = r->get_view(x,1,y,1);
      if (!v)
        vcl_sprintf(msg, "Pixel Not Available");
      else
        vcl_sprintf(msg, "(%d, %d)   (int16)[ R=%d,G=%d,B=%d]", x, y,
                    v(0,0).R(), v(0,0).G(),v(0,0).B() );
      return;
    }
    return; }
   case  VIL_PIXEL_FORMAT_FLOAT: {
    vil_image_view<float> v = r->get_view(x,1,y,1);
    if (!v)
      vcl_sprintf(msg, "Pixel Not Available");
    else
      vcl_sprintf(msg, "(%d, %d)   (float) %f", x, y, v(0,0));
    return; }
   case  VIL_PIXEL_FORMAT_DOUBLE: {
    vil_image_view<double> v = r->get_view(x,1,y,1);
    if (!v)
      vcl_sprintf(msg, "Pixel Not Available");
    else
      vcl_sprintf(msg, "(%d, %d)   (double) %g", x, y, v(0,0));
    return; }
   case VIL_PIXEL_FORMAT_RGB_BYTE: {
    vil_image_view<vil_rgb<vxl_byte> > v = r->get_view(x,1,y,1);
    if (!v)
      vcl_sprintf(msg, "Pixel Not Available");
    else
      vcl_sprintf(msg, "(%d, %d)   (ubyte)[ R=%d,G=%d,B=%d]", x, y,
                  v(0,0).R(), v(0,0).G(),v(0,0).B() );
      return; }
   case VIL_PIXEL_FORMAT_RGB_UINT_16: {
    vil_image_view<vil_rgb<vxl_uint_16> > v = r->get_view(x,1,y,1);
    if (!v)
      vcl_sprintf(msg, "Pixel Not Available");
    else
      vcl_sprintf(msg, "(%d, %d)   (ubyte)[ R=%d,G=%d,B=%d]", x, y,
                  v(0,0).R(), v(0,0).G(),v(0,0).B() );
    return; }
#if 0
   case VIL_PIXEL_FORMAT_UINT_32:
   case VIL_PIXEL_FORMAT_INT_32:
   case VIL_PIXEL_FORMAT_RGB_SBYTE:
   case VIL_PIXEL_FORMAT_RGB_INT_16:
   case VIL_PIXEL_FORMAT_RGB_UINT_32:
   case VIL_PIXEL_FORMAT_RGB_INT_32:
   case VIL_PIXEL_FORMAT_RGB_FLOAT:
   case VIL_PIXEL_FORMAT_RGB_DOUBLE:
   case VIL_PIXEL_FORMAT_RGBA_UINT_32:
   case VIL_PIXEL_FORMAT_RGBA_INT_32:
   case VIL_PIXEL_FORMAT_RGBA_UINT_16:
   case VIL_PIXEL_FORMAT_RGBA_INT_16:
   case VIL_PIXEL_FORMAT_RGBA_BYTE:
   case VIL_PIXEL_FORMAT_RGBA_SBYTE:
   case VIL_PIXEL_FORMAT_RGBA_FLOAT:
   case VIL_PIXEL_FORMAT_RGBA_DOUBLE:
   case VIL_PIXEL_FORMAT_COMPLEX_FLOAT:
   case VIL_PIXEL_FORMAT_COMPLEX_DOUBLE:
#endif
   default:
    vcl_sprintf(msg, "Pixel Not Available");
  }
}

double bgui_image_tableau::
get_pixel_value(const unsigned c, const unsigned r)
{
  vil_image_resource_sptr rs = this->get_image_resource();
  if (!rs)
    return 0;
  if (c<0||c>=rs->ni()||r<0||r>=rs->nj())
    return 0;
  unsigned n_p = rs->nplanes();
  vil_pixel_format type = rs->pixel_format();
  switch (type )
    {
    case VIL_PIXEL_FORMAT_BOOL: {
      vil_image_view<bool> v = rs->get_view();
      if (!v)
        return 0;
      else
        return static_cast<double>(v(0,0));
    }
    case  VIL_PIXEL_FORMAT_BYTE: {

      vil_image_view<vxl_byte> v = rs->get_view(c,1,r,1);
      if (!v)
        return 0;
      if (n_p==1)
        return static_cast<double>(v(0,0));
      else if (n_p==3)
        return static_cast<double>(v(0,0,0)+v(0,0,1)+v(0,0,2))/3;
    }
    case  VIL_PIXEL_FORMAT_SBYTE: {
      vil_image_view<vxl_sbyte> v = rs->get_view(c,1,r,1);
      if (!v)
        return 0;
      else
        if (n_p==1)
          return static_cast<double>(v(0,0));
        else if (n_p==3)
          return static_cast<double>(v(0,0,0)+v(0,0,1)+v(0,0,2))/3;
    }
    case  VIL_PIXEL_FORMAT_UINT_16: {
      vil_image_view<vxl_uint_16> v = rs->get_view(c,1,r,1);
      if (!v)
        return 0;
      else
        if (n_p==1)
          return static_cast<double>(v(0,0));
        else if (n_p==3)
          return static_cast<double>(v(0,0,0)+v(0,0,1)+v(0,0,2))/3;
    }
    case  VIL_PIXEL_FORMAT_INT_16: {
      vil_image_view<vxl_int_16> v = rs->get_view(c,1,r,1);
      if (!v)
        return 0;
      else
        if (n_p==1)
          return static_cast<double>(v(0,0));
        else if (n_p==3)
          return static_cast<double>(v(0,0,0)+v(0,0,1)+v(0,0,2))/3;
    }
    case  VIL_PIXEL_FORMAT_FLOAT: {
      vil_image_view<float> v = rs->get_view(c,1,r,1);
      if (!v)
        return 0;
      else
        return static_cast<double>(v(0,0));
    }
    case  VIL_PIXEL_FORMAT_DOUBLE: {
      vil_image_view<double> v = rs->get_view(c,1,r,1);
      if (!v)
        return 0;
      else
        return v(0,0);
    }
    case VIL_PIXEL_FORMAT_RGB_BYTE: {
      vil_image_view<vil_rgb<vxl_byte> > v = rs->get_view(c,1,r,1);
      if (!v)
        return 0;
      else
        return static_cast<double>(v(0,0).R()+v(0,0).G()+v(0,0).B())/3;
    }
    case VIL_PIXEL_FORMAT_RGB_UINT_16: {
      vil_image_view<vil_rgb<vxl_uint_16> > v = rs->get_view(c,1,r,1);
      if (!v)
        return 0;
      else
        return static_cast<double>(v(0,0).R()+v(0,0).G()+v(0,0).B())/3;
    }
    default:
      return 0;
    }
}

void bgui_image_tableau::image_line(const float col_start,
                                    const float row_start,
                                    const float col_end,
                                    const float row_end,
                                    vcl_vector<double>& line_pos,
                                    vcl_vector<double>& vals)
{
  line_pos.clear();vals.clear();
  //Get the image data
  // the line length in pixels
  float length = vcl_sqrt((col_end-col_start)*(col_end-col_start) +
                          (row_end-row_start)*(row_end-row_start));
  if (length == 0)
    return;
  //initialize the line scan parameters
  float xstep = (col_end-col_start)/length;
  float ystep = (row_end-row_start)/length;
  float sinc = vcl_sqrt(xstep*xstep + ystep*ystep);
  float spos = 0;
  line_pos.push_back(spos);
  unsigned c = static_cast<unsigned>(col_start),
   r = static_cast<unsigned>(row_start);
  vals.push_back(get_pixel_value(c, r));

  //extract the pixel values along the line
  float xpos = col_start, ypos = row_start;
  unsigned nsteps = static_cast<unsigned>(length);
  for (unsigned i = 0; i<nsteps; ++i)
  {
    xpos += xstep;
    ypos += ystep;
    spos += sinc;
    c = static_cast<unsigned>(xpos);
    r = static_cast<unsigned>(ypos);
    line_pos.push_back(spos);
    vals.push_back(get_pixel_value(c, r));
  }
}

//--------------------------------------------------------------------------------
//:
// Handle all events for this tableau.
bool bgui_image_tableau::handle(vgui_event const &e)
{
  static bool button_down = false;
  if (e.type == vgui_DRAW)
  {
    base::handle(e);
    return true;
  }

  if (e.type == vgui_BUTTON_DOWN)
  {
    button_down = true;
    if (handle_motion_)
      vgui::out << ' ' << vcl_endl;
  }
  else if (e.type == vgui_BUTTON_UP)
  {
    button_down = false;
  }
  else if (e.type == vgui_MOTION && handle_motion_&&button_down == false)
  {
    // Get X,Y position to display on status bar:
    float pointx, pointy;
    vgui_projection_inspector p_insp;
    p_insp.window_to_image_coordinates(e.wx, e.wy, pointx, pointy);
    int intx = (int)vcl_floor(pointx), inty = (int)vcl_floor(pointy);
    char msg[100];

    this->get_pixel_info_from_image(intx, inty,e, msg);

    // gets a snapshot of the viewport so that it restores it back after vgui::out
    // this is needed, because vgui::out changes viewport, and image tableau 
    // gets confused
    bgui_image_tableau_vp_sc_snapshot snap;

    // Display on status bar:
    if (!locked_) {
      if(show_path_)
        vgui::out << msg << "  " << this->file_name() << vcl_endl;
      else
        vgui::out << msg << vcl_endl;
    }
  }
  return base::handle(e);
}

//--------------------------------------------------------------------------------
