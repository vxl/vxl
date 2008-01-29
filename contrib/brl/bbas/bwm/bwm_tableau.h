#ifndef bwm_tableau_h_
#define bwm_tableau_h_

#include <vbl/vbl_ref_count.h>
//#include <vgui/vgui_tableau.h>
#include <vcl_string.h>

class bwm_tableau : public vbl_ref_count
{
public:
  bwm_tableau(){}

  ~bwm_tableau(){}

  virtual vcl_string type_name() const { return "bwm_tableau_img"; }
};

#endif