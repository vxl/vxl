// This is ./oxl/xcv/xcv_axes_tableau.cxx

//:
//  \file
// \author   K.Y.McGaul
//
// See xcv_axes_tableau.h for a description of this file.
//
// \verbatim
//  Modifications:
//   K.Y.McGaul 26-APR-2001   Initial version.
// \endverbatim

#include <xcv/xcv_axes_tableau.h>

#include <vcl_iostream.h>
#include <vcl_cstdio.h>
#include <vcl_cmath.h>

#include <vnl/vnl_math.h>
#include <vgui/vgui.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_soview2D.h>
#include <vgui/vgui_easy2D.h>
#include <vgui/vgui_text_tableau.h>
#include <vgui/vgui_composite.h>

//========================================================================
//: Constructor takes all the labels used on the graph.
xcv_axes_tableau::xcv_axes_tableau(vcl_string heading, vcl_string x_axes_label,
  vcl_string y_axes_label)
{
  xlow_ = 0.0;
  xhigh_ = 0.0;
  ylow_ = 0.0;
  yhigh_ = 0.0;
  heading_ = heading;
  x_axes_label_ = x_axes_label;
  y_axes_label_ = y_axes_label;
  easy_ = vgui_easy2D_new();
  text_ = vgui_text_tableau_new();
  comp_ = vgui_composite_new(easy_, text_);
}

//========================================================================
//: Destructor.
xcv_axes_tableau::~xcv_axes_tableau()
{
}

//========================================================================
//: Add another point to be plotted on the graph.
//  Note - once all points are added call compute_axes to draw the graph.
void xcv_axes_tableau::add_point(float x, float y)
{
  if (xpoints_.size() == 0)
  {
    xhigh_ = x;
    xlow_ = x;
    yhigh_ = y;
    ylow_ = y;
  }
  if (x > xhigh_)
    xhigh_ = x;
  if (x < xlow_)
    xlow_ = x;
  if (y > yhigh_)
    yhigh_ = y;
  if (y < ylow_)
    ylow_ = y;

  xpoints_.push_back(x);
  ypoints_.push_back(y);
}

//========================================================================
//: Lay out the graph.
//  xcv_axes_limits is called to compute the numbering and tick spacing for the
//  graph.  Then the graph is drawn by adding objects to an easy2D and a
//  text_tableau.
void xcv_axes_tableau::compute_axes()
{
  // Decide the offset, height & width of the graph:
  int left_offset = 130; //KYM - this should change depending on width of y-label
  int graph_width = 650 - left_offset;
  int top_offset = 50;
  int graph_height = 360;
  int tick_width = 3;

  // Compute the positions for the tick marks:
  xlimits_.set_range(xlow_, xhigh_);
  xlimits_.calc_ticks();
  ylimits_.set_range(ylow_, yhigh_);
  ylimits_.calc_ticks();

  // Draw a square around the graph:
  vcl_vector<float> x_corners, y_corners;
  x_corners.push_back(left_offset);
  x_corners.push_back(left_offset+graph_width);
  x_corners.push_back(left_offset+graph_width);
  x_corners.push_back(left_offset);
  y_corners.push_back(top_offset);
  y_corners.push_back(top_offset);
  y_corners.push_back(top_offset+graph_height);
  y_corners.push_back(top_offset+graph_height);
  easy_->add_polygon(4, &x_corners[0]/*.begin()*/, &y_corners[0]/*.begin()*/);

  // Add text (headings, axes labels and numbering):
  text_->add(left_offset,top_offset-10, heading_);
  text_->add(left_offset + graph_width/2, top_offset+graph_height+40, x_axes_label_);
  text_->add(0, top_offset+graph_height/2, y_axes_label_);

  float xspacing = graph_width / (2*xlimits_.tick_n);
  for (int xcounter = 0; xcounter < xlimits_.tick_n+1; xcounter++)
  {
    float tmp_width = left_offset + (2*xcounter*xspacing);

    // tick marks:
    easy_->add_line(tmp_width, top_offset+graph_height+tick_width,
                    tmp_width, top_offset+graph_height);
    if (xcounter != xlimits_.tick_n)
      easy_->add_line(tmp_width+xspacing, top_offset+graph_height+tick_width,
                      tmp_width+xspacing, top_offset+graph_height);

    // numbering:
    char tmp_number_string[20];
    if (xlimits_.tick_spacing < 1)
    {
      float tmp_number = xlimits_.tick_start + (xcounter*xlimits_.tick_spacing);
      vcl_sprintf(tmp_number_string, "%.1f", tmp_number);
    }
    else
    {
      int tmp_number = (int)vcl_ceil(xlimits_.tick_start + (xcounter*xlimits_.tick_spacing));
      vcl_sprintf(tmp_number_string, "%d", tmp_number);
    }
    text_->add(tmp_width-10, top_offset+graph_height+20, tmp_number_string);
  }

  float yspacing = graph_height / (2*ylimits_.tick_n);
  for (int ycounter = 0; ycounter < ylimits_.tick_n+1; ycounter++)
  {
    float tmp_height = top_offset + (2*ycounter*yspacing);

    // tick marks:
    easy_->add_line(left_offset-tick_width, tmp_height, left_offset, tmp_height);
    if (ycounter != ylimits_.tick_n)
      easy_->add_line(left_offset-tick_width, tmp_height+yspacing,
                      left_offset, tmp_height+yspacing);

    // numbering:
    char tmp_number_string[20];
    if (ylimits_.tick_spacing < 1)
    {
      float tmp_number = ylimits_.tick_end - (ycounter*ylimits_.tick_spacing);
      vcl_sprintf(tmp_number_string, "%.1f", tmp_number);
    }
    else
    {
      int tmp_number = (int)vcl_ceil(ylimits_.tick_end - (ycounter*ylimits_.tick_spacing));
      vcl_sprintf(tmp_number_string, "%d", tmp_number);
    }
    text_->add(left_offset-40, tmp_height+3, tmp_number_string);
  }

  // Re-calibrate points and add them to the graph:
  vcl_vector<float> xcalib, ycalib;
  float xscaling = graph_width/(xlimits_.tick_end - xlimits_.tick_start);
  for (unsigned int i=0; i<xpoints_.size(); i++)
    xcalib.push_back(left_offset + (xpoints_[i] - xlimits_.tick_start)*xscaling);

  float yscaling = graph_height/(ylimits_.tick_end - ylimits_.tick_start);
  for (unsigned int i=0; i<ypoints_.size(); i++)
  {
    ycalib.push_back(
      top_offset + graph_height - ((ypoints_[i]-ylimits_.tick_start)*yscaling));
  }

  easy_->add_linestrip(xcalib.size(), &xcalib[0]/*.begin()*/, &ycalib[0]/*.begin()*/);
}

//========================================================================
//: Handles all events for this tableau.
bool xcv_axes_tableau::handle(const vgui_event& event)
{
  // Pass events on down to the child tableaux:
  //child_tab->handle(event);
  comp_->handle(event);

  return true;
}
