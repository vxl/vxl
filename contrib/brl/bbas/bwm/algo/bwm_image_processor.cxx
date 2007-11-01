#include "bwm_image_processor.h"

#include <vcl_cstdio.h>

#include <vgui/vgui_range_map_params.h>

#include <bgui/bgui_image_utils.h>
#include <vdgl/vdgl_digital_curve.h>
#include <sdet/sdet_detector.h>
#include <brip/brip_roi.h>

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
  if(!img) return;
  //Allow only grey scale for now
  if(img->get_image_resource()->nplanes()!=1)
    return;
  static double min = 0, max = 2048;
  if(img->get_image_resource()->pixel_format()==VIL_PIXEL_FORMAT_BYTE)
    max = 255;
  static double gamma = 1.0;
  static bool invert = false;
  bool gl_map = true;
  bool cache = true;
  vgui_dialog range_dlg("Set Range Map Params(Grey Scale Only!)");
  range_dlg.field("Range min:", min);
  range_dlg.field("Range max:", max);
  range_dlg.field("Gamma:", gamma);
  range_dlg.checkbox("Invert:", invert);
  if (!range_dlg.ask())
    return;

  vgui_range_map_params_sptr rmps= 
    new vgui_range_map_params(min, max, gamma, invert, gl_map, cache);

  img->set_mapping(rmps);
}

bool bwm_image_processor::
step_edges_vd(bgui_image_tableau_sptr const& img,
              vsol_box_2d_sptr const& roi,
              vcl_vector<vdgl_digital_curve_sptr>& edges)
{
  if(!img) return false;
  //Allow only grey scale for now
  if(img->get_image_resource()->nplanes()!=1)
    {
      vcl_cerr << "In bwm_observer_img::step_edges_vd() - step_edges only works on grey level images \n";
        return false;
    }

  static bool agr = true;
  static sdet_detector_params dp;
  static float nm = 2.0;

  vgui_dialog vd_dialog("Step Edges Params");
  vd_dialog.field("Gaussian sigma", dp.smooth);
  vd_dialog.field("Noise Threshold", nm);
  vd_dialog.checkbox("Automatic Threshold", dp.automatic_threshold);
  vd_dialog.checkbox("Agressive Closure", agr);
  vd_dialog.checkbox("Compute Junctions", dp.junctionp);
  if (!vd_dialog.ask())
    return false;
  dp.noise_multiplier=nm;
  if (agr)
    dp.aggressive_junction_closure=1;
  else
    dp.aggressive_junction_closure=0;
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
  if(!det.get_vdgl_edges(edges))
  {
    vcl_cerr << "In bwm_observer_img::step_edges_vd() - edge detection failed\n";
    return false;
  }
  return true;
}
