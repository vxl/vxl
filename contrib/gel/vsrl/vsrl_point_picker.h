#ifndef vsrl_point_picker_h_
#define vsrl_point_picker_h_

#include "vsrl_point_picker_sptr.h"
#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_easy2D_tableau_sptr.h>
#include <vgl/vgl_point_2d.h>

class vsrl_point_picker : public vgui_wrapper_tableau
{
 public:
  vsrl_point_picker( vgui_tableau_sptr child);

  bool handle( vgui_event const &e);
  vgui_easy2D_tableau_sptr get_easy2D_pointer(vgui_tableau_sptr const& tab);
  vgui_image_tableau_sptr get_image_tab_pointer(vgui_tableau_sptr const& tab);
  vgl_point_2d<float> put_point(float x, float y);
  vgl_point_2d<float> put_point(vgl_point_2d<float> const& p) { return put_point(p.x(),p.y()); }
  vgl_point_2d<float> put_H_line(float x, float y);
  vgl_point_2d<float> put_H_line(vgl_point_2d<float> const& p) { return put_H_line(p.x(),p.y()); }
  inline vgl_point_2d<float> get_point() {return point_;}
  int get_value(float x, float y);
 private:
  vgl_point_2d<float> point_;
};

#endif // vsrl_point_picker_h_
