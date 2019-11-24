// This is brl/bbas/bgui/bgui_image_tableau.cxx
#include <iostream>
#include <cmath>
#include <cstdio>
#include "bgui_image_tableau.h"
//:
// \file
// \author J.L. Mundy
// \brief  See bgui_image_tableau.h for a description of this file.

#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vgui/vgui_event.h"
#include "vgui/vgui.h"
#include "vgui/vgui_projection_inspector.h"
#include "vil1/vil1_image.h"
#include "vil1/vil1_rgba.h"
#include "vil/vil_image_view.h"
#include "vgui/vgui_range_map_params.h"


//-----------------------------------------------------------------------------
//: The constructor takes a snapshot of the current viewport and scissor areas.
//  The destructor restores that state.
//--------------------------------------------------------------------------------

bgui_image_tableau::bgui_image_tableau()
{
  handle_motion_ = true; locked_ = false; show_path_=false;
  mouse_message_ = false;
  tt_ = new vgui_text_tableau();
}

bgui_image_tableau::bgui_image_tableau(vil_image_resource_sptr const & img,
                                       vgui_range_map_params_sptr const& rmp)
 : base(img, rmp) { handle_motion_ = true; locked_ = false; show_path_=false;
  mouse_message_ = false;
  capture_mouse_ = false;
  tt_ = new vgui_text_tableau();}

bgui_image_tableau::bgui_image_tableau(vil_image_view_base const & img,
                                       vgui_range_map_params_sptr const& rmp)
 : base(img, rmp) { handle_motion_ = true; locked_ = false; show_path_=false;
  mouse_message_ = false;
  capture_mouse_ = false;
 tt_ = new vgui_text_tableau();}

bgui_image_tableau::bgui_image_tableau(vil1_image const & img,
                                       vgui_range_map_params_sptr const& rmp)
 : base(img, rmp) { handle_motion_ = true; locked_ = false; show_path_=false;
  mouse_message_ = false;
  capture_mouse_ = false;
 tt_ = new vgui_text_tableau();}

bgui_image_tableau::bgui_image_tableau(char const *f,
                                       vgui_range_map_params_sptr const& rmp)
 : base(f, rmp) { handle_motion_ = true; locked_ = false; show_path_=false;
  mouse_message_ = false;
  capture_mouse_ = false;
 tt_ = new vgui_text_tableau();}

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
  std::sprintf(msg, "(%d, %d)   (ubyte)[ R=%d,G=%d,B=%d]", x, y,
              pixel.r, pixel.g, pixel.b);
}

void bgui_image_tableau::
get_pixel_info_from_image(const int x, const int y,
                          vgui_event const &e, char* msg)
{
  //only implemented for vil image resource and common vil types
  //(I don't like macros)
  vil_image_resource_sptr r = this->get_image_resource();
  if (!r) {
    if (!this->get_image())
    {
      std::sprintf(msg, "(%d, %d)   ?", x, y);
      return;
    }
    else
    {
      this->get_pixel_info_from_frame_buffer(x, y, e, msg);
      return;
    }
  }
  //At this point, we have a vil_image and can proceed with the cases.
  unsigned w = r->ni(), h = r->nj(), n_p = r->nplanes();
  if (x<0||x>=int(w)||y<0||y>=int(h))
  {
    std::sprintf(msg, "(%d, %d)   ?", 0, 0);
    return;
  }
  //note: will treat RGBA as a four plane type
  vil_pixel_format type = vil_pixel_format_component_format(r->pixel_format());

  switch (type )
  {
   case VIL_PIXEL_FORMAT_BOOL:
   {
    vil_image_view<bool> v = r->get_view();
    if (!v)
      std::sprintf(msg, "Pixel Not Available");
    else
      std::sprintf(msg, "(%d, %d)   (bool) %d", x, y, v(x,y));
    return;
   }
   case  VIL_PIXEL_FORMAT_BYTE:
   {
    if (n_p==1)
    {
      vil_image_view<vxl_byte> v = r->get_view(x,1,y,1);
      if (!v)
        std::sprintf(msg, "Pixel Not Available");
      else
        std::sprintf(msg, "(%d, %d)   (ubyte) %d", x, y, v(0,0));
      return;
    }
    else if (n_p==3)
    {
      vil_image_view<vxl_byte > v = r->get_view(x,1,y,1);
      if (!v)
        std::sprintf(msg, "Pixel Not Available");
      else
        std::sprintf(msg, "(%d, %d)   (ubyte)[ R=%d,G=%d,B=%d]", x, y,
                    v(0,0,0), v(0,0,1), v(0,0,2) );
      return;
    }
    else if (n_p==4) {
      vil_image_view<vxl_byte > v = r->get_view(x,1,y,1);
      if (!v)
        std::sprintf(msg, "Pixel Not Available");
      else//as a default, just display first three bands as RGB
        std::sprintf(msg, "(%d, %d) (ubyte)[ R=%d,G=%d,B=%d, X=%d]", x, y,
                    v(0,0,0), v(0,0,1), v(0,0,2), v(0,0,3));
      return;
    }
    return;
   }
   case  VIL_PIXEL_FORMAT_SBYTE:
   {
    if (n_p==1)
    {
      vil_image_view<vxl_sbyte> v = r->get_view(x,1,y,1);
      if (!v)
        std::sprintf(msg, "Pixel Not Available");
      else
        std::sprintf(msg, "(%d, %d)   (sbyte) %d", x, y, v(0,0));
      return;
    }
    else if (n_p==3)
    {
      vil_image_view<vil_rgb<vxl_sbyte> > v = r->get_view(x,1,y,1);
      if (!v)
        std::sprintf(msg, "Pixel Not Available");
      else
        std::sprintf(msg, "(%d, %d)   (sbyte)[ R=%d,G=%d,B=%d]", x, y,
                    v(0,0).R(), v(0,0).G(),v(0,0).B() );
      return;
    }
    else if (n_p==4) {
      vil_image_view<vxl_byte > v = r->get_view(x,1,y,1);
      if (!v)
        std::sprintf(msg, "Pixel Not Available");
      else//as a default, just display first three bands as RGB
        std::sprintf(msg, "(%d, %d) (sbyte)[ R=%d,G=%d,B=%d,X=%d]", x, y,
                    v(0,0,0), v(0,0,1), v(0,0,2), v(0,0,3));
      return;
    }
    return;
   }
   case  VIL_PIXEL_FORMAT_UINT_16:
   {
    if (n_p==1)
    {
      vil_image_view<vxl_uint_16> v = r->get_view(x,1,y,1);
      if (!v)
        std::sprintf(msg, "Pixel Not Available");
      else
        std::sprintf(msg, "(%d, %d)   (uint16) %d", x, y, v(0,0));
      return;
    }
    else if (n_p==3)
    {
      vil_image_view<vil_rgb<vxl_uint_16> > v = r->get_view(x,1,y,1);
      if (!v)
        std::sprintf(msg, "Pixel Not Available");
      else
        std::sprintf(msg, "(%d, %d)   (uint16)[ R=%d,G=%d,B=%d]", x, y,
                    v(0,0).R(), v(0,0).G(),v(0,0).B() );
    }
    else if (n_p==4)
    {
      vil_image_view<vil_rgba<vxl_uint_16> > v = r->get_view(x,1,y,1);
      if (!v) {
        std::sprintf(msg, "Pixel Not Available");
        return;
      }
      else
      {
        int band_map = 0;
        if (rmp_)
          band_map = rmp_->band_map_;
        switch (band_map)
        {
         case vgui_range_map_params::RGB_m :
          std::sprintf(msg, "(%d, %d)   (RGB:uint16)[ R=%d,G=%d,B=%d,I=%d]",
                      x, y, v(0,0).R(), v(0,0).G(),v(0,0).B(),v(0,0).A() );
          break;
         case vgui_range_map_params::XRG_m :
          std::sprintf(msg, "(%d, %d)   (IRG:uint16)[ R=%d,G=%d,B=%d,I=%d]",
                      x, y, v(0,0).R(), v(0,0).G(),v(0,0).B(),v(0,0).A() );
          break;
         case vgui_range_map_params::RXB_m :
          std::sprintf(msg, "(%d, %d)   (RIB:uint16)[ R=%d,G=%d,B=%d,I=%d]",
                      x, y, v(0,0).R(), v(0,0).G(),v(0,0).B(),v(0,0).A() );
          break;
         case vgui_range_map_params::RGX_m :
          std::sprintf(msg, "(%d, %d)   (RGI:uint16)[ R=%d,G=%d,B=%d,I=%d]",
                      x, y, v(0,0).R(), v(0,0).G(),v(0,0).B(),v(0,0).A() );
          break;
         default:
          std::sprintf(msg, "Pixel Not Available");
          return;
        }
      }
    }
   }
   case  VIL_PIXEL_FORMAT_INT_16: {
    if (n_p==1)
    {
      vil_image_view<vxl_int_16> v = r->get_view(x,1,y,1);
      if (!v)
        std::sprintf(msg, "Pixel Not Available");
      else
        std::sprintf(msg, "(%d, %d)   (int16) %d", x, y, v(0,0));
      return;
    }
    else if (n_p==3)
    {
      vil_image_view<vil_rgb<vxl_int_16> > v = r->get_view(x,1,y,1);
      if (!v)
        std::sprintf(msg, "Pixel Not Available");
      else
        std::sprintf(msg, "(%d, %d)   (int16)[ R=%d,G=%d,B=%d]", x, y,
                    v(0,0).R(), v(0,0).G(),v(0,0).B() );
      return;
    }
    return;
   }
   case  VIL_PIXEL_FORMAT_UINT_32: {
    if (n_p==1)
    {
      vil_image_view<vxl_uint_32> v = r->get_view(x,1,y,1);
      if (!v)
        std::sprintf(msg, "Pixel Not Available");
      else
        std::sprintf(msg, "(%d, %d)   (uint32) %d", x, y, v(0,0));
      return;
    }
    else if (n_p==3)
    {
      vil_image_view<vil_rgb<vxl_uint_32> > v = r->get_view(x,1,y,1);
      if (!v)
        std::sprintf(msg, "Pixel Not Available");
      else
        std::sprintf(msg, "(%d, %d)   (uint32)[ R=%d,G=%d,B=%d]", x, y,
                    v(0,0).R(), v(0,0).G(),v(0,0).B() );
      return;
    }
    return;
   }
   case  VIL_PIXEL_FORMAT_FLOAT: {
    vil_image_view<float> v = r->get_view(x,1,y,1);
    if (!v)
      std::sprintf(msg, "Pixel Not Available");
    else if (n_p == 1)
      std::sprintf(msg, "(%d, %d)   (float) %f", x, y, v(0,0));
    else if (n_p ==3)
      std::sprintf(msg, "(%d, %d)   (float)[ R=%6.3f,G=%6.3f,B=%6.3f]", x, y,
                  v(0,0,0), v(0,0,1), v(0,0,2) );
    return;
   }
   case  VIL_PIXEL_FORMAT_DOUBLE: {
    vil_image_view<double> v = r->get_view(x,1,y,1);
    if (!v)
      std::sprintf(msg, "Pixel Not Available");
    else
      std::sprintf(msg, "(%d, %d)   (double) %g", x, y, v(0,0));
    return;
   }
   case VIL_PIXEL_FORMAT_RGB_BYTE: {
    vil_image_view<vil_rgb<vxl_byte> > v = r->get_view(x,1,y,1);
    if (!v)
      std::sprintf(msg, "Pixel Not Available");
    else
      std::sprintf(msg, "(%d, %d)   (ubyte)[ R=%d,G=%d,B=%d]", x, y,
                  v(0,0).R(), v(0,0).G(),v(0,0).B() );
      return;
   }
   case VIL_PIXEL_FORMAT_RGB_UINT_16: {
    vil_image_view<vil_rgb<vxl_uint_16> > v = r->get_view(x,1,y,1);
    if (!v)
      std::sprintf(msg, "Pixel Not Available");
    else
      std::sprintf(msg, "(%d, %d)   (uint16)[ R=%d,G=%d,B=%d]", x, y,
                  v(0,0).R(), v(0,0).G(),v(0,0).B() );
    return;
   }
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
    std::sprintf(msg, "Pixel Not Available");
  }
}

double bgui_image_tableau::
get_pixel_value(const unsigned c, const unsigned r)
{
  vil_image_resource_sptr rs = this->get_image_resource();
  if (!rs)
    return 0;
  if (c>=rs->ni()||r>=rs->nj())
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
        else if (n_p==4)
          return static_cast<double>(v(0,0,0)+v(0,0,1)+v(0,0,2)+v(0,0,3))/4;
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

//: get the pixel value as color
std::vector<double> bgui_image_tableau::get_color_pixel_value(const unsigned c, const unsigned r)
{
  vil_image_resource_sptr rs = this->get_image_resource();
  if (!rs)
    return std::vector<double>(0);
  if (c>=rs->ni()||r>=rs->nj())
    return std::vector<double>(0);
  unsigned n_p = rs->nplanes();
  std::vector<double> val(n_p, 0.0);
  if (n_p==1)
  {
    val[0]=this->get_pixel_value(c, r);
    return val;
  }
  //note: will treat RGBA as a four plane type
  vil_pixel_format type = vil_pixel_format_component_format(rs->pixel_format());

  switch (type )
  {
    case  VIL_PIXEL_FORMAT_BYTE: {
      vil_image_view<vxl_byte> v = rs->get_view(c,1,r,1);
      if (!v)
        return val;
      for (unsigned p = 0; p<n_p; ++p)
        val[p]=static_cast<double>(v(0,0,p));
      return val;
    }
    case  VIL_PIXEL_FORMAT_UINT_16: {
      vil_image_view<vxl_uint_16> v = rs->get_view(c,1,r,1);
      if (!v)
        return val;
      for (unsigned p = 0; p<n_p; ++p)
            val[p]=static_cast<double>(v(0,0,p));
      return val;
    }
    default:
      return val;
  }
}

void bgui_image_tableau::image_line(const float col_start,
                                    const float row_start,
                                    const float col_end,
                                    const float row_end,
                                    std::vector<double>& line_pos,
                                    std::vector<double>& vals)
{
  line_pos.clear();vals.clear();
  //Get the image data
  // the line length in pixels
  float length = std::sqrt((col_end-col_start)*(col_end-col_start) +
                          (row_end-row_start)*(row_end-row_start));
  if (length == 0)
    return;
  //initialize the line scan parameters
  float xstep = (col_end-col_start)/length;
  float ystep = (row_end-row_start)/length;
  float sinc = std::sqrt(xstep*xstep + ystep*ystep);
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

//: Extract a line of pixel values return color if available
void bgui_image_tableau::image_line(const float col_start,
                                    const float row_start,
                                    const float col_end,
                                    const float row_end,
                                    std::vector<double>& line_pos,
                                    std::vector<std::vector<double> >& vals)
{
  line_pos.clear();vals.clear();
  //Get the image data
  // the line length in pixels
  float length = std::sqrt((col_end-col_start)*(col_end-col_start) +
                          (row_end-row_start)*(row_end-row_start));
  if (length == 0)
    return;
  //initialize the line scan parameters
  float xstep = (col_end-col_start)/length;
  float ystep = (row_end-row_start)/length;
  float sinc = std::sqrt(xstep*xstep + ystep*ystep);
  float spos = 0;
  line_pos.push_back(spos);
  unsigned c = static_cast<unsigned>(col_start),
   r = static_cast<unsigned>(row_start);
  std::vector<double> cv = get_color_pixel_value(c, r);
  unsigned n_bands = cv.size();
  vals.resize(n_bands);
  for (unsigned i = 0; i<n_bands; ++i)
    vals[i].push_back(cv[i]);

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
    cv = get_color_pixel_value(c, r);
    for (unsigned i = 0; i<n_bands; ++i)
      vals[i].push_back(cv[i]);
  }
}
void bgui_image_tableau::unset_mouse_message(){
  mouse_message_ = false;
  capture_mouse_ = false;
  tt_->clear();
  post_redraw();
}

//--------------------------------------------------------------------------------
//:
// Handle all events for this tableau.
bool bgui_image_tableau::handle(vgui_event const &e)
{
#if 0
  if(e.type != vgui_MOTION)
    std::cout << "bgui_image " << e << '\n' << std::flush;
#endif
  static bool button_down = false;
  if (e.type == vgui_DRAW)
  {
    bool handled = base::handle(e);

    if(tt_) tt_->handle(e);

    return handled;
  }

  if (e.type == vgui_BUTTON_DOWN)
  {
    button_down = true;
    if (handle_motion_)
      vgui::out << ' ' << std::endl;
    // if the mouse message is active
    // then store the pixel location of the mouse
    if(e.button == vgui_LEFT && capture_mouse_){
      vgui_projection_inspector p_insp;
      p_insp.window_to_image_coordinates(e.wx, e.wy, mouse_pos_[0],
                                         mouse_pos_[1]);
    }
  }
  else if (e.type == vgui_BUTTON_UP)
  {
    button_down = false;
    // if the mouse message is active
    // display a single line of text (mouse_message_text_)
    if(e.button == vgui_LEFT && mouse_message_){
      tt_->clear();
      tt_->set_colour(0.0f, 0.0f, 0.0f);
      tt_->set_size(12);
      //                                                        display banner
      //                                                            |
      //                                                            V
      tt_->add(mouse_pos_[0], mouse_pos_[1], mouse_message_text_);//need to modify text-tableau
      post_redraw();
    }
  }
  else if (e.type == vgui_MOTION && handle_motion_ && !button_down)
  {
    // Get X,Y position to display on status bar:
    float pointx, pointy;
    vgui_projection_inspector p_insp;
    p_insp.window_to_image_coordinates(e.wx, e.wy, pointx, pointy);
    int intx = (int)std::floor(pointx), inty = (int)std::floor(pointy);
    char msg[100];

    this->get_pixel_info_from_image(intx, inty,e, msg);

    // gets a snapshot of the viewport so that it restores it back after vgui::out
    // this is needed, because vgui::out changes viewport, and image tableau
    // gets confused
    bgui_image_tableau_vp_sc_snapshot snap;

    // Display on status bar:
    if (!locked_) {
      if (show_path_)
        vgui::out << msg << "  " << this->file_name() << std::endl;
      else
        vgui::out << msg << std::endl;
    }
  }
  return base::handle(e);
}

//--------------------------------------------------------------------------------
