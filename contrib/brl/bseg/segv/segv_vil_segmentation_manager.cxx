// This is brl/bseg/segv/segv_vil_segmentation_manager.cxx
#include "segv_vil_segmentation_manager.h"
//:
// \file
// \author J.L. Mundy

#include <vcl_cstdlib.h> // for vcl_exit()
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vnl/vnl_matlab_read.h>
#include <vbl/vbl_array_2d.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_decimate.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
#if 0
#ifdef HAS_XERCES
#include <bxml/bxml_vtol_io.h>
#endif
#endif
#include <sdet/sdet_detector_params.h>
#include <sdet/sdet_detector.h>
#include <sdet/sdet_harris_detector_params.h>
#include <sdet/sdet_harris_detector.h>
#include <sdet/sdet_fit_lines_params.h>
#include <sdet/sdet_fit_lines.h>
#include <sdet/sdet_grid_finder_params.h>
#include <sdet/sdet_grid_finder.h>
#include <vgui/vgui.h>
#include <vgui/vgui_find.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_style_sptr.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_soview2D.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_rubberband_tableau.h>
#include <vgui/vgui_range_map_params.h>
#include <bgui/bgui_image_tableau.h>
#include <bgui/bgui_vtol2D_tableau.h>
#include <bgui/bgui_vtol2D_rubberband_client.h>
#include <bgui/bgui_picker_tableau.h>
#include <bgui/bgui_range_adjuster_tableau.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_curve_2d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_vertex.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_one_chain_sptr.h>
#include <vtol/vtol_one_chain.h>
#include <vtol/vtol_intensity_face.h>
#include <bsol/bsol_algs.h>
#include <brip/brip_vil1_float_ops.h>
#include <brip/brip_para_cvrg_params.h>
#include <brip/brip_para_cvrg.h>
#include <brip/brip_watershed_params.h>
#include <sdet/sdet_watershed_region_proc_params.h>
#include <sdet/sdet_watershed_region_proc.h>
#include <sdet/sdet_vehicle_finder_params.h>
#include <sdet/sdet_vehicle_finder.h>
#include <bsol/bsol_hough_line_index.h>
#include <sdet/sdet_region_proc_params.h>
#include <sdet/sdet_region_proc.h>
#include <strk/strk_region_info_params.h>
#include <strk/strk_region_info.h>
#include <strk/strk_io.h>

segv_vil_segmentation_manager *segv_vil_segmentation_manager::instance_ = 0;

segv_vil_segmentation_manager *segv_vil_segmentation_manager::instance()
{
  if (!instance_)
  {
    instance_ = new segv_vil_segmentation_manager();
    instance_->init();
  }
  return segv_vil_segmentation_manager::instance_;
}

//-----------------------------------------------------------
// constructors/destructor
//
segv_vil_segmentation_manager::segv_vil_segmentation_manager():vgui_wrapper_tableau()
{
  first_ = true;
}

segv_vil_segmentation_manager::~segv_vil_segmentation_manager()
{
}

//: Set up the tableaux
void segv_vil_segmentation_manager::init()
{
  bgui_image_tableau_sptr itab = bgui_image_tableau_new();
  bgui_vtol2D_tableau_sptr t2D = bgui_vtol2D_tableau_new(itab);
  bgui_vtol2D_rubberband_client* rcl =  new bgui_vtol2D_rubberband_client(t2D);
  vgui_rubberband_tableau_sptr rubber = vgui_rubberband_tableau_new(rcl);
  vgui_composite_tableau_new comp(t2D,rubber);
  bgui_picker_tableau_sptr picktab = bgui_picker_tableau_new(comp);
  vgui_viewer2D_tableau_sptr v2D = vgui_viewer2D_tableau_new(picktab);
  grid_ = vgui_grid_tableau_new(1,1);
  grid_->set_grid_size_changeable(true);
  grid_->add_at(v2D, 0, 0);
  vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(grid_);
  this->add_child(shell);
  first_ = true;
}

//: set the image at the currently selected grid cell
void segv_vil_segmentation_manager::set_selected_grid_image(vil_image_resource_sptr const& image)
{
  bgui_image_tableau_sptr itab = this->selected_image_tab();
  if (!itab)
    this->add_image(image);
  else
    itab->set_image_resource(image);
  itab->post_redraw();
}

//: Add an image to the currently selected grid cell
void segv_vil_segmentation_manager::add_image(vil_image_resource_sptr const& image)
{
  bgui_image_tableau_sptr itab = bgui_image_tableau_new(image);
  bgui_vtol2D_tableau_sptr t2D = bgui_vtol2D_tableau_new(itab);
  bgui_vtol2D_rubberband_client* rcl =  new bgui_vtol2D_rubberband_client(t2D);
  vgui_rubberband_tableau_sptr rubber = vgui_rubberband_tableau_new(rcl);
  vgui_composite_tableau_new comp(t2D,rubber);
  bgui_picker_tableau_sptr picktab = bgui_picker_tableau_new(comp);
  vgui_viewer2D_tableau_sptr v2D = vgui_viewer2D_tableau_new(picktab);
  unsigned row=0, col=0;
  grid_->get_last_selected_position(&col, &row);
  grid_->add_at(v2D, col, row);
  itab->post_redraw();
}

//: Get the image tableau for the currently selected grid cell
bgui_image_tableau_sptr segv_vil_segmentation_manager::selected_image_tab()
{
  unsigned row=0, col=0;
  grid_->get_last_selected_position(&col, &row);
  vgui_tableau_sptr top_tab = grid_->get_tableau_at(col, row);
  if (top_tab)
  {
    bgui_image_tableau_sptr itab;
    itab.vertical_cast(vgui_find_below_by_type_name(top_tab,
                                                    vcl_string("vgui_image_tableau")));
    if (itab)
      return itab;
  }
  vcl_cout << "Unable to get bgui_image_tableau at (" << col
           << ", " << row << ")\n";
  return bgui_image_tableau_sptr();
}

//: Get the vtol2D tableau for the currently selected grid cell
bgui_vtol2D_tableau_sptr segv_vil_segmentation_manager::selected_vtol2D_tab()
{
  unsigned row=0, col=0;
  grid_->get_last_selected_position(&col, &row);
  vgui_tableau_sptr top_tab = grid_->get_tableau_at(col, row);
  if (top_tab)
  {
    bgui_vtol2D_tableau_sptr v2D;
    v2D.vertical_cast(vgui_find_below_by_type_name(top_tab,
                                                   vcl_string("bgui_vtol2D_tableau")));
    if (v2D)
      return v2D;
  }
  vcl_cout << "Unable to get bgui_vtol2D_tableau at (" << col
           << ", " << row << ")\n";
  return bgui_vtol2D_tableau_sptr();
}

//: Get the picker tableau for the currently selected grid cell
bgui_picker_tableau_sptr segv_vil_segmentation_manager::selected_picker_tab()
{
  unsigned row=0, col=0;
  grid_->get_last_selected_position(&col, &row);
  vgui_tableau_sptr top_tab = grid_->get_tableau_at(col, row);
  if (top_tab)
  {
    bgui_picker_tableau_sptr pick;
    pick.vertical_cast(vgui_find_below_by_type_name(top_tab,
                                                    vcl_string("bgui_picker_tableau")));
    if (pick)
      return pick;
  }
  vcl_cout << "Unable to get bgui_picker_tableau at (" << col
           << ", " << row << ")\n";
  return bgui_picker_tableau_sptr();
}

//: Get the rubberband tableau at the selected grid cell
vgui_rubberband_tableau_sptr segv_vil_segmentation_manager::selected_rubber_tab()
{
  unsigned row=0, col=0;
  grid_->get_last_selected_position(&col, &row);
  vgui_tableau_sptr top_tab = grid_->get_tableau_at(col, row);
  if (top_tab)
  {
    vgui_rubberband_tableau_sptr rubber;
    rubber.vertical_cast(vgui_find_below_by_type_name(top_tab,
                                                      vcl_string("vgui_rubberband_tableau")));
    if (rubber)
      return rubber;
  }
  vcl_cout << "Unable to get vgui_rubberband_tableau at (" << col
           << ", " << row << ")\n";
  return vgui_rubberband_tableau_sptr();
}

vil_image_resource_sptr segv_vil_segmentation_manager::selected_image()
{
  bgui_image_tableau_sptr itab = this->selected_image_tab();
  if (!itab)
    return 0;
  return itab->get_image_resource();
}

vil_image_resource_sptr segv_vil_segmentation_manager::image_at(const unsigned col,
                                               const unsigned row)
{
  vgui_tableau_sptr top_tab = grid_->get_tableau_at(col, row);
  if (!top_tab)
    return 0;

  bgui_image_tableau_sptr itab;
  itab.vertical_cast(vgui_find_below_by_type_name(top_tab,
                                                  vcl_string("vgui_image_tableau")));
  if (!itab)
  {
    vcl_cout << "Unable to get bgui_image_tableau at (" << col
             << ", " << row << ")\n";
    return 0;
  }
  return itab->get_image_resource();
}

//-----------------------------------------------------------------------------
//: Clear the display
//-----------------------------------------------------------------------------
void segv_vil_segmentation_manager::clear_display()
{
  bgui_vtol2D_tableau_sptr t2D = this->selected_vtol2D_tab();
  if (!t2D)
    return;
  t2D->clear_all();
}

//-----------------------------------------------------------------------------
//: Draw edges onto the tableau
//-----------------------------------------------------------------------------
void
segv_vil_segmentation_manager::draw_edges(vcl_vector<vtol_edge_2d_sptr>& edges,
                                      bool verts)
{
  bgui_vtol2D_tableau_sptr t2D = this->selected_vtol2D_tab();
  if (!t2D)
    return;
  this->clear_display();
#if 0
  vgui_image_tableau_sptr itab = t2D->get_image_tableau();
  if (!itab)
  {
    vcl_cout << "In segv_vil_segmentation_manager::draw_edges - null image tab\n";
    return;
  }
#endif
  for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges.begin();
       eit != edges.end(); eit++)
  {
    t2D->add_edge(*eit);
    //optionally display the edge vertices
    if (verts)
    {
      if ((*eit)->v1())
      {
        vtol_vertex_2d_sptr v1 = (*eit)->v1()->cast_to_vertex_2d();
        t2D->add_vertex(v1);
      }
      if ((*eit)->v2())
      {
        vtol_vertex_2d_sptr v2 = (*eit)->v2()->cast_to_vertex_2d();
        t2D->add_vertex(v2);
      }
    }
  }
  t2D->post_redraw();
}


//-----------------------------------------------------------------------------
//: Draw polylines on the tableau
//-----------------------------------------------------------------------------
void segv_vil_segmentation_manager::
draw_polylines(vcl_vector<vsol_polyline_2d_sptr > const& polys)
{
  bgui_vtol2D_tableau_sptr t2D = this->selected_vtol2D_tab();
  if (!t2D)
    return;
  //this->clear_display();
  vgui_image_tableau_sptr itab = t2D->get_image_tableau();
  if (!itab)
  {
    vcl_cout << "In segv_vil_segmentation_manager::draw_edges - null image tab\n";
    return;
  }
  for (vcl_vector<vsol_polyline_2d_sptr>::const_iterator pit = polys.begin();
       pit != polys.end(); pit++)
  {
    t2D->add_vsol_polyline_2d(*pit);
  }

  t2D->post_redraw();
}

//-----------------------------------------------------------------------------
//: Draw line segments on the tableau
//-----------------------------------------------------------------------------
void segv_vil_segmentation_manager::
draw_lines(vcl_vector<vsol_line_2d_sptr > const& lines,
           const vgui_style_sptr& style)
{
  bgui_vtol2D_tableau_sptr t2D = this->selected_vtol2D_tab();
  if (!t2D)
    return;
  //this->clear_display();
#if 0
  vgui_image_tableau_sptr itab = t2D->get_image_tableau();
  if (!itab)
  {
    vcl_cout << "In segv_vil_segmentation_manager::draw_edges - null image tab\n";
    return;
  }
#endif
  for (vcl_vector<vsol_line_2d_sptr>::const_iterator lit = lines.begin();
       lit != lines.end(); lit++)
  {
    t2D->add_vsol_line_2d(*lit,style);
  }

  t2D->post_redraw();
}

//-----------------------------------------------------------------------------
//: Draw points on the tableau
//-----------------------------------------------------------------------------
void segv_vil_segmentation_manager::
draw_points(vcl_vector<vsol_point_2d_sptr> const& points, const vgui_style_sptr& style)
{
  bgui_vtol2D_tableau_sptr t2D = this->selected_vtol2D_tab();
  if (!t2D)
    return;
  //this->clear_display();
  vgui_image_tableau_sptr itab = t2D->get_image_tableau();
  if (!itab)
  {
    vcl_cout << "In segv_vil_segmentation_manager::draw_edges - null image tab\n";
    return;
  }
  for (vcl_vector<vsol_point_2d_sptr>::const_iterator pit = points.begin();
       pit != points.end(); pit++)
  {
    t2D->add_vsol_point_2d(*pit,style);
  }

  t2D->post_redraw();
}

void segv_vil_segmentation_manager::draw_regions(vcl_vector<vtol_intensity_face_sptr>& regions,
                                             bool verts)
{
  bgui_vtol2D_tableau_sptr t2D = this->selected_vtol2D_tab();
  if (!t2D)
    return;
  for (vcl_vector<vtol_intensity_face_sptr>::iterator rit = regions.begin();
       rit != regions.end(); rit++)
  {
    vtol_face_2d_sptr f = (*rit)->cast_to_face_2d();
    t2D->add_face(f);
    if (verts)
    {
      vcl_vector<vtol_vertex_sptr> vts;
      f->vertices(vts);
      for (vcl_vector<vtol_vertex_sptr>::iterator vit = vts.begin();
           vit != vts.end(); vit++)
      {
        vtol_vertex_2d_sptr v = (*vit)->cast_to_vertex_2d();
        t2D->add_vertex(v);
      }
    }
  }
  t2D->post_redraw();
}

void segv_vil_segmentation_manager::quit()
{
  vcl_exit(1);
}

void segv_vil_segmentation_manager::load_image()
{
  static bool greyscale = true;
  vgui_dialog load_image_dlg("Load image file");
  static vcl_string image_filename = "/home/dec/images/cal_image1.tif";
  static vcl_string ext = "*.*";
  load_image_dlg.file("Image Filename:", ext, image_filename);
  load_image_dlg.checkbox("greyscale ", greyscale);
  if (!load_image_dlg.ask())
    return;

  vil_image_resource_sptr image =
    vil_load_image_resource(image_filename.c_str());
  if (!image)
  {
    vcl_cout << "Null image resource - couldn't load from "
             << image_filename << '\n';
    return;
  }


#if 0
  if (greyscale)
    image = brip_vil1_float_ops::convert_to_grey(temp);
  else
    image = temp;
#endif
  if (first_)
  {
    this->set_selected_grid_image(image);
    first_ = false;
  }
  else
    this->add_image(image);
}

void segv_vil_segmentation_manager::set_range_params()
{
  bgui_image_tableau_sptr itab = this->selected_image_tab();
  if (!itab||!itab->get_image_resource())
    return;
  static double min = 0.0, max = 255;
  static float gamma = 1.0;
  static bool invert = false;
  static bool gl_map = false;
  static bool cache = false;
  vgui_dialog range_dlg("Set Range Map Params");
  range_dlg.field("Range min:", min);
  range_dlg.field("Range max:", max);
  range_dlg.field("Gamma:", gamma);
  range_dlg.checkbox("Invert:", invert);
  range_dlg.checkbox("Use GL Mapping", gl_map);
  range_dlg.checkbox("Cache Pixels", cache);
  if (!range_dlg.ask())
    return;
  vil_image_resource_sptr img = itab->get_image_resource();
  unsigned n_components = img->nplanes();
  vgui_range_map_params_sptr rmps;
  if (n_components == 1)
     rmps=
      new vgui_range_map_params(min, max, gamma, invert,
                                gl_map, cache);
  else if (n_components == 3)
     rmps =
      new vgui_range_map_params(min, max, min, max, min, max,
                                gamma, gamma, gamma, invert,
                                gl_map, cache);

  itab->set_mapping(rmps);
}

void segv_vil_segmentation_manager::harris_corners()
{
  vil_image_resource_sptr img = selected_image();
  if (!img)
  {
    vcl_cout << "In segv_segmentation_manager::harris_measure) - no image\n";
    return;
  }
  static sdet_harris_detector_params hdp;
  vgui_dialog harris_dialog("harris");
  harris_dialog.field("sigma", hdp.sigma_);
  harris_dialog.field("thresh", hdp.thresh_);
  harris_dialog.field("N = 2n+1, (n)", hdp.n_);
  harris_dialog.field("Max No Corners(percent)", hdp.percent_corners_);
  harris_dialog.field("scale_factor", hdp.scale_factor_);
  harris_dialog.checkbox("Use vil harris\n corner strength ", hdp.use_vil_harris_);
  if (!harris_dialog.ask())
    return;
  sdet_harris_detector hd(hdp);
  hd.set_image_resource(img);
  hd.extract_corners();
  vcl_vector<vsol_point_2d_sptr>& points = hd.get_points();
  int N = points.size();
  if (!N)
    return;
  bgui_vtol2D_tableau_sptr t2D = this->selected_vtol2D_tab();
  if (!t2D)
    return;
  t2D->clear_all();
  for (int i=0; i<N; i++)
    t2D->add_vsol_point_2d(points[i]);
  t2D->post_redraw();
}

void segv_vil_segmentation_manager::vd_edges()
{
  this->clear_display();
  static bool agr = true;
  static sdet_detector_params dp;
  static float nm = 2.0;

  vgui_dialog vd_dialog("VD Edges");
  vd_dialog.field("Gaussian sigma", dp.smooth);
  vd_dialog.field("Noise Threshold", nm);
  vd_dialog.checkbox("Automatic Threshold", dp.automatic_threshold);
  vd_dialog.checkbox("Agressive Closure", agr);
  vd_dialog.checkbox("Compute Junctions", dp.junctionp);
  if (!vd_dialog.ask())
    return;
  dp.noise_multiplier=nm;
  if (agr)
    dp.aggressive_junction_closure=1;
  else
    dp.aggressive_junction_closure=0;
  vil_image_resource_sptr img = selected_image();
  if (!img||!img->ni()||!img->nj())
  {
    vcl_cout << "In segv_vil_segmentation_manager::vd_edges() - no image\n";
    return;
  }
  sdet_detector det(dp);
  det.SetImage(img);

  det.DoContour();
  vcl_vector<vtol_edge_2d_sptr>* edges = det.GetEdges();
  if (edges)
    this->draw_edges(*edges, true);
}

void segv_vil_segmentation_manager::fit_lines()
{
  this->clear_display();
  static sdet_detector_params dp;
  static bool agr = true;
  static float nm = 2.0;

  static sdet_fit_lines_params flp;

  vgui_dialog lf_dialog("Fit Lines");
  lf_dialog.field("Gaussian sigma", dp.smooth);
  lf_dialog.field("Noise Threshold", nm);
  lf_dialog.checkbox("Automatic Threshold", dp.automatic_threshold);
  lf_dialog.checkbox("Agressive Closure", agr);
  lf_dialog.checkbox("Compute Junctions", dp.junctionp);
  lf_dialog.field("Min Fit Length", flp.min_fit_length_);
  lf_dialog.field("RMS Distance", flp.rms_distance_);

  if (!lf_dialog.ask())
    return;
  dp.noise_multiplier=nm;
  if (agr)
    dp.aggressive_junction_closure=1;
  else
    dp.aggressive_junction_closure=0;
  dp.borderp = false;
  sdet_detector det(dp);

  vil_image_resource_sptr img = selected_image();
  if (!img||!img->ni()||!img->nj())
  {
    vcl_cout << "In segv_vil_segmentation_manager::vd_edges() - no image\n";
    return;
  }

  det.SetImage(img);

  det.DoContour();
  vcl_vector<vtol_edge_2d_sptr>* edges = det.GetEdges();
  if (!edges)
  {
    vcl_cout << "No edges to fit lines\n";
    return;
  }
  sdet_fit_lines fl(flp);
  fl.set_edges(*edges);
  fl.fit_lines();
  vcl_vector<vsol_line_2d_sptr> lines = fl.get_line_segs();
  this->draw_lines(lines);
}

void segv_vil_segmentation_manager::regions()
{
  this->clear_display();
  static bool debug = false;
  static bool agr = true;
  static bool residual = false;
  static sdet_detector_params dp;
  static float nm = 1.0;
  vgui_dialog region_dialog("Edgel Regions");
  region_dialog.field("Gaussian sigma", dp.smooth);
  region_dialog.field("Noise Threshold", nm);
  region_dialog.checkbox("Automatic Threshold", dp.automatic_threshold);
  region_dialog.checkbox("Agressive Closure", agr);
  region_dialog.checkbox("Compute Junctions", dp.junctionp);
  region_dialog.checkbox("Debug", debug);
  region_dialog.checkbox("Residual Image", residual);
  if (!region_dialog.ask())
    return;
  dp.noise_multiplier=nm;
  if (agr)
    dp.aggressive_junction_closure=1;
  else
    dp.aggressive_junction_closure=0;

  vil_image_resource_sptr img = selected_image();
  if (!img||!img->ni()||!img->nj())
  {
    vcl_cout << "In segv_vil_segmentation_manager::vd_edges() - no image\n";
    return;
  }

  sdet_region_proc_params rpp(dp, true, debug, 2);
  sdet_region_proc rp(rpp);
  rp.set_image_resource(img);
  rp.extract_regions();
  if (debug)
  {
    vil1_image ed_img = rp.get_edge_image();
#if 0
    vgui_image_tableau_sptr itab =  t2D->get_image_tableau();
    if (!itab)
    {
      vcl_cout << "In segv_vil_segmentation_manager::regions() - null image tableau\n";
      return;
    }
    itab->set_image(ed_img);
    itab->post_redraw();
#endif
//    this->add_image(ed_img);
  }
  if (!debug)
  {
    vcl_vector<vtol_intensity_face_sptr>& regions = rp.get_regions();
    this->draw_regions(regions, true);
  }
  if (residual)
  {
    vil_image_view<float> res_img = rp.get_residual_image_view();
    if (!res_img)
      return;
    this->add_image(vil_new_image_resource_of_view(res_img));
  }
}

void segv_vil_segmentation_manager::test_inline_viewer()
{
  bgui_image_tableau_sptr itab = this->selected_image_tab();
  bgui_range_adjuster_tableau_sptr h= bgui_range_adjuster_tableau_new(itab);
  h->set_hardware(true);
  h->update();
  vgui_viewer2D_tableau_sptr v = vgui_viewer2D_tableau_new(h);
  vgui_shell_tableau_sptr s = vgui_shell_tableau_new(v);
  //popup adjuster
  vgui_dialog test_inline("Histogram Range Adjuster");
  test_inline.inline_tableau(s, 280, 200);
  if (!test_inline.ask())
    return;
}
