//
// Code to help with picking points in an image.
//

#include "vsrl_point_picker.h"
#include <vcl_iostream.h>
#include <vgui/vgui.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_find.h>
#include <vgui/vgui_projection_inspector.h>
#include <vil/vil_image.h>

// Default ctor
vsrl_point_picker::vsrl_point_picker( vgui_tableau_sptr child)
  : vgui_wrapper_tableau( child)
{
  point_ = new vnl_vector<float>(2);
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

      if ( (e.button== vgui_LEFT) && !m) 
        {
          put_point(ix,iy);
          vgui::out << "(" << ix << " " << iy << ")" << vcl_endl;
        }
      else if (e.button== vgui_LEFT && (m & vgui_SHIFT)) 
        {
          put_H_line(ix,iy);
          vgui::out << "(" << ix << " " << iy << ")" << vcl_endl;
        }
      else if (e.button== vgui_LEFT && (m & vgui_ALT)) 
        {
          put_H_line(ix,iy);
          vgui::out << "(" << ix << " " << iy << ") ALT!" << vcl_endl;
        }
      else if (e.button== vgui_MIDDLE && !e.modifier) 
        {
          vgui::out << "(" << ix << " " << iy << ")" << vcl_endl;
        }
      else 
        {
          vgui::out << "(" << ix << " " << iy << ")" << vcl_endl;
        }
      this->post_redraw();
    }


  return child && child->handle( e);
}

vgui_easy2D_tableau_sptr
vsrl_point_picker::get_easy2D_pointer(vgui_tableau_sptr const& tab)
{
  vgui_easy2D_tableau_sptr e2d_ptr;
  if(tab)
    e2d_ptr.vertical_cast(vgui_find_below_by_type_name(tab,
                                                       vcl_string("vgui_easy2D_tableau")));
  return e2d_ptr;
}

vgui_image_tableau_sptr
vsrl_point_picker::get_image_tab_pointer(vgui_tableau_sptr const& tab)
{
  vgui_image_tableau_sptr img_tab_ptr;
  if(tab)
    img_tab_ptr.vertical_cast(vgui_find_below_by_type_name(tab,
                                                       vcl_string("vgui_image_tableau")));
  return img_tab_ptr;
}


vnl_vector<float>*  vsrl_point_picker::put_point(float x, float y)
{
  // Get the easy2D tableau pointers
  vgui_easy2D_tableau_sptr e2d = get_easy2D_pointer(this);
  
  // Draw the point
  e2d->add_point(x,y);

  point_->x()=x;
  point_->y()=y;

  this->post_redraw();

  // return the point in case it's needed elsewhere
  return point_;
}

vnl_vector<float>*  vsrl_point_picker::put_H_line(float x, float y)
{
  // Get the easy2D tableau pointers
  vgui_easy2D_tableau_sptr e2d = get_easy2D_pointer(this);
  vgui_image_tableau_sptr img_tab = get_image_tab_pointer(this);
  vil_image img = img_tab->get_image();
  int xmin = 0;
  int xmax = img.cols()-1;
  
  // Draw the line
  e2d->add_line(xmin,y,xmax,y);

  point_->x()=x;
  point_->y()=y;

  this->post_redraw();

  // return the point in case it's needed elsewhere
  return point_;
}
