// This is brl/bbas/bgui/bgui_image_tableau.cxx
#include "bgui_image_tableau.h"
//:
// \file
// \author J.L. Mundy
// \brief  See bgui_image_tableau.h for a description of this file.

#include <vcl_cmath.h>
#include <vcl_cstdio.h> // sprintf
#include <vgui/vgui_event.h>
#include <vgui/vgui_gl.h>
#include <vil1/vil1_crop.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_rgba.h>
#include <vgui/vgui.h>
#include <vgui/vgui_projection_inspector.h>


//--------------------------------------------------------------------------------

bgui_image_tableau::bgui_image_tableau()
  { } 

bgui_image_tableau::bgui_image_tableau(vil_image_resource_sptr const & img,
                                       vgui_range_map_params_sptr const& rmp)
  : base(img, rmp) { }

bgui_image_tableau::bgui_image_tableau(vil_image_view_base const & img,
                                       vgui_range_map_params_sptr const& rmp)
  : base(img, rmp) { }

bgui_image_tableau::bgui_image_tableau(vil1_image const & img,
                                       vgui_range_map_params_sptr const& rmp)
  : base(img, rmp) { }

bgui_image_tableau::bgui_image_tableau(char const *f,
                                       vgui_range_map_params_sptr const& rmp)
  : base(f, rmp) { }

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
  if(!r)
    if(!this->get_image())
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
  if(x<0||x>=w||y<0||y>=h)
    {
      vcl_sprintf(msg, "(%d, %d)   ?", 0, 0);
      return;
    }
  vil_pixel_format type = r->pixel_format();
  unsigned nbytes = vil_pixel_format_sizeof_components(type);
  switch (type )
    {
    case VIL_PIXEL_FORMAT_BOOL:
      {
        vil_image_view<bool> v = r->get_view();
        if(!v)
          vcl_sprintf(msg, "Pixel Not Available");
        else
          vcl_sprintf(msg, "(%d, %d)   (bool) %d", x, y,
                      v(x,y));
        return;
      }
    case  VIL_PIXEL_FORMAT_BYTE:
      {
        if(n_p==1)
          {       
            vil_image_view<vxl_byte> v = r->get_view(x,1,y,1);
            if(!v)
          vcl_sprintf(msg, "Pixel Not Available");
            else
              vcl_sprintf(msg, "(%d, %d)   (ubyte) %d", x, y, v(0,0));
            return;
          }
        else if(n_p==3)
          {
            vil_image_view<vxl_byte > v = r->get_view(x,1,y,1);
            if(!v)
              vcl_sprintf(msg, "Pixel Not Available");
            else
              vcl_sprintf(msg, "(%d, %d)   (ubyte)[ R=%d,G=%d,B=%d]", x, y,
                          v(0,0,0), v(0,0,1), v(0,0,2) );
            return;
          }
        return;
      }
    case  VIL_PIXEL_FORMAT_SBYTE:
      {
        if(n_p==1)
          {       
            vil_image_view<vxl_sbyte> v = r->get_view(x,1,y,1);
            if(!v)
              vcl_sprintf(msg, "Pixel Not Available");
            else
              vcl_sprintf(msg, "(%d, %d)   (sbyte) %d", x, y, v(0,0));
            return;
          }
        else if(n_p==3)
          {
            vil_image_view<vil_rgb<vxl_sbyte> > v = r->get_view(x,1,y,1);
            if(!v)
              vcl_sprintf(msg, "Pixel Not Available");
            else
              vcl_sprintf(msg, "(%d, %d)   (sbyte)[ R=%d,G=%d,B=%d]", x, y,
                          v(0,0).R(), v(0,0).G(),v(0,0).B() );
            return;
          }
        return;
      }
    case  VIL_PIXEL_FORMAT_UINT_16:
      {
        if(n_p==1)
          {       
            vil_image_view<vxl_uint_16> v = r->get_view(x,1,y,1);
            if(!v)
              vcl_sprintf(msg, "Pixel Not Available");
            else
              vcl_sprintf(msg, "(%d, %d)   (uint16) %d", x, y, v(0,0));
            return;
          }
        else if(n_p==3)
          {
            vil_image_view<vil_rgb<vxl_uint_16> > v = r->get_view(x,1,y,1);
            if(!v)
              vcl_sprintf(msg, "Pixel Not Available");
            else
              vcl_sprintf(msg, "(%d, %d)   (uint16)[ R=%d,G=%d,B=%d]", x, y,
                          v(0,0).R(), v(0,0).G(),v(0,0).B() );
            return;
          }
        return;
      }
    case  VIL_PIXEL_FORMAT_INT_16:
      {
        if(n_p==1)
          {       
            vil_image_view<vxl_int_16> v = r->get_view(x,1,y,1);
            if(!v)
              vcl_sprintf(msg, "Pixel Not Available");
            else
              vcl_sprintf(msg, "(%d, %d)   (int16) %d", x, y, v(0,0));
            return;
          }
        else if(n_p==3)
          {
            vil_image_view<vil_rgb<vxl_int_16> > v = r->get_view(x,1,y,1);
            if(!v)
              vcl_sprintf(msg, "Pixel Not Available");
            else
              vcl_sprintf(msg, "(%d, %d)   (int16)[ R=%d,G=%d,B=%d]", x, y,
                          v(0,0).R(), v(0,0).G(),v(0,0).B() );
            return;
          }
        return;
      }
#if 0
    case VIL_PIXEL_FORMAT_UINT_32:
      {
      }
    case  VIL_PIXEL_FORMAT_INT_32:
      {
      }
#endif 
    case  VIL_PIXEL_FORMAT_FLOAT:
      {
        vil_image_view<float> v = r->get_view(x,1,y,1);
        if(!v)
          vcl_sprintf(msg, "Pixel Not Available");
        else
          vcl_sprintf(msg, "(%d, %d)   (float) %f", x, y,
                      v(0,0));
		return;
      }
    case  VIL_PIXEL_FORMAT_DOUBLE:
      {
        vil_image_view<double> v = r->get_view(x,1,y,1);
        if(!v)
          vcl_sprintf(msg, "Pixel Not Available");
        else
          vcl_sprintf(msg, "(%d, %d)   (double) %g", x, y,
                      v(0,0));
		return;
      }
    case VIL_PIXEL_FORMAT_RGB_BYTE:
      {
        vil_image_view<vil_rgb<vxl_byte> > v = r->get_view(x,1,y,1);
        if(!v)
          vcl_sprintf(msg, "Pixel Not Available");
        else
          vcl_sprintf(msg, "(%d, %d)   (ubyte)[ R=%d,G=%d,B=%d]", x, y,
                      v(0,0).R(), v(0,0).G(),v(0,0).B() );
		return;
      }
#if 0
    case VIL_PIXEL_FORMAT_RGB_SBYTE:
      {
      }
#endif
    case VIL_PIXEL_FORMAT_RGB_UINT_16:
      {
        vil_image_view<vil_rgb<vxl_uint_16> > v = r->get_view(x,1,y,1);
        if(!v)
          vcl_sprintf(msg, "Pixel Not Available");
        else
          vcl_sprintf(msg, "(%d, %d)   (ubyte)[ R=%d,G=%d,B=%d]", x, y,
                      v(0,0).R(), v(0,0).G(),v(0,0).B() );
		return;
      }
#if 0
    case VIL_PIXEL_FORMAT_RGB_INT_16:
      {
      }
    case VIL_PIXEL_FORMAT_RGB_UINT_32:
      {
      }
    case VIL_PIXEL_FORMAT_RGB_INT_32:
      {
      }
    case VIL_PIXEL_FORMAT_RGB_FLOAT:
      {
      }
    case VIL_PIXEL_FORMAT_RGB_DOUBLE:
      {
      }
    case VIL_PIXEL_FORMAT_RGBA_UINT_32:
      {
      }
    case VIL_PIXEL_FORMAT_RGBA_INT_32:
      {
      }
    case VIL_PIXEL_FORMAT_RGBA_UINT_16:
      {
      }
    case VIL_PIXEL_FORMAT_RGBA_INT_16:
      {
      }
    case VIL_PIXEL_FORMAT_RGBA_BYTE:
      {
      }
    case VIL_PIXEL_FORMAT_RGBA_SBYTE:
      {
      }
    case VIL_PIXEL_FORMAT_RGBA_FLOAT:
      {
      }
    case VIL_PIXEL_FORMAT_RGBA_DOUBLE:
      {
      }
    case VIL_PIXEL_FORMAT_COMPLEX_FLOAT:
      {
      }
    case VIL_PIXEL_FORMAT_COMPLEX_DOUBLE:
      {
      }
#endif
    default:
      vcl_sprintf(msg, "Pixel Not Available");
    }
}


//--------------------------------------------------------------------------------
//:
// Handle all events for this tableau.
bool bgui_image_tableau::handle(vgui_event const &e)
{
  static bool button_down = false;
  if (e.type == vgui_BUTTON_DOWN)
    {
      button_down = true;
      vgui::out << ' ' << vcl_endl;
    }
  else if (e.type == vgui_BUTTON_UP)
    {
      button_down = false;
    }
  else if (e.type == vgui_MOTION && button_down == false)
    {
      // Get X,Y position to display on status bar:
      float pointx, pointy;
      vgui_projection_inspector p_insp;
      p_insp.window_to_image_coordinates(e.wx, e.wy, pointx, pointy);
      int intx = (int)vcl_floor(pointx), inty = (int)vcl_floor(pointy);
      char msg[100];

      this->get_pixel_info_from_image(intx, inty,e, msg);

      // Display on status bar:
      vgui::out << msg << vcl_endl;
    }

  return base::handle(e);
}

//--------------------------------------------------------------------------------
