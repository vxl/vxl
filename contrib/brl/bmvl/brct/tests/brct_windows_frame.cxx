//:
// \file
// \author Kongbin Kang

#include "brct_windows_frame.h"
#include <vcl_cstdlib.h> // for vcl_exit()
#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_easy3D_tableau.h>
#include <vgui/vgui_viewer3D_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include "brct_menus.h"
//static live_video_manager instance
brct_windows_frame *brct_windows_frame::instance_ = 0;

//===============================================================
//: The singleton pattern - only one instance of the manager can occur
//==============================================================
brct_windows_frame *brct_windows_frame::instance()
{
  if (!instance_)
  {
    instance_ = new brct_windows_frame();
    instance_->init();
  }
  return brct_windows_frame::instance_;
}

//==================================================================
//: constructors/destructor
//==================================================================
brct_windows_frame::
brct_windows_frame() : vgui_wrapper_tableau()
{
}

brct_windows_frame::~brct_windows_frame()
{
}

//======================================================================
//: set up the tableaux at each grid cell
//  the vtol2D_tableaux have been initialized in the constructor
//======================================================================
void brct_windows_frame::init()
{
  grid_ = vgui_grid_tableau_new(2,1);
  grid_->set_grid_size_changeable(true);
  unsigned int col=0, row = 0;
  // add 3D tableau

  vgui_easy3D_tableau_new tab3d;
  tab_3d_ = tab3d;
  tab_3d_->set_point_radius(2);
  // Add a point at the origin
  tab_3d_->set_foreground(1,1,1);
  tab_3d_->add_point(0,0,0);

    // Add a line in the xaxis:
  tab_3d_->set_foreground(1,1,0);
  tab_3d_->add_line(1,0,0, 4,0,0);

  // Add a line in the yaxis:
  tab_3d_->set_foreground(0,1,0);
  tab_3d_->add_line(0,1,0, 0,4,0);

  // Add a line in the zaxis:
  tab_3d_->set_foreground(0,0,1);
  tab_3d_->add_line(0,0,1, 0,0,4);

  vgui_viewer3D_tableau_sptr v3d = vgui_viewer3D_tableau_new(tab_3d_);
  grid_->add_at(v3d, col+1, row);

  // initialize the easy 2d grid
  vgui_easy2D_tableau_new tab2d;

  tab_2d_ = tab2d;
  tab_2d_->set_foreground(0, 0, 1);

  vgui_viewer2D_tableau_sptr v2d = vgui_viewer2D_tableau_new(tab_2d_);
  grid_->add_at(v2d, col, row);

  vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(grid_);
  this->add_child(shell);

  // set a kalman filter
  kalman_ = new kalman_filter();
}

//=========================================================================
//: make an event handler
// note that we have to get an adaptor and set the tableau to receive events
// this handler does nothing but is a place holder for future event processing
// For now, just pass the events down to the child tableaux
//=========================================================================
bool brct_windows_frame::handle(const vgui_event &e)
{
  return this->child.handle(e);
}


void brct_windows_frame::quit()
{
  clean_up();
  vcl_exit(1);
}

void brct_windows_frame::clean_up()
{
  if (kalman_)
    delete kalman_;
}

void brct_windows_frame::add_curve2d(vcl_vector<vgl_point_2d<double> > &pts)
{
  int size = pts.size();
  assert(size > 1);
  curves_2d_.resize(size-1);
  instance_->tab_2d_->set_foreground(1, 1, 1);
  for (int i=0; i<size-1; i++) {
    vgl_point_2d<double>& s = pts[i];
    vgl_point_2d<double>& e = pts[i+1];
    vgui_soview2D_lineseg* l = instance_->tab_2d_->add_line(s.x(), s.y(), e.x(), e.y());
    curves_2d_[i] = l;
  }

  instance_->post_redraw();
}

void brct_windows_frame::remove_curve2d()
{
  predicted_curves_2d_.clear();
  curves_2d_.clear();
  this->post_redraw();
}

void brct_windows_frame::add_curve3d(vcl_vector<vgl_point_3d<double> >& pts)
{
  int size = pts.size();
  assert(size > 1);
  curves_3d_.resize(size-1);
  instance_->tab_3d_->set_foreground(1, 1, 1);
  for (int i=0; i<size-1; i++) {
    vgl_point_3d<double>& s = pts[i];
    vgl_point_3d<double>& e = pts[i+1];
    vgui_lineseg3D* l = instance_->tab_3d_->add_line(s.x(), s.y(), s.z(), e.x(), e.y(), e.z());
    curves_3d_[i] = l;
  }
  instance_->post_redraw();
}

void brct_windows_frame::remove_curve3d()
{
  int size = curves_3d_.size();
  for(int i=0; i<size; i++){
    instance_->tab_3d_->remove(curves_3d_[i]);
  }
  curves_3d_.clear();
  this->post_redraw();
}

void brct_windows_frame::init_kalman()
{

  vcl_vector<vgl_point_2d<double> > c2d;

  if (kalman_ == 0)
    vcl_cout<<"brct_windows_frame::kalman_ not created yet\n";
  else {

    kalman_->read_data("data/curves.txt");
    kalman_->init();
}

   // add the curve in the first view
   c2d = kalman_->get_pre_observes();
   add_curve2d(c2d);

  //update the display.
  vcl_vector<vgl_point_3d<double> > c3d = kalman_->get_local_pts();
  add_curve3d(c3d);

  // add the curve in the second view
  c2d = kalman_->get_cur_observes();
  add_curve2d(c2d); 
}

void brct_windows_frame::go()
{
  remove_curve3d();
  kalman_->inc();

  // add current data
  vcl_vector<vgl_point_2d<double> > c2d = kalman_->get_cur_observes();
  add_curve2d(c2d);
  
  //add 3D resoult
  vcl_vector<vgl_point_3d<double> > c3d = kalman_->get_local_pts();
  add_curve3d(c3d);
  this->post_redraw();
}

void brct_windows_frame::show_predicted_curve()
{
  vnl_matrix<double> pts = kalman_->get_predicted_curve();
  vcl_vector<vgl_point_2d<double> > curve;

  int num_points = pts.columns();
  curve.resize(num_points);

  for (int i=0; i<num_points; i++) {
    vgl_point_2d<double> pt(pts[0][i], pts[1][i]);
    curve[i]=pt;
  }
  
  add_predicted_curve2d(curve);
  this->post_redraw();
}

void brct_windows_frame::add_predicted_curve2d(vcl_vector<vgl_point_2d<double> > &pts)
{
  int size = pts.size();
  assert(size > 1);
  predicted_curves_2d_.resize(size-1);
  instance_->tab_2d_->set_foreground(0, 1, 0);
  for (int i=0; i<size-1; i++) {
    vgl_point_2d<double>& s = pts[i];
    vgui_soview2D_point* p = instance_->tab_2d_->add_point(s.x(), s.y());
    predicted_curves_2d_[i] = p;
  }

  instance_->post_redraw();
}
