#ifndef example_point_selector_h_
#define example_point_selector_h_

#include "vsrl_point_picker_sptr.h"
#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_easy2D_tableau_sptr.h>
#include <vnl/vnl_vector.h>

class vsrl_point_picker : public vgui_wrapper_tableau
{
 public:

  vsrl_point_picker( vgui_tableau_sptr child);

  bool handle( vgui_event const &e);
  vgui_easy2D_tableau_sptr get_easy2D_pointer(vgui_tableau_sptr const& tab);
  vgui_image_tableau_sptr get_image_tab_pointer(vgui_tableau_sptr const& tab);
  vnl_vector<float>* put_point(float x, float y);
  vnl_vector<float>* put_H_line(float x, float y);
  vnl_vector<float>* get_point() {return point_;}
  vnl_vector<float>* point_;

};

#endif // example_point_selector_h_
