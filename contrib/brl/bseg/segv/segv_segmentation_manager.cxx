// This is brl/bseg/segv/segv_segmentation_manager.cxx
#include "segv_segmentation_manager.h"
//:
// \file
// \author J.L. Mundy

#include <vcl_cstdlib.h> // for vcl_exit()
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vbl/vbl_array_2d.h>
#include <vil1/vil1_load.h>
#include <vil1/vil1_crop.h>
#include <vil1/vil1_memory_image_of.h>
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
#include <vgui/vgui_key.h>
#include <vgui/vgui_modifier.h>
#include <vgui/vgui.h>
#include <vgui/vgui_find.h>
#include <vgui/vgui_error_dialog.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_macro.h>
#include <vgui/vgui_image_tableau.h>
#include <bgui/bgui_image_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <bgui/bgui_picker_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_soview.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_curve_2d_sptr.h>
#include <vtol/vtol_one_chain_sptr.h>
#include <vtol/vtol_one_chain.h>
#include <vtol/vtol_intensity_face.h>
#include <bgui/bgui_vtol2D_tableau.h>
#include <brip/brip_float_ops.h>
#include <bsol/bsol_hough_line_index.h>
#include <sdet/sdet_region_proc_params.h>
#include <sdet/sdet_region_proc.h>
//static live_video_manager instance
segv_segmentation_manager *segv_segmentation_manager::instance_ = 0;


segv_segmentation_manager *segv_segmentation_manager::instance()
{
  if (!instance_)
  {
    instance_ = new segv_segmentation_manager();
    instance_->init();
  }
  return segv_segmentation_manager::instance_;
}

//-----------------------------------------------------------
// constructors/destructor
//
segv_segmentation_manager::segv_segmentation_manager():vgui_wrapper_tableau()
{
}

segv_segmentation_manager::~segv_segmentation_manager()
{
}

//: Set up the tableaux
void segv_segmentation_manager::init()
{
  vgui_image_tableau_sptr itab = bgui_image_tableau_new();
  t2D_ = bgui_vtol2D_tableau_new(itab);
  //  t2D_->set_edge_style(0.0, 0.0, 0.0, 2.0);
  picktab_ = bgui_picker_tableau_new(t2D_);
  vgui_viewer2D_tableau_sptr v2d = vgui_viewer2D_tableau_new(picktab_);
  vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(v2d);
  this->add_child(shell);
}

void segv_segmentation_manager::quit()
{
  vcl_exit(1);
}

void segv_segmentation_manager::load_image()
{
  static bool greyscale = true;
  vgui_dialog load_image_dlg("Load image file");
  static vcl_string image_filename = "/home/dec/images/cal_image1.tif";
  static vcl_string ext = "*.*";
  load_image_dlg.file("Image Filename:", ext, image_filename);
  load_image_dlg.checkbox("greyscale ", greyscale);
  if (!load_image_dlg.ask())
    return;

  vil1_image temp = vil1_load(image_filename.c_str());

  if (greyscale)
    img_ = brip_float_ops::convert_to_grey(temp);
  else
    img_ = temp;

  t2D_->get_image_tableau()->set_image(img_);
  t2D_->post_redraw();
}

//-----------------------------------------------------------------------------
//: Clear the display
//-----------------------------------------------------------------------------
void segv_segmentation_manager::clear_display()
{
  if (!t2D_)
    return;
  t2D_->clear_all();
}

//-----------------------------------------------------------------------------
//: Draw edges onto the tableau
//-----------------------------------------------------------------------------
void
segv_segmentation_manager::draw_edges(vcl_vector<vtol_edge_2d_sptr>& edges,
                                      bool verts)
{
  if (!t2D_)
    return;
  this->clear_display();
  vgui_image_tableau_sptr itab = t2D_->get_image_tableau();
  if (!itab)
  {
    vcl_cout << "In segv_segmentation_manager::draw_edges - null image tab\n";
    return;
  }
  for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges.begin();
       eit != edges.end(); eit++)
  {
    t2D_->add_edge(*eit);
    //optionally display the edge vertices
    if (verts)
    {
      if ((*eit)->v1())
      {
        vtol_vertex_2d_sptr v1 = (*eit)->v1()->cast_to_vertex_2d();
        t2D_->add_vertex(v1);
      }
      if ((*eit)->v2())
      {
        vtol_vertex_2d_sptr v2 = (*eit)->v2()->cast_to_vertex_2d();
        t2D_->add_vertex(v2);
      }
    }
  }
  t2D_->post_redraw();
}
//-----------------------------------------------------------------------------
//: Draw line segments on the tableau
//-----------------------------------------------------------------------------
void segv_segmentation_manager::
draw_lines(vcl_vector<vsol_line_2d_sptr > const& lines)
{
  if (!t2D_)
    return;
  //this->clear_display();
  vgui_image_tableau_sptr itab = t2D_->get_image_tableau();
  if (!itab)
  {
    vcl_cout << "In segv_segmentation_manager::draw_edges - null image tab\n";
    return;
  }
  for (vcl_vector<vsol_line_2d_sptr>::const_iterator lit = lines.begin();
       lit != lines.end(); lit++)
  {
    t2D_->add_vsol_line_2d(*lit);
  }

  t2D_->post_redraw();
}
//-----------------------------------------------------------------------------
//: Draw line segments on the tableau
//-----------------------------------------------------------------------------
void segv_segmentation_manager::
draw_lines(vcl_vector<vsol_line_2d_sptr > const& lines,
           float r, float g, float b, int width)
{
  if (!t2D_)
    return;
  //this->clear_display();
  vgui_image_tableau_sptr itab = t2D_->get_image_tableau();
  if (!itab)
  {
    vcl_cout << "In segv_segmentation_manager::draw_edges - null image tab\n";
    return;
  }
  for (vcl_vector<vsol_line_2d_sptr>::const_iterator lit = lines.begin();
       lit != lines.end(); lit++)
  {
    t2D_->add_vsol_line_2d(*lit,r,g,b,width);
  }

  t2D_->post_redraw();
}

//-----------------------------------------------------------------------------
//: Draw points on the tableau
//-----------------------------------------------------------------------------
void segv_segmentation_manager::
draw_points(vcl_vector<vsol_point_2d_sptr> const& points, float r, float g, float b, int radius)
{
  if (!t2D_)
    return;
  //this->clear_display();
  vgui_image_tableau_sptr itab = t2D_->get_image_tableau();
  if (!itab)
  {
    vcl_cout << "In segv_segmentation_manager::draw_edges - null image tab\n";
    return;
  }
  for (vcl_vector<vsol_point_2d_sptr>::const_iterator pit = points.begin();
       pit != points.end(); pit++)
  {
    t2D_->add_vsol_point_2d(*pit,r,g,b,radius);
  }

  t2D_->post_redraw();
}

void segv_segmentation_manager::draw_regions(vcl_vector<vtol_intensity_face_sptr>& regions,
                                             bool verts)
{
  for (vcl_vector<vtol_intensity_face_sptr>::iterator rit = regions.begin();
       rit != regions.end(); rit++)
  {
    vtol_face_2d_sptr f = (*rit)->cast_to_face_2d();
    t2D_->add_face(f);
    if (verts)
    {
      vcl_vector<vtol_vertex_sptr>* vts = f->vertices();
      for (vcl_vector<vtol_vertex_sptr>::iterator vit = vts->begin();
           vit != vts->end(); vit++)
      {
        vtol_vertex_2d_sptr v = (*vit)->cast_to_vertex_2d();
        t2D_->add_vertex(v);
      }
      delete vts;
    }
  }
  t2D_->post_redraw();
}

void segv_segmentation_manager::original_image()
{
  if (img_)
  {
    t2D_->get_image_tableau()->set_image(img_);
    t2D_->post_redraw();
  }
}

void segv_segmentation_manager::roi()
{
  float x0=0, y0=0, x1=0, y1=0;

  vcl_cout << "Choose upper left corner of ROI.\n";
  picktab_->pick_point(&x0,&y0);
  vcl_cout << "picked (x="<<x0<<", y="<<y0<<")\n"
           << "Choose lower right corner of ROI.\n";
  picktab_->pick_point(&x1,&y1);
  vcl_cout << "picked (x="<<x1<<", y="<<y1<<")\n";
  if ( (x1 > x0) && (y1 > y0) )
  {
    int w = int(x1 - x0);
    int h = int(y1 - y0);
    vil1_image cropped = vil1_crop(img_,int(x0),int(y0),w,h);
    vcl_cout << "cropped x=" <<x0<<" y=" <<y0<< " w=" <<w<<" h=" <<h<< '\n';
    if (cropped)
    {
      img_ = cropped;
      t2D_->get_image_tableau()->set_image(cropped);
      t2D_->post_redraw();
      return;
    }
    vcl_cout << "crop failed.\n";
    return;
  }
  vcl_cout << "invalid ROI\n";
  return;
}

void segv_segmentation_manager::gaussian()
{
  if (!img_)
  {
    vcl_cout << "In segv_segmentation_manager::gaussian() - no image\n";
    return;
  }
  static float sigma = 1.0;
  vgui_dialog gauss_dialog("Gaussian Smooth");
  gauss_dialog.field("Gaussian sigma", sigma);
  if (!gauss_dialog.ask())
    return;
  vil1_memory_image_of<float> input(img_);
  vil1_memory_image_of<float> smooth = brip_float_ops::gaussian(input, sigma);
  vil1_memory_image_of<unsigned char> char_smooth =
    brip_float_ops::convert_to_byte(smooth);
  t2D_->get_image_tableau()->set_image(char_smooth);
  t2D_->post_redraw();
}

void segv_segmentation_manager::convolution()
{
  vgui_dialog kernel_dlg("Load Kernel");
  static vcl_string kernel_filename = "c:/images";
  static vcl_string ext = "*.*";
  kernel_dlg.file("Kernel Filename:", ext, kernel_filename);
  if (!kernel_dlg.ask())
    return;
  vbl_array_2d<float> kernel = brip_float_ops::load_kernel(kernel_filename);

  //convert input image
  vil1_memory_image_of<unsigned char> temp(img_);
  vil1_memory_image_of<float> temp2 = brip_float_ops::convert_to_float(temp);

  //convolve
  vil1_memory_image_of<float> conv = brip_float_ops::convolve(temp2, kernel);

  //convert back to unsigned char
  vil1_memory_image_of<unsigned char> char_conv =
    brip_float_ops::convert_to_byte(conv);

  //display the image
  t2D_->get_image_tableau()->set_image(char_conv);
  t2D_->post_redraw();
}

void segv_segmentation_manager::downsample()
{
  if (!img_)
  {
    vcl_cout << "In segv_segmentation_manager::downsample) - no image\n";
    return;
  }
  static float filter_factor = 0.36f;
  vgui_dialog downsample_dialog("Downsample");
  downsample_dialog.field("Bert-Adelson Factor", filter_factor);
  if (!downsample_dialog.ask())
    return;
  vil1_memory_image_of<unsigned char> input(img_);
  vil1_memory_image_of<float> inputf = brip_float_ops::convert_to_float(input);
  vil1_memory_image_of<float> half_res =
    brip_float_ops::half_resolution(inputf, filter_factor);
  vil1_memory_image_of<unsigned char> char_half_res =
    brip_float_ops::convert_to_byte(half_res);
  t2D_->get_image_tableau()->set_image(char_half_res);
  t2D_->post_redraw();
}

void segv_segmentation_manager::harris_measure()
{
  if (!img_)
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
  if (!harris_dialog.ask())
    return;
  sdet_harris_detector hd(hdp);
  hd.set_image(img_);
  hd.extract_corners();
  vcl_vector<vsol_point_2d_sptr>& points = hd.get_points();
  int N = points.size();
  if (!N)
    return;
  t2D_->clear_all();
  for (int i=0; i<N; i++)
    t2D_->add_vsol_point_2d(points[i]);
  t2D_->post_redraw();
}

void segv_segmentation_manager::beaudet_measure()
{
  if (!img_)
  {
    vcl_cout <<"In segv_segmentation_manager::beaudet_measure) - no image\n";
    return;
  }
  static float sigma = 1.0f;
  //static float scale_factor = 0.04f;
  //static int n = 2;
  static float cmax=100;
  vgui_dialog harris_dialog("beaudet");
  harris_dialog.field("sigma", sigma);
  harris_dialog.field("range", cmax);
  if (!harris_dialog.ask())
    return;
  int w = img_.width(), h = img_.height();
  vil1_memory_image_of<unsigned char> input(img_);
  vil1_memory_image_of<float> inputf = brip_float_ops::convert_to_float(input);
  vil1_memory_image_of<float> smooth = brip_float_ops::gaussian(inputf, sigma);
  vil1_memory_image_of<float> Ixx, Ixy, Iyy, b;
  Ixx.resize(w,h);  Ixy.resize(w,h);   Iyy.resize(w,h);
  brip_float_ops::hessian_3x3(smooth, Ixx, Ixy, Iyy);
  b = brip_float_ops::beaudet(Ixx, Ixy, Iyy);
  vil1_memory_image_of<unsigned char> uchar_b =
    brip_float_ops::convert_to_byte(b,0.0f, cmax);
  t2D_->get_image_tableau()->set_image(uchar_b);
  t2D_->post_redraw();
}

void segv_segmentation_manager::vd_edges()
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

  sdet_detector det(dp);
  det.SetImage(img_);

  det.DoContour();
  vcl_vector<vtol_edge_2d_sptr>* edges = det.GetEdges();
  if (edges)
    this->draw_edges(*edges, true);
}

void segv_segmentation_manager::regions()
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

  sdet_region_proc_params rpp(dp, true, debug, 2);
  sdet_region_proc rp(rpp);
  rp.set_image(img_);
  rp.extract_regions();
  if (debug)
  {
    vil1_image ed_img = rp.get_edge_image();
    vgui_image_tableau_sptr itab =  t2D_->get_image_tableau();
    if (!itab)
      {
        vcl_cout << "In segv_segmentation_manager::regions() - null image tableau\n";
        return;
      }
    itab->set_image(ed_img);
    itab->post_redraw();
  }
  if (!debug)
  {
    vcl_vector<vtol_intensity_face_sptr>& regions = rp.get_regions();
    this->draw_regions(regions, true);
  }
  if (residual)
  {
    vil1_image res_img = rp.get_residual_image();
    vgui_image_tableau_sptr itab =  t2D_->get_image_tableau();
    if (!itab)
    {
      vcl_cout << "In segv_segmentation_manager::regions() - null image tableau\n";
      return;
    }
    itab->set_image(res_img);
    itab->post_redraw();
  }
}

// Test calculated camera parameter matrices (K and M) by reading from a file.
// file should be in the following format:
// K n_views M1, M2, M3... Mn_views
// where K is the 3x3 intrinsic parameter matrix,
// n_views is an integer value > 0,
// and M1 - Mn_views are the 4x4 homogeneous extrinsic parameter matrices.
void segv_segmentation_manager::test_camera_parms()
{
  this->clear_display();
  vgui_dialog tcp_dialog("Test Camera Parms");
  static vcl_string camera_parms_filename = "/home/dec/camera_parms.left.txt";
  static vcl_string input_points_filename = "/home/dec/point_correspondences.left.txt";
  static vcl_string ext = "*.*";
  static bool show_input_points = true;
  static bool show_calculated_points = true;
  static int view_num = 1;
  tcp_dialog.file("camera parms file:", ext, camera_parms_filename);
  tcp_dialog.file("input points file:", ext, input_points_filename);
  tcp_dialog.field("view number:",view_num);
  tcp_dialog.checkbox("show input points",show_input_points);
  tcp_dialog.checkbox("show calculated points",show_calculated_points);

  if (!tcp_dialog.ask())
    return;

  if (show_calculated_points)
  {
    // read K
    vnl_matrix_fixed<double,3,3> K;
    vcl_ifstream parms_instream(camera_parms_filename.c_str());

    double k_values[9];
    for (int i=0; i < 9; i++)
    {
      parms_instream >> k_values[i];
      //vcl_cout << "k_values["<<i<<"] = "<<k_values[i] << '\n';
    }
    K.put(0,0,k_values[0]); K.put(0,1,k_values[1]); K.put(0,2,k_values[2]);
    K.put(1,0,k_values[3]); K.put(1,1,k_values[4]); K.put(1,2,k_values[5]);
    K.put(2,0,k_values[6]); K.put(2,1,k_values[7]); K.put(2,2,k_values[8]);

    vcl_cout << "K =\n" << K << '\n';

    // read number of views
    int n_views = 0;
    parms_instream >> n_views;

    // read M
    vnl_matrix_fixed<double,3,4> M;

    double m_values[12];
    double dummy;
    for (int v = 1; v <= view_num; v++)
    {
      if (v > n_views)
      {
        vcl_cout << "error: view number > n_views\n";
        break;
      }
      for (int i=0; i < 12; i++)
      {
        parms_instream >> m_values[i];
        //vcl_cout << "m_values["<<i<<"] = "<<m_values[i] << '\n';
      }
      // read 4th row, should just be [0 0 0 1]
      for (int i=0; i < 4; i++)
      {
        parms_instream >> dummy;
      }
    }
    parms_instream.close();

    M.put(0,0,m_values[0]); M.put(0,1,m_values[1]); M.put(0,2,m_values[2]);  M.put(0,3,m_values[3]);
    M.put(1,0,m_values[4]); M.put(1,1,m_values[5]); M.put(1,2,m_values[6]);  M.put(1,3,m_values[7]);
    M.put(2,0,m_values[8]); M.put(2,1,m_values[9]); M.put(2,2,m_values[10]); M.put(2,3,m_values[11]);

    vcl_cout << "M =\n" << M << '\n';

    //transform the grid points to the image
    sdet_grid_finder_params gfp;
    sdet_grid_finder gf(gfp);
    vcl_vector<vsol_point_2d_sptr> calculated_points;
    gf.transform_grid_points(K,M,calculated_points);

    // draw points on image
    this->draw_points(calculated_points,1.0f,0.0f,0.0f,5);
  }
  if (show_input_points)
  {
    vcl_vector<vsol_point_2d_sptr> input_points;
    vcl_ifstream points_instream(input_points_filename.c_str());
    int n_points = 0;
    // read number of points
    points_instream >> n_points;
    // read grid points, discard
    double dummy;
    double* points_x = new double[n_points];
    double* points_y = new double[n_points];
    for (int i = 0; i < n_points; i++)
    {
      points_instream >> dummy; //x
      points_instream >> dummy; //y
    }

    // read number of views
    int n_views = 0;
    points_instream >> n_views;

    // read grid points for view number
    for (int v = 1; v <= view_num; v++)
    {
      if (v > n_views)
      {
        vcl_cout << "error: view number > n_views\n";
        break;
      }
      for (int i = 0; i < n_points; i++)
      {
        points_instream >> points_x[i];
        points_instream >> points_y[i];
      }
    }
    for (int i = 0; i < n_points; i++)
    {
      vsol_point_2d_sptr point = new vsol_point_2d(points_x[i],points_y[i]);
      input_points.push_back(point);
    }
    this->draw_points(input_points,0.0f,1.0f,0.0f,4);
    delete[] points_x;
    delete[] points_y;
  }

  return;
}

void segv_segmentation_manager::fit_lines()
{
   this->clear_display();
  static sdet_grid_finder_params gfp;
  vcl_vector<vcl_string> choices;
  gfp.get_debug_choices(choices);
  static bool agr = true;
  static sdet_detector_params dp;
  dp.borderp=false;
  static sdet_fit_lines_params flp;
  static float nm = 2.0;
  static bool detect_grid=true;
//static bool grid_debug=false;
  static bool matched_lines = false;
  static bool manual_pt_selection = false;
  vgui_dialog vd_dialog("Fit Lines");
  vd_dialog.field("Gaussian sigma", dp.smooth);
  vd_dialog.field("Noise Threshold", nm);
  vd_dialog.checkbox("Automatic Threshold", dp.automatic_threshold);
  vd_dialog.checkbox("Agressive Closure", agr);
  vd_dialog.checkbox("Compute Junctions", dp.junctionp);
  vd_dialog.field("Min Fit Length", flp.min_fit_length_);
  vd_dialog.field("RMS Distance", flp.rms_distance_);
  vd_dialog.field("Angle Tolerance", gfp.angle_tol_);
  vd_dialog.field("Line Count Threshold", gfp.thresh_);
  vd_dialog.checkbox("Detect Grid", detect_grid);
  vd_dialog.checkbox("Grid Debug Output", gfp.verbose_);
  vd_dialog.checkbox("Matched Lines", matched_lines);
  vd_dialog.checkbox("Use Manual Point Selection",manual_pt_selection);
  vd_dialog.choice("Choose Debug Line Display", choices, gfp.debug_state_);

  if (!vd_dialog.ask())
    return;
  dp.noise_multiplier=nm;
  if (agr)
    dp.aggressive_junction_closure=1;
  else
    dp.aggressive_junction_closure=0;
  dp.borderp = false;
  sdet_detector det(dp);
  det.SetImage(img_);

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
  if (detect_grid)
  {
    sdet_grid_finder gf(gfp);
    if (!gf.set_lines(img_.width(), img_.height(), lines))
    {
      vcl_cout << "Less than two dominant groups\n";
      return;
    }
    vcl_vector<vsol_line_2d_sptr> mapped_lines;
    vcl_vector<vsol_line_2d_sptr> mapped_grid_lines;
    if (manual_pt_selection)
    {
      vsol_point_2d_sptr corners[4];
      vcl_cout << "Select the four corners of the grid, starting with "
               << "the upper left and moving clockwise.\n";
      for (int p=0; p<4; p++)
      {
        float x=0, y=0;
        picktab_->pick_point(&x,&y);
        vcl_cout << "corner "<< p <<" (x=" << x << ", y=" << y <<")\n";
        corners[p] = new vsol_point_2d(x,y);
      }
      gf.compute_manual_homography(corners[0],corners[1],
                                   corners[2],corners[3]);
      if (!gfp.debug_state_)
        //gf.get_mapped_lines(mapped_lines);
        gf.get_backprojected_grid(mapped_lines);
      else
        gf.get_debug_lines(mapped_lines);

      this->draw_lines(mapped_lines);
      return;
    }

    gf.compute_homography();
    // double-check grid match
    if (!gf.check_grid_match(img_))
    {
      // for now just display message - if this was a video process
      // we would want to disregard this homography and move on
      vcl_cout << "warning: grid match failed double-check\n";
    }
    if (!gfp.debug_state_)
      //gf.get_mapped_lines(mapped_lines);
      gf.get_backprojected_grid(mapped_lines);
    else
    {
      gf.get_debug_lines(mapped_lines);
      gf.get_debug_grid_lines(mapped_grid_lines);
      this->draw_lines(mapped_grid_lines,1.0f,0.0f,0.0f,1);
    }
    this->draw_lines(mapped_lines);
    return;
  }
  this->draw_lines(lines);
}
#if 0
#ifdef HAS_XERCES
void segv_segmentation_manager::read_xml_edges()
{
  vgui_dialog load_image_dlg("Load XML edges");
  static vcl_string xml_filename = "";
  static vcl_string ext = "*.*";
  load_image_dlg.file("XML filename:", ext, xml_filename);
  if (!load_image_dlg.ask())
    return;
  vcl_vector<vtol_edge_2d_sptr> edges;
  if (bxml_vtol_io::read_edges(xml_filename, edges))
    this->draw_edges(edges, true);
}
#endif
#endif
void segv_segmentation_manager::test_face()
{
  if (!img_)
    return;
  int sx = img_.cols(), sy = img_.rows();
  if (sx<10||sy<10)
    return;
  t2D_->set_foreground(0.0, 1.0, 0.0);
  vsol_point_2d_sptr pa = new vsol_point_2d(1,1);
  vsol_point_2d_sptr pb = new vsol_point_2d(sx-2,1);
  vsol_point_2d_sptr pc = new vsol_point_2d(sx-2,sy-2);
  vsol_point_2d_sptr pd = new vsol_point_2d(1,sy-2);
  vsol_curve_2d_sptr cab = new vdgl_digital_curve(pa, pb);
  vsol_curve_2d_sptr cbc = new vdgl_digital_curve(pb, pc);
  vsol_curve_2d_sptr ccd = new vdgl_digital_curve(pc, pd);
  vsol_curve_2d_sptr cda = new vdgl_digital_curve(pd, pa);
  vtol_vertex_2d_sptr va = new vtol_vertex_2d(*pa);
  vtol_vertex_2d_sptr vb = new vtol_vertex_2d(*pb);
  vtol_vertex_2d_sptr vc = new vtol_vertex_2d(*pc);
  vtol_vertex_2d_sptr vd = new vtol_vertex_2d(*pd);
  vtol_edge_2d_sptr eab = new vtol_edge_2d(va, vb, cab);
  vtol_edge_2d_sptr ebc = new vtol_edge_2d(vb, vc, cbc);
  vtol_edge_2d_sptr ecd = new vtol_edge_2d(vc, vd, ccd);
  vtol_edge_2d_sptr eda = new vtol_edge_2d(vd, va, cda);
  vcl_vector<vtol_edge_sptr> edges;
  edges.push_back(eab->cast_to_edge());
  edges.push_back(ebc->cast_to_edge());
  edges.push_back(ecd->cast_to_edge());
  edges.push_back(eda->cast_to_edge());
  vtol_one_chain_sptr b_onch = new vtol_one_chain(edges,true);
  vtol_face_2d_sptr b_f = new  vtol_face_2d(b_onch);
  int px = sx/2, py = sy/2;

  vsol_point_2d_sptr p1 = new vsol_point_2d(px+3,py+3);
  vsol_point_2d_sptr p2 = new vsol_point_2d(px,py-3);
  vsol_point_2d_sptr p3 = new vsol_point_2d(px-3,py+3);
  vsol_curve_2d_sptr c12 = new vdgl_digital_curve(p1, p2);
  vsol_curve_2d_sptr c23 = new vdgl_digital_curve(p2, p3);
  vsol_curve_2d_sptr c31 = new vdgl_digital_curve(p3, p1);
  vtol_vertex_2d_sptr v1 = new vtol_vertex_2d(*p1);
  vtol_vertex_2d_sptr v2 = new vtol_vertex_2d(*p2);
  vtol_vertex_2d_sptr v3 = new vtol_vertex_2d(*p3);
  vtol_edge_2d_sptr e12 = new vtol_edge_2d(v1, v2, c12);
  vtol_edge_2d_sptr e23 = new vtol_edge_2d(v2, v3, c23);
  vtol_edge_2d_sptr e31 = new vtol_edge_2d(v3, v1, c31);
  edges.clear();
  edges.push_back(e12->cast_to_edge());
  edges.push_back(e23->cast_to_edge());
  edges.push_back(e31->cast_to_edge());
  vtol_one_chain_sptr t_onch = new vtol_one_chain(edges,true);
  vtol_face_2d_sptr t_f = new  vtol_face_2d(t_onch);
  t2D_->add_face(b_f);   t2D_->add_face(t_f);
  t2D_->set_foreground(1.0, 0.0, 0.0);
  for (int x = 0; x<sx; x+=20)
    for (int y = 0; y<sy; y+=20)
    {
      vtol_vertex_2d_sptr v = new vtol_vertex_2d(x, y);
      t2D_->add_vertex(v);
    }
}

void segv_segmentation_manager::test_digital_lines()
{
  t2D_->set_foreground(1.0, 1.0, 0.0);
  vsol_point_2d_sptr pa = new vsol_point_2d(0,0);
  vsol_point_2d_sptr pb = new vsol_point_2d(20,0);
  vsol_point_2d_sptr pc = new vsol_point_2d(10, 20);
  vsol_point_2d_sptr pd = new vsol_point_2d(20,20);
  vsol_point_2d_sptr pe = new vsol_point_2d(20, 10);
  vsol_point_2d_sptr pf = new vsol_point_2d(0, 20);
  vdgl_digital_curve_sptr cab = new vdgl_digital_curve(pa, pb);
  vdgl_digital_curve_sptr cac = new vdgl_digital_curve(pa, pc);
  vdgl_digital_curve_sptr cad = new vdgl_digital_curve(pa, pd);
  vdgl_digital_curve_sptr cae = new vdgl_digital_curve(pa, pe);
  vdgl_digital_curve_sptr caf = new vdgl_digital_curve(pa, pf);
  t2D_->add_digital_curve(cab);
  t2D_->add_digital_curve(cac);
  t2D_->add_digital_curve(cad);
  t2D_->add_digital_curve(cae);
  t2D_->add_digital_curve(caf);
}
