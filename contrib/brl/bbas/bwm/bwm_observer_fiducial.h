#ifndef bwm_observer_fiducial_h_
#define bwm_observer_fiducial_h_
//:
// \file

#include <iostream>
#include "bwm_observer_vgui.h"
#include "bwm_observer_img.h"
#include "bwm_observable_sptr.h"
#include <vgui/vgui_style.h>
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
#include <vgui/vgui_event.h>
#include "algo/bwm_soview2D_cross.h"
#include "io/bwm_io_config_parser.h"
struct fiducial_corr {
  fiducial_corr() :soview_(nullptr), loc_(vgl_point_2d<double>(0.0, 0.0)) {}
  vgl_point_2d<double> loc_;
  bwm_soview2D_cross* soview_;
  static float cross_radius_;
};
class bwm_observer_fiducial : public bwm_observer_vgui
{
 public:

  typedef bwm_observer_vgui base;

 bwm_observer_fiducial(bgui_image_tableau_sptr const& img,
                       std::string const& site_name,
                       std::string const& image_path,
                       std::string const& fid_path,
                       bool display_image_path);

 virtual ~bwm_observer_fiducial();

  bgui_image_tableau_sptr image_tableau() { return img_tab_; }

  bool handle(const vgui_event &e);

  virtual std::string type_name() const { return "bwm_observer_fiducial"; }

  bool save_fiducial_corrs(std::string path);
  bool read_fiducial_corrs();
  bool add_fiducial_corrs(bwm_io_config_parser* parser);
  //: unused pure virtual functions ===========
  virtual void proj_poly(vsol_polygon_3d_sptr poly_3d,vsol_polygon_2d_sptr& poly_2d){}
  virtual void proj_line(vsol_line_3d_sptr l3d, vsol_line_2d_sptr& l2d){}
  virtual void proj_point(vgl_point_3d<double> p3d, vgl_point_2d<double>& p2d){}
  virtual void backproj_poly(vsol_polygon_2d_sptr poly2d,
                             vsol_polygon_3d_sptr& poly3d){}
  //=================
 protected:
  //: add a commited fiducial correspondence
  void commit_pending();

  //: removes a committed fiducial correspondence
  void remove_sel_committed();

  //: update a pending fiducial correspondence
  void update_pending(vgl_point_2d<double> const& pt);
  std::string mode_;
  std::string type_;
  fiducial_corr pending_corr_;
  std::vector<fiducial_corr> committed_fid_corrs_;
  std::string site_name_;
  std::string composite_image_path_;
  std::string fiducial_path_;
  vgui_style_sptr COMMITED_STYLE;
};

#endif
