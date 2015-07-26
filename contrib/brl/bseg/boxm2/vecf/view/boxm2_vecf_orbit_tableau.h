// This is brl/bseg/boxm2/vecf/view/boxm2_vecf_orbit_tableau.h
#ifndef boxm2_vecf_orbit_tableau_h
#define boxm2_vecf_orbit_tableau_h
//:
// \file
// \brief A tableau to view projections of the orbit model 
// \author J.L. Mundy
// \date July 21, 2015
#include <vnl/vnl_math.h>
#include <vgui/vgui_wrapper_tableau.h>
#include <bgui/bgui_vsol2D_tableau.h>
#include <vecf/boxm2_vecf_orbit_params.h>
#include <vecf/boxm2_vecf_eyelid.h>
#include <vecf/boxm2_vecf_eyelid_crease.h>
#include <bgui/bgui_image_tableau.h>
class boxm2_vecf_orbit_tableau : public vgui_wrapper_tableau
{
 public:
  boxm2_vecf_orbit_tableau(){this->init();}
  virtual ~boxm2_vecf_orbit_tableau() {}
  void init();
  void set_params(vcl_string const& param_path, bool is_right);
  bool set_image(vcl_string const& image_path);
  void draw_orbit(bool is_right);
 private:
  bgui_image_tableau_sptr img_tab_;
  bgui_vsol2D_tableau_sptr vsol_tab_;
  boxm2_vecf_orbit_params left_params_;
  boxm2_vecf_orbit_params right_params_;
};

//: declare smart pointer
typedef vgui_tableau_sptr_t<boxm2_vecf_orbit_tableau> boxm2_vecf_orbit_tableau_sptr;

//: Create a smart-pointer to a boxm2_vecf_orbit_tableau tableau.
struct boxm2_vecf_orbit_tableau_new : public boxm2_vecf_orbit_tableau_sptr
{
  //: Constructor - create an empty vgui_easy3D_tableau.
  typedef boxm2_vecf_orbit_tableau_sptr base;
  boxm2_vecf_orbit_tableau_new() : base( new boxm2_vecf_orbit_tableau ) { }
};
#endif // boxm2_vecf_orbit_tablea
