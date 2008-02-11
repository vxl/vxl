#ifndef bwm_tableau_h_
#define bwm_tableau_h_

#include <vbl/vbl_ref_count.h>
#include <vgui/vgui_viewer2D_tableau_sptr.h>
#include <vcl_string.h>

class bwm_tableau : public vbl_ref_count
{
public:
  bwm_tableau(){}

  ~bwm_tableau(){}

  virtual vcl_string type_name() const { return "bwm_tableau"; }

  virtual void set_viewer(vgui_viewer2D_tableau_sptr)=0;

  void set_grid_location(unsigned row, unsigned col) { row_=row; col_=col; }

protected:
  unsigned row_, col_;
};

#endif