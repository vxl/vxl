#ifndef bwm_observer_fiducial_h_
#define bwm_observer_fiducial_h_
//:
// \file

#include <iostream>
#include "bwm_observer_vgui.h"
#include "bwm_observer_img.h"
#include "bwm_observable_sptr.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vnl/vnl_math.h>


class bwm_observer_fiducial : public bwm_observer_vgui
{
 public:

  typedef bwm_observer_img base;

 bwm_observer_fiducial(bgui_image_tableau_sptr const& img,
                       std::string const& name,
                       std::string const& image_path,
                       std::string const& fid_path,
                       bool display_image_path);

  virtual ~bwm_observer_fiducial() {}

  bgui_image_tableau_sptr image_tableau() { return img_tab_; }

  bool handle(const vgui_event &e);

  virtual std::string type_name() const { return "bwm_observer_fiducial"; }
  virtual void proj_poly(vsol_polygon_3d_sptr poly_3d,vsol_polygon_2d_sptr& poly_2d){}
  virtual void proj_line(vsol_line_3d_sptr l3d, vsol_line_2d_sptr& l2d){}
  virtual void proj_point(vgl_point_3d<double> p3d, vgl_point_2d<double>& p2d){}
  virtual void bwm_observer_vgui::backproj_poly(vsol_polygon_2d_sptr poly2d, vsol_polygon_3d_sptr& poly3d) {}
 protected:
  std::string fiducial_path_;

};

#endif
