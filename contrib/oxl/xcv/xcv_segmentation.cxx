// This is oxl/xcv/xcv_segmentation.cxx
#include "xcv_segmentation.h"
//:
// \file
// See xcv_segmentation.h for a description of this file.
//
// \author K.Y.McGaul

#include <vil1/vil1_image.h>
#include <vcl_cmath.h>

#include <osl/osl_harris_params.h>
#include <osl/osl_harris.h>
#include <osl/osl_canny_ox_params.h>
#include <osl/osl_canny_ox.h>
#include <osl/osl_break_edge.h>
#include <osl/osl_ortho_regress.h>

#include <vgui/vgui.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_macro.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_soview.h>

#include <xcv/xcv_image_tableau.h>

vcl_list<osl_edge*> xcv_segmentation::detected_edges = vcl_list<osl_edge*>();

extern void get_current(unsigned*, unsigned*);
extern bool get_image_at(vil1_image*, unsigned, unsigned);
extern xcv_image_tableau_sptr get_image_tableau_at(unsigned,unsigned);
extern vgui_easy2D_tableau_sptr get_easy2D_at(unsigned, unsigned);
extern void add_image(vil1_image& img);

//-----------------------------------------------------------------------------
//: Make and display a dialog box to get Harris parameters.
//-----------------------------------------------------------------------------
bool xcv_segmentation::get_harris_params(osl_harris_params* params)
{
  vgui_dialog* harris_dialog = new vgui_dialog("Harris");

  harris_dialog->field("Gaussian sigma", params->gauss_sigma);
  harris_dialog->field("Maximum corner count", params->corner_count_max);
  harris_dialog->field("Expected ratio lowest/max corner strength", params->relative_minimum);
  harris_dialog->field("Auto-correlation scale factor", params->scale_factor);
  harris_dialog->checkbox("Verbose", params->verbose);
  harris_dialog->checkbox("Adaptive", params->adaptive);

  return harris_dialog->ask();
}

//-----------------------------------------------------------------------------
//: Perform Harris corner detection.
//-----------------------------------------------------------------------------
void xcv_segmentation::perform_harris(osl_harris_params& params,
                                      unsigned col, unsigned row)
{
  vil1_image img;
  bool image_ok = get_image_at(&img, col, row);
  if (image_ok == false)
    return;
  vgui_easy2D_tableau_sptr easy_tab = get_easy2D_at(col, row);
  if (!easy_tab)
    return;
  osl_harris harris(params);
  harris.compute(img);

  vcl_vector<vcl_pair<float, float> > cor;
  harris.get_corners(cor);

  if (!easy_tab)
  {
    vgui_macro_warning << "Can't get current easy2D to add Harris corners\n";
    return;
  }

  //easy_tab->set_point_radius(3.0);
  xcv_image_tableau_sptr tableau = get_image_tableau_at(col,row);
  float low[3],high[3];
  tableau->get_bounding_box(low,high);
  for (unsigned i = 0; i < cor.size(); i++)
    easy_tab->add_point(low[0]+cor[i].first, low[1]+cor[i].second);
}

//-----------------------------------------------------------------------------
//: Get the current row and column and params for Harris corner detection.
//-----------------------------------------------------------------------------
void xcv_segmentation::harris()
{
  unsigned col, row;
  get_current(&col, &row);

  osl_harris_params params;
  if (!get_harris_params(&params))
    return;

  perform_harris(params, col, row);
}

//-----------------------------------------------------------------------------
//: Draw the given edges onto the given location.
//-----------------------------------------------------------------------------
void xcv_segmentation::draw_edges(vcl_list<osl_edge*> lines, unsigned col,
                                  unsigned row)
{
  vgui_easy2D_tableau_sptr easy_tab = get_easy2D_at(col, row);
  if (!easy_tab)
    return;

  // Delete old edges and then add the new edges:
  vcl_vector<vgui_soview*> sel_edges = easy_tab->get_all();
  for (vcl_vector<vgui_soview*>::iterator i = sel_edges.begin();
    i != sel_edges.end(); i++)
  {
    if ((*i)->type_name() == "vgui_soview2D_linestrip")
      easy_tab->remove((vgui_soview*)(*i));
  }

  xcv_image_tableau_sptr tableau = get_image_tableau_at(col,row);
  float low[3],high[3];
  tableau->get_bounding_box(low,high);
  for (vcl_list<osl_edge*>::const_iterator i = lines.begin(); i != lines.end(); ++i)
  {
    osl_edge const* e = *i;
    float *x = e->GetY(),*y = e->GetX(); // note x-y confusion.
    // - Offset the edges
    for (unsigned int j = 0;j<e->size();j++)
    {
      x[j]+=low[0];
      y[j]+=low[1];
    }
    easy_tab->add_linestrip(e->size(),x, y);
    for (unsigned int j = 0;j<e->size();j++)
    {
      x[j]-=low[0];
      y[j]-=low[1];
    }
  }
  easy_tab->post_redraw();
}

//-----------------------------------------------------------------------------
//: Draw straight lines onto the given location.
//-----------------------------------------------------------------------------
void xcv_segmentation::draw_straight_lines(vcl_vector<float> x1, vcl_vector<float> y1,
                                           vcl_vector<float> x2, vcl_vector<float> y2,
                                           unsigned col, unsigned row)
{
  vgui_easy2D_tableau_sptr easy_tab = get_easy2D_at(col, row);
  if (!easy_tab)
    return;

  // Delete old edges and then add the new edges:
  vcl_vector<vgui_soview*> sel_edges = easy_tab->get_all();
  for (vcl_vector<vgui_soview*>::iterator i = sel_edges.begin();
    i != sel_edges.end(); i++)
  {
    if ((*i)->type_name() == "vgui_soview2D_linestrip")
      easy_tab->remove((vgui_soview*)(*i));
  }

  xcv_image_tableau_sptr tableau = get_image_tableau_at(col,row);
  float low[3],high[3];
  tableau->get_bounding_box(low,high);
  for (unsigned i=0; i<x1.size(); i++)
  {
    // - Offset the edges
    float offset_x1 = x1[i] + low[0];
    float offset_y1 = y1[i] + low[1];
    float offset_x2 = x2[i] + low[0];
    float offset_y2 = y2[i] + low[1];
    easy_tab->add_line(offset_x1, offset_y1, offset_x2, offset_y2);
  }

  easy_tab->post_redraw();
}

//-----------------------------------------------------------------------------
//: Display a dialog box to get the parameters needed for Oxford Canny.
//-----------------------------------------------------------------------------
bool xcv_segmentation::get_canny_ox_params(osl_canny_ox_params* params)
{
  vgui_dialog canny_ox_dialog("Canny - Oxford");

  canny_ox_dialog.field("Standard deviation (sigma)", params->sigma);
  canny_ox_dialog.field("Max smoothing kernel width", params->max_width);
  canny_ox_dialog.field("Gauss tail", params->gauss_tail);
  canny_ox_dialog.field("Low hysteresis threshold", params->low);
  canny_ox_dialog.field("High hysteresis threshold", params->high);
  canny_ox_dialog.field("Min edge pixel intensity", params->edge_min);
  canny_ox_dialog.field("Min pixels in curve", params->min_length);
  canny_ox_dialog.field("Border size", params->border_size);
  canny_ox_dialog.field("Scale", params->scale);
  canny_ox_dialog.field("Strategy", params->follow_strategy);
  canny_ox_dialog.checkbox("Enable pixel jumping?", params->join_flag);
  canny_ox_dialog.field("Junction option", params->junction_option);
  canny_ox_dialog.checkbox("Verbose?", params->verbose);

  return canny_ox_dialog.ask();
}


//-----------------------------------------------------------------------------
//: Perform Oxford canny edge detection.
//-----------------------------------------------------------------------------
void xcv_segmentation::canny_ox()
{
  unsigned col, row;
  get_current(&col, &row);
  vil1_image img;
  bool image_ok = get_image_at(&img, col, row);
  if (image_ok == false)
    return;

  osl_canny_ox_params params;
  if (!get_canny_ox_params(&params))
    return;
  osl_canny_ox cox(params);
  cox.detect_edges(img, &detected_edges);

  draw_edges(detected_edges, col, row);
}

//-----------------------------------------------------------------------------
//: Get parameters from the user for break_lines_ox.
//-----------------------------------------------------------------------------
bool xcv_segmentation::get_break_lines_ox_params(double* bk_thresh)
{
  vgui_dialog dlg("Contour break parameters");
  dlg.field("Break threshold", *bk_thresh);
  return dlg.ask();
}

//-----------------------------------------------------------------------------
//: Get a list of broken edges from the current image.
//-----------------------------------------------------------------------------
void xcv_segmentation::get_broken_edges(double bk_thresh, vcl_list<osl_edge*>* broken_edges)
{
  unsigned col, row;
  get_current(&col, &row);
  vil1_image img;
  bool image_ok = get_image_at(&img, col, row);
  if (!image_ok)
    return;

  // Canny OX finding edges ------
  osl_canny_ox_params params;

  params.sigma = 1.0;
  params.max_width = 50;
  params.gauss_tail = 0.0001f;
  params.low = 2.0;
  params.border_size = 2;
  params.border_value = 0.0;
  params.scale = 5.0;
  params.follow_strategy = 2;
  params.join_flag = 1;
  params.junction_option = 0;
  params.high = 12.0;
  params.edge_min = 60;
  params.min_length = 60;

  osl_canny_ox cox(params);
  cox.detect_edges(img, &detected_edges);

  // breaking edges -----
  for (vcl_list<osl_edge*>::iterator iter = detected_edges.begin();
       iter!= detected_edges.end(); iter++)
    osl_break_edge(*iter, broken_edges, bk_thresh);
}

//-----------------------------------------------------------------------------
//: Break edges at points of extreme "bentness".
//-----------------------------------------------------------------------------
void xcv_segmentation::break_lines_ox()
{
  double bk_thresh = 0.3;
  get_break_lines_ox_params(&bk_thresh);

  unsigned col, row;
  get_current(&col, &row);

  vcl_list<osl_edge*> broken_edges;
  get_broken_edges(bk_thresh, &broken_edges);
  draw_edges(broken_edges, col, row);
}

//-----------------------------------------------------------------------------
//: Get parameters from user required for detect_lines_ox.
//-----------------------------------------------------------------------------
bool xcv_segmentation::get_detect_lines_ox_params(float* high, int* edge_min,
  int* min_length, int* min_fit_length)
{
  vgui_dialog dlg("");
  dlg.field("High threshold", *high);
  dlg.field("Min. edgel intensity", *edge_min);
  dlg.field("Min. edgel length", *min_length);
  dlg.field("Min. line length", *min_fit_length);
  return dlg.ask();
}

//-----------------------------------------------------------------------------
//: Detect straight lines in the edges computed by Canny.
//-----------------------------------------------------------------------------
void xcv_segmentation::detect_lines_ox()
{
  float high = 12.0;
  int edge_min = 60;
  int min_length = 60;
  int min_fit_length = 10;
  get_detect_lines_ox_params(&high, &edge_min, &min_length, &min_fit_length);

  unsigned col, row;
  get_current(&col, &row);

  double bk_thresh = 0.3;
  vcl_list<osl_edge*> broken_edges;
  get_broken_edges(bk_thresh, &broken_edges);

  // Select straight edges only:
  vcl_vector<float> x1, y1, x2, y2;
  for (vcl_list<osl_edge*>::iterator iter = broken_edges.begin();
    iter!= broken_edges.end(); iter++)
  {
    osl_edge* e = *iter;
    if (e->size() < (unsigned int)min_fit_length)  // Reject edges which are too short
      continue;

    // Fit a straight line to the edgel:
    osl_ortho_regress fitter;
    fitter.reset();
    fitter.add_points(e->GetX(), e->GetY(), e->size());
    double a,b,c;
    fitter.fit(&a, &b, &c);
    if (fitter.rms_cost(a,b,c) > 0.7)  // Reject edges which are too bent
      continue;

    // Now project the endpoints of the edge onto the fitted line:
    double m = vcl_sqrt(a*a + b*b);
    if (m == 0)
      continue;

    double la = a/m, lb = b/m, lc = c/m;

    double X1 = e->GetStartX();
    double Y1 = e->GetStartY();
    double X2 = e->GetEndX();
    double Y2 = e->GetEndY();

    x1.push_back(lb*lb*X1 - la*lb*Y1 - la*lc);
    y1.push_back(la*la*Y1 - la*lb*X1 - lb*lc);
    x2.push_back(lb*lb*X2 - la*lb*Y2 - la*lc);
    y2.push_back(la*la*Y2 - la*lb*X2 - lb*lc);
  }

  draw_straight_lines(y1, x1, y2, x2, col, row);
}

//-----------------------------------------------------------------------------
//: Creates a menu containing all the functions in this file.
//-----------------------------------------------------------------------------
vgui_menu xcv_segmentation::create_segmentation_menu()
{
  vgui_menu seg_menu;
  seg_menu.add("Harris corner detection", harris);
  seg_menu.separator();
  seg_menu.add("Canny: Oxford", canny_ox);
//  seg_menu.add("Canny: Van-Duc", canny_vd);
  seg_menu.add("Break lines: Oxford", break_lines_ox);
  seg_menu.add("Detect lines: Oxford", detect_lines_ox);

  return seg_menu;
}
