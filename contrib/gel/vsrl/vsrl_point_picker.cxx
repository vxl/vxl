//
// Code to help with picking points in an image.
//

#include "vsrl_point_picker.h"
#include <vgui/vgui.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_find.h>
#include <vgui/vgui_projection_inspector.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_pixel.h>
#include <vxl_config.h>

// Default ctor
vsrl_point_picker::vsrl_point_picker( vgui_tableau_sptr child)
  : vgui_wrapper_tableau( child)
  , point_(0.f,0.f)
{
}

bool vsrl_point_picker::handle( vgui_event const &e)
{
  // just display any time the mouse button is pressed

  vgui_modifier m = e.modifier;

  if (e.type== vgui_BUTTON_DOWN)
  {
    // undo the window transformation
    vgui_projection_inspector pi;
    float ix, iy;
    pi.window_to_image_coordinates( e.wx, e.wy, ix, iy);
    point_.set(ix,iy); // save the point that was picked.

    if ( (e.button== vgui_LEFT) && !m)
    {
      put_point(ix,iy);
      vgui::out << '(' << ix << ' ' << iy << ")\n";
    }
    else if (e.button== vgui_LEFT && (m & vgui_SHIFT))
    {
      put_H_line(ix,iy);
      vgui::out << '(' << ix << ' ' << iy << ")\n";
    }
    else if (e.button== vgui_LEFT && (m & vgui_ALT))
    {
      put_H_line(ix,iy);
      vgui::out << '(' << ix << ' ' << iy << ") ALT!\n";
    }
    else if (e.button== vgui_MIDDLE && !e.modifier)
    {
      int value = this->get_value(ix,iy);
      vgui::out << '(' << ix << ' ' << iy << ") I= " << value << "\n";
    }
    else
    {
      vgui::out << '(' << ix << ' ' << iy << ")\n";
    }
    this->post_redraw();
  }


  return child && child->handle( e);
}

vgui_easy2D_tableau_sptr
vsrl_point_picker::get_easy2D_pointer(vgui_tableau_sptr const& tab)
{
  vgui_easy2D_tableau_sptr e2d_ptr;
  if (tab)
    e2d_ptr.vertical_cast(vgui_find_below_by_type_name(tab,
                          vcl_string("vgui_easy2D_tableau")));
  return e2d_ptr;
}

vgui_image_tableau_sptr
vsrl_point_picker::get_image_tab_pointer(vgui_tableau_sptr const& tab)
{
  vgui_image_tableau_sptr img_tab_ptr;
  if (tab)
    img_tab_ptr.vertical_cast(vgui_find_below_by_type_name(tab,
                              vcl_string("vgui_image_tableau")));
  return img_tab_ptr;
}


vgl_point_2d<float> vsrl_point_picker::put_point(float x, float y)
{
  // Get the easy2D tableau pointers
  vgui_easy2D_tableau_sptr e2d = get_easy2D_pointer(this);

  // Draw the point
  e2d->add_point(x,y);

  this->post_redraw();

  // return the point in case it's needed elsewhere
  return point_;
}

vgl_point_2d<float> vsrl_point_picker::put_H_line(float x, float y)
{
  // Get the easy2D tableau pointers
  vgui_easy2D_tableau_sptr e2d = get_easy2D_pointer(this);
  vgui_image_tableau_sptr img_tab = get_image_tab_pointer(this);
  vil1_image img = img_tab->get_image();
  int xmin = 0;
  int xmax = img.cols()-1;

  // Draw the horizontal line
  e2d->add_line(xmin,y,xmax,y);

  point_.set(x,y);

  this->post_redraw();

  // return the point in case it's needed elsewhere
  return point_;
}

int vsrl_point_picker::get_value(float x, float y)
{
  vgui_image_tableau_sptr img_tab = get_image_tab_pointer(this);
  vil1_image img = img_tab->get_image();
  switch ( vil1_pixel_format(img) )
  {
   case VIL1_BYTE: { vxl_byte value=0;
    img.get_section(&value,int(x),int(y),1,1); return int(value); }
   case VIL1_UINT16: { vxl_uint_16 value=0;
    img.get_section(&value,int(x),int(y),1,1); return int(value); }
   case VIL1_UINT32: { vxl_uint_32 value=0;
    img.get_section(&value,int(x),int(y),1,1); return int(value); }
   default:
    vcl_cerr << "vsrl_point_picker::get_value() not implemented for this pixel type\n";
    return -1;
  }
}
