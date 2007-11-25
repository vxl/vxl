#include "bwm_image_processor.h"

#include <vcl_cstdio.h>

#include <vgui/vgui_range_map_params.h>

#include <bgui/bgui_image_utils.h>
#include <vdgl/vdgl_digital_curve.h>
#include <sdet/sdet_detector.h>
#include <sdet/sdet_fit_lines_params.h>
#include <sdet/sdet_fit_lines.h>
#include <brip/brip_roi.h>
#include <vtol/vtol_edge_2d_sptr.h>

void bwm_image_processor::hist_plot(bgui_image_tableau_sptr img)
{
  if (!img)
    {
      vcl_cout << "intensity_histogram() - no image tableau\n";
      return;
    }

  vil_image_resource_sptr res = img->get_image_resource();
  if (!res)
    {
      vcl_cout << "intensity_histogram() - the tableau does not have an image resource\n";
      return;
    }

  bgui_image_utils iu(res);
  bgui_graph_tableau_sptr g = iu.hist_graph();

  if (!g)
    { vcl_cout << "In intensity_histogram()- color images not supported\n";
    return;
    }

  //popup a profile graph
  char location[100];
  vcl_sprintf(location, "Intensity Histogram");
  vgui_dialog* ip_dialog = g->popup_graph(location);
  if (!ip_dialog->ask())
    {
      delete ip_dialog;
      return;
    }
  delete ip_dialog;
}

void bwm_image_processor::intensity_profile(bgui_image_tableau_sptr img,
                                            float start_col, float end_col, 
                                            float start_row, float end_row)
{
  if (img) {
    vcl_vector<double> pos, vals;
    img->image_line(start_col, start_row, end_col, end_row, pos, vals);
    bgui_graph_tableau_sptr g = bgui_graph_tableau_new(512, 512);
    g->update(pos, vals);

    //popup a profile graph
    char location[100];
    vcl_sprintf(location, "scan:(%d, %d)<->(%d, %d)",
                static_cast<unsigned>(start_col),
                static_cast<unsigned>(start_row),
                static_cast<unsigned>(end_col),
                static_cast<unsigned>(end_row));
    vgui_dialog* ip_dialog = g->popup_graph(location);
    if (!ip_dialog->ask()){
      delete ip_dialog;
      return;
    }
    delete ip_dialog;
  }
}


void bwm_image_processor::range_map(bgui_image_tableau_sptr img)
{
  vgui_range_map_params_sptr rmp = img->map_params();
  if(!rmp)
    {
      bgui_image_utils biu(img->get_image_resource());
      biu.default_range_map(rmp);
      return;
    }
  vgui_range_map_params_sptr new_rmp = new vgui_range_map_params(*rmp);
  unsigned nc = rmp->n_components_;
  // use this array because vgui_dialog does not support long double fields
  double ranges[8];
  int choice=new_rmp->band_map_;
  vgui_dialog rmp_dialog("Set Range Mapping Parameters");
  if (nc == 1) {
    ranges[0] = new_rmp->min_L_;
    ranges[1] = new_rmp->max_L_;
    rmp_dialog.field("Luminance Min ",ranges[0]);
    rmp_dialog.field("Luminance Max ",ranges[1]);
    rmp_dialog.field("Luminance Gamma ",new_rmp->gamma_L_);
  }
  if (nc == 3 || nc == 4) {
    ranges[0] = new_rmp->min_R_;
    ranges[1] = new_rmp->max_R_;
    rmp_dialog.field("R Min ",ranges[0]);
    rmp_dialog.field("R Max ",ranges[1]);
    rmp_dialog.field("R Gamma ",new_rmp->gamma_R_);
    ranges[2] = new_rmp->min_G_;
    ranges[3] = new_rmp->max_G_;
    rmp_dialog.field("G Min ",ranges[2]);
    rmp_dialog.field("G Max ",ranges[3]);
    rmp_dialog.field("G Gamma ",new_rmp->gamma_G_);
    ranges[4] = new_rmp->min_B_;
    ranges[5] = new_rmp->max_B_;
    rmp_dialog.field("B Min ",ranges[4]);
    rmp_dialog.field("B Max ",ranges[5]);
    rmp_dialog.field("B Gamma ",new_rmp->gamma_B_);
  }
  if (nc==4) {
    vcl_vector<vcl_string> choices;
    for(unsigned c = 0; c<vgui_range_map_params::END_m; ++c)
    choices.push_back(vgui_range_map_params::bmap[c]);
    rmp_dialog.choice("Band Map", choices, choice);
    ranges[6] = new_rmp->min_X_;
    ranges[7] = new_rmp->max_X_;
    rmp_dialog.field("X Min ",ranges[6]);
    rmp_dialog.field("X Max ",ranges[7]);
    rmp_dialog.field("X Gamma ",new_rmp->gamma_X_);
  }
  rmp_dialog.checkbox("Invert ",new_rmp->invert_);

  rmp_dialog.checkbox("Use glPixelMap ",new_rmp->use_glPixelMap_);
  rmp_dialog.checkbox("Cache Map ",new_rmp->cache_mapped_pix_);

  if (!rmp_dialog.ask())
    return;
  new_rmp->band_map_ = choice;
  new_rmp->min_L_ = ranges[0];
  new_rmp->max_L_ = ranges[1];
  new_rmp->min_R_ = ranges[0];
  new_rmp->max_R_ = ranges[1];
  new_rmp->min_G_ = ranges[2];
  new_rmp->max_G_ = ranges[3];
  new_rmp->min_B_ = ranges[4];
  new_rmp->max_B_ = ranges[5];
  new_rmp->min_X_ = ranges[6];
  new_rmp->max_X_ = ranges[7];

  img->set_mapping(new_rmp);
}

bool bwm_image_processor::
step_edges_vd(bgui_image_tableau_sptr const& img,
              vsol_box_2d_sptr const& roi,
              vcl_vector<vsol_digital_curve_2d_sptr>& edges)
{
  if(!img) return false;
  static sdet_detector_params dp;
  static float nm = 2.0;
  vgui_dialog vd_dialog("Step Edges Params");
  vd_dialog.field("Gaussian sigma (vd)", dp.smooth);
  vd_dialog.field("Noise Threshold", nm);

  if (!vd_dialog.ask())
    return false;

  dp.noise_multiplier=nm;
  dp.aggressive_junction_closure=1;
  dp.borderp = false;
  vil_image_resource_sptr image = img->get_image_resource();
  if (!image||!image->ni()||!image->nj())
    {
      vcl_cerr << "In bwm_observer_img::step_edges_vd() - no image\n";
      return false;
    }
  sdet_detector det(dp);
  brip_roi broi(image->ni(), image->nj());
  broi.add_region(roi);
  
  det.SetImage(image, broi);

  det.DoContour();
  if(!det.get_vsol_edges(edges))
    {
      vcl_cerr << "In bwm_observer_img::step_edges_vd() - edge detection failed\n";
      return false;
    }
  return true;
}

bool bwm_image_processor::lines_vd(bgui_image_tableau_sptr const& img,
                                   vsol_box_2d_sptr const& roi,
                                   vcl_vector<vsol_line_2d_sptr>& lines)
{
  if(!img) return false;

  static sdet_detector_params dp;
  static float nm = 2.0;

  static sdet_fit_lines_params flp;

  vgui_dialog lf_dialog("Detect Lines");
  lf_dialog.field("Gaussian sigma", dp.smooth);
  lf_dialog.field("Noise Threshold", nm);
  lf_dialog.field("Min Fit Length", flp.min_fit_length_);
  lf_dialog.field("RMS Distance", flp.rms_distance_);

  if (!lf_dialog.ask())
    return false;
  dp.noise_multiplier=nm;
  dp.aggressive_junction_closure=1;
  dp.borderp = false;
  sdet_detector det(dp);

  vil_image_resource_sptr image = img->get_image_resource();
  if (!image||!image->ni()||!image->nj())
    {
      vcl_cerr << "In bwm_image_processor::lines_vd() - no image\n";
      return false;
    }
  brip_roi broi(image->ni(), image->nj());
  broi.add_region(roi);

  det.SetImage(image, broi);

  det.DoContour();

  vcl_vector<vtol_edge_2d_sptr>* edges = det.GetEdges();
  if (!edges)
    {
      vcl_cout << "bwm_image_processor::lines_vd() - no edges to fit lines\n";
      return false;
    }
  sdet_fit_lines fl(flp);
  fl.set_edges(*edges);
  if(!fl.fit_lines())
    {
      vcl_cout << "bwm_image_processor::lines_vd() - lit fitting failed\n";
      return false;
    }
  fl.get_line_segs(lines);
  return true;
}
