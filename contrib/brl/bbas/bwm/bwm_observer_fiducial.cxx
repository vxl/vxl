#include <ios>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "bwm_observer_fiducial.h"
//:
// \file
#include <bwm/bwm_observer_mgr.h>
#include <bwm/algo/bwm_algo.h>
#include <bwm/algo/bwm_image_processor.h>
#include <bwm/bwm_tableau_mgr.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vgui/vgui.h>
#include <vnl/vnl_math.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_dialog_extensions.h>
#include <vgui/vgui_projection_inspector.h>
#include <bgui/bgui_vsol_soview2D.h>
#include "bwm_observer_fiducial.h"
#include <vsl/vsl_basic_xml_element.h>
#include "bwm_utils.h"

bwm_observer_fiducial::bwm_observer_fiducial(bgui_image_tableau_sptr const& img,
                                             std::string const& name,
                                             std::string const& image_path,
                                             std::string const& fid_path,
                                             bool display_image_path): bwm_observer_vgui(img), fiducial_path_(fid_path)
{
  img->show_image_path(display_image_path);
  // LOAD IMAGE
  vgui_range_map_params_sptr params;
  std::string& non_const_path = const_cast<std::string&>(image_path);//load image path should be const..
  vil_image_resource_sptr img_res = bwm_utils::load_image(non_const_path, params);
  if (!img_res) {
    bwm_utils::show_error("Image [" + image_path + "] is NOT found");
    return;
  }
  img->set_image_resource(img_res, params);
  img->set_file_name(image_path);
  std::string temp = name;
  if(temp == "")
    temp = fid_path;
  set_tab_name(temp);
  bwm_observer_mgr::instance()->add(this);
}

bool bwm_observer_fiducial::handle(const vgui_event &e){
  return base::handle(e);
}
