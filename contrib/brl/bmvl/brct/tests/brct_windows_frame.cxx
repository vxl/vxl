// This is brl/bmvl/brct/tests/brct_windows_frame.cxx
#include "brct_windows_frame.h"
#include "brct_menus.h"
//:
// \file
// \author Kongbin Kang

#include <vcl_cstdlib.h> // for vcl_exit()
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_fstream.h>
#include <vcl_cassert.h>
#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_easy3D_tableau.h>
#include <vgui/vgui_soview3D.h> // for vgui_lineseg3D
#include <vgui/vgui_viewer3D_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_composite_tableau.h>
#include <vgui/vgui_composite_tableau_sptr.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <vil1/vil1_load.h>
#define JOE_RECON
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
brct_windows_frame::brct_windows_frame() : vgui_wrapper_tableau()
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
  tab_3d_->set_foreground(1,0,0);
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
  vgui_image_tableau_sptr tab_img = vgui_image_tableau_new();
  vgui_easy2D_tableau_sptr easy2d = vgui_easy2D_tableau_new(tab_img);

  bgui_picker_tableau_sptr picker = bgui_picker_tableau_new(easy2d);
  vgui_rubberband_easy2D_client* r_client = new vgui_rubberband_easy2D_client(easy2d);
  vgui_rubberband_tableau_sptr rubber = vgui_rubberband_tableau_new(r_client);

  vgui_composite_tableau_sptr tab2d = vgui_composite_tableau_new(picker, rubber);

  tab_picker_ = picker;
  tab_rubber_ = rubber;
  tab_cps_ = tab2d;
  easy_2d_ = easy2d;
  img_2d_ = tab_img;
  easy_2d_->set_foreground(0, 0, 1);

  vgui_viewer2D_tableau_sptr v2d = vgui_viewer2D_tableau_new(tab2d);
  grid_->add_at(v2d, col, row);

  vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(grid_);
  this->add_child(shell);

  // set a kalman filter
  //data_file_name_ = "c:/videos/SUV2/track_data/par.txt";
  //data_file_name_ = "c:/videos/SUV3/track_data/par.txt";
  //  data_file_name_ = "c:/videos/VWBus/track_data/par.txt";
  //data_file_name_ = "c:/videos/PoliceCar2/track_data/par.txt";
  //  data_file_name_ = "c:/vxl/vxl/contrib/brl/bmvl/brct/tests/par.txt";
  data_file_name_ = "";
#ifdef JOE_RECON
  kalman_ = 0;
  epi_recon_ = new brct_epi_reconstructor(data_file_name_.c_str());
#else
  kalman_ = new kalman_filter(data_file_name_.c_str());
  epi_recon_ = 0;
#endif
  show_epipolar_lines_=false;
  e_ = 0;
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

  if (epi_recon_)
    delete epi_recon_;

  if (e_)
    delete e_;
}

void brct_windows_frame::add_curve2d(vcl_vector<vgl_point_2d<double> > &pts,
                                     float r, float g, float b)
{
  int size = pts.size();
  assert(size > 1);
  curves_2d_.resize(size);
  instance_->easy_2d_->set_foreground(1, 0, 0);
  for (int i=0; i<size; i++)
  {
    vgl_point_2d<double>& s = pts[i];
#if 1
    vgui_soview2D *so = instance_->easy_2d_->add_point(s.x(), s.y());
    so->set_point_size(5);
    so->set_colour(r, g, b);
#else
    vgl_point_2d<double>& e = i<size-1 ? pts[i+1] : pts[0];
    vgui_soview2D* so = instance_->easy_2d_->add_line(s.x(), s.y(), e.x(), e.y());
#endif
#if 0
    so->set_point_size(2);
    so->set_colour(1, 0, 0);
#endif
    curves_2d_[i] = so;
  }

  instance_->post_redraw();
}

void brct_windows_frame::remove_curve2d()
{
  predicted_curves_2d_.clear();
  curves_2d_.clear();
  this->post_redraw();
}

void brct_windows_frame::add_curve3d(bugl_curve_3d& c3d)
{
  int size = c3d.get_num_points();
  if (size > 1){
    instance_->tab_3d_->set_foreground(1, 1, 1);
    for (int i=0; i<size; i++)
    {
      bugl_normal_point_3d_sptr s = c3d.get_point(i);
      bugl_normal_point_3d_sptr e = c3d.get_neighbor(i, 1);
      if (e.ptr()!=0) {
        //vgui_soview3D* so = instance_->tab_3d_->add_line(s->x(), s->y(), s->z(), e->x(), e->y(), e->z());
        vgui_soview3D* so = instance_->tab_3d_->add_point(s->x(), s->y(), s->z());
        curves_3d_.push_back(so);
      }
    }
  instance_->post_redraw();
  }
}

void brct_windows_frame::remove_debug_info()
{
  int size = predicted_curves_2d_.size();
  for (int i=0; i<size; i++)
    instance_->easy_2d_->remove(predicted_curves_2d_[i]);
  predicted_curves_2d_.size();

  size = debug_curves_2d_.size();
  for (int i=0; i<size; i++)
    instance_->easy_2d_->remove(debug_curves_2d_[i]);
  predicted_curves_2d_.size();

  this->post_redraw();
}

void brct_windows_frame::remove_curve3d()
{
  int size = curves_3d_.size();
  for (int i=0; i<size; i++)
    instance_->tab_3d_->remove(curves_3d_[i]);
  curves_3d_.clear();
  this->post_redraw();
}


void brct_windows_frame::
show_epipolar_lines(vcl_vector<vgl_point_2d<double> > const& c2d)
{
  vgl_point_2d<double> e;
  if(kalman_)
    e = kalman_->get_cur_epipole();
  else
    e = epi_recon_->get_cur_epipole();
  double ex = e.x(), ey = e.y();
  int n =0;
  for(vcl_vector<vgl_point_2d<double> >::const_iterator pit = c2d.begin();
      pit != c2d.end(); pit++)
    {
      double px = (*pit).x(), py = (*pit).y();
      easy_2d_->set_foreground(1, 1, 0);
      easy_2d_->add_infinite_line( -(py-ey), px-ex, ex*py - px*ey);
      easy_2d_->set_foreground(1, 0, 0);
      n++;
    }
}
void brct_windows_frame::init_kalman()
{
  vcl_vector<vgl_point_2d<double> > c2d;

  if (!e_)
  {
    assert(lines_.size()>=2);
    init_epipole();
  }

  if (!kalman_&&!epi_recon_)
    {
      vcl_cout<<"brct_windows_frame::kalman or recon not created yet\n";
      return;
    }
  if(kalman_)
    kalman_->init();
  else
    epi_recon_->init();

  // add the curve in the first view
  if(kalman_)
    c2d = kalman_->get_pre_observes();
  else
    c2d = epi_recon_->get_joe_pre_observes();

  add_curve2d(c2d, 0.0, 1.0, 0.0);
  if(show_epipolar_lines_)
    this->show_epipolar_lines(c2d);

  //update the display.
  bugl_curve_3d c3d;
  if(kalman_)
    c3d = kalman_->get_curve_3d();
  else
    c3d = epi_recon_->get_curve_3d();

  add_curve3d(c3d);

  // add the curve in the second view
  if(kalman_)
    c2d = kalman_->get_cur_observes();
  else
    c2d = epi_recon_->get_joe_cur_observes();

  add_curve2d(c2d);

  instance_->post_redraw();
}

void brct_windows_frame::go()
{
  remove_curve3d();
  

  // add current data
  vcl_vector<vgl_point_2d<double> > c2d;
  if(kalman_)
  {
    c2d = kalman_->get_cur_observes();
	kalman_->inc();
  }
  else
  {
    c2d = epi_recon_->get_joe_cur_observes();
    epi_recon_->inc();
  }
  add_curve2d(c2d);

  //add 3D result
  bugl_curve_3d c3d;
  if(kalman_)
    c3d = kalman_->get_curve_3d();
  else
    c3d = epi_recon_->get_curve_3d();

  add_curve3d(c3d);

  show_epipole();
  this->post_redraw();
}

void brct_windows_frame::show_predicted_curve()
{
  vnl_matrix<double> pts;
  if(kalman_)
    pts = kalman_->get_predicted_curve();
  else
    pts = epi_recon_->get_predicted_curve();

  int num_points = pts.columns();
  vcl_vector<vgl_point_2d<double> > curve(num_points);

  for (int i=0; i<num_points; i++)
    curve[i]= vgl_point_2d<double>(pts[0][i], pts[1][i]);

  add_predicted_curve2d(curve);
  //add_curve2d(curve);
  this->post_redraw();
}

void brct_windows_frame::add_predicted_curve2d(vcl_vector<vgl_point_2d<double> > &pts)
{
  int size = pts.size();
  assert(size > 1);
  predicted_curves_2d_.resize(size);
  for (int i=0; i<size; i++) {
    vgl_point_2d<double>& s = pts[i];
    vgui_soview2D* so = instance_->easy_2d_->add_point(s.x(), s.y());
    so->set_colour(0, 0, 1);
    so->set_point_size(2);
    predicted_curves_2d_[i] = so;
  }

  instance_->post_redraw();
}

void brct_windows_frame::add_next_observes(vcl_vector<vgl_point_2d<double> >&pts)
{
  int size = pts.size();
  assert(size > 1);
  debug_curves_2d_.resize(size);
  instance_->easy_2d_->set_foreground(0, 0, 1);
  for (int i=0; i<size; i++) {
    vgl_point_2d<double>& s = pts[i];
#if 1
    debug_curves_2d_[i]= instance_->easy_2d_->add_point(s.x(), s.y());
#else
    vgl_point_2d<double>& e = i<size-1 ? pts[i+1] : pts[0];
    debug_curves_2d_[i]= instance_->easy_2d_->add_line(s.x(),s.y(),e.x(),e.y());
#endif
  }

  instance_->post_redraw();
}

void brct_windows_frame::show_next_observes()
{
  vcl_vector<vgl_point_2d<double> > c2d;
  if(kalman_)
    c2d = kalman_->get_next_observes();
  else
    c2d = epi_recon_->get_joe_next_observes();

  add_next_observes(c2d);
}

void brct_windows_frame::show_back_projection()
{
  vcl_vector<vnl_matrix<double> > c2d;
  if(kalman_)
    c2d = kalman_->get_back_projection();
  else
    c2d = epi_recon_->get_back_projection();

  instance_->easy_2d_->set_foreground(0, 0, 1);

  int framenum = c2d.size();

  for (int f=0; f<framenum; f++)
  {
    int size = c2d[f].cols();
    assert(size > 1);

    for (int i=0; i<size; i++)
    {
      double x1 = c2d[f][0][i],
             y1 = c2d[f][1][i];
#if 1
      vgui_soview2D* so = instance_->easy_2d_->add_point(x1, y1);
#else
      double x2 = i<size-1 ? c2d[f][0][i+1] : c2d[f][0][0],
             y2 = i<size-1 ? c2d[f][1][i+1] : c2d[f][1][0];
      vgui_soview2D* so = instance_->easy_2d_->add_line(x1, y1, x2, y2);
#endif
      so->set_colour(0, 0, 1);
      so->set_point_size(2);
      debug_curves_2d_.push_back(so);
    }
  }
  instance_->post_redraw();
}

void brct_windows_frame::print_motion_array()
{
  if(kalman_)
  {
	vcl_cout << "Not Implemented\n";
    return;
  }
  else
    epi_recon_->print_motion_array();

}

void brct_windows_frame::load_image()
{
  bool greyscale = false;
  vgui_dialog load_image_dlg("Load Image");
  static vcl_string image_filename = "";
  static vcl_string ext = "*.*";
  load_image_dlg.file("Image Filename:", ext, image_filename);
  load_image_dlg.checkbox("greyscale ", greyscale);
  if (!load_image_dlg.ask())
    return;
  img_ = vil1_load(image_filename.c_str());
  if (img_2d_)
  {
    img_2d_->set_image(img_);
    instance_->post_redraw();
    status_info_ = image_filename;
    status_info_ += "\n";
  }
  else
    vcl_cout << "In brct_windows_frame::load_image() - null tableau\n";
}

void brct_windows_frame::show_epipole()
{
  instance_->easy_2d_->set_foreground(1, 0, 0);
  vgl_point_2d<double> e;
  if(kalman_)
    e = kalman_->get_cur_epipole();
  else
    e = epi_recon_->get_cur_epipole();

  vcl_cout<<"\n epipole ("<<e.x() <<'\t'<<e.y()<<")\n";
  instance_->easy_2d_->add_point(e.x(), e.y());
}

void brct_windows_frame::init_epipole()
{
  if (!e_)
    e_ = new vgl_point_2d<double>;

  assert(lines_.size() >= 2);
  vgl_homg_point_2d<double> epipole = vgl_homg_operators_2d<double>::lines_to_point(lines_);

  vcl_cout<<"epipole = ("<<epipole.x()<<' ' << epipole.y() << ' '<< epipole.w()<<")\n";

  vgl_point_2d<double> pt(epipole);
  e_ -> set(pt.x(), pt.y());

  if(kalman_)
    kalman_->init_epipole(pt.x(), pt.y());
  else
    epi_recon_->init_epipole(pt.x(), pt.y());

  vcl_stringstream ss;
  ss<<pt.x()<<' '<<pt.y();
  status_info_ += ss.str();
}

void brct_windows_frame::creat_line()
{
  float x1=0, y1=0, x2=0, y2=0;
  vcl_cout<<"pick a line\n";
  tab_picker_->pick_line(&x1, &y1, &x2, &y2);
  vgl_homg_point_2d<double> p1(x1, y1, 1), p2(x2, y2, 1);
  vgl_homg_line_2d<double> l(p1, p2);
  lines_.push_back(l);
  easy_2d_->add_infinite_line( y1-y2, x2-x1, x1*y2 - x2*y1 );
  instance_->post_redraw();
}

void brct_windows_frame::save_status()
{
  vgui_dialog save_file_dlg("save status");
  static vcl_string filename = "";
  static vcl_string ext = "*.*";
  save_file_dlg.file("file name", ext, filename);
  if (!save_file_dlg.ask())
    return;

  if (filename != "")
  {
    vcl_ofstream of(filename.c_str());
    of << status_info_;
  }
}

void brct_windows_frame::load_status()
{
  vgui_dialog load_file_dlg("load status");
  static vcl_string filename = "";
  static vcl_string ext = "*.*";
  load_file_dlg.file("file name", ext, filename);
  if (!load_file_dlg.ask())
    return;

  if (filename != "")
  {
    vcl_ifstream in(filename.c_str());

    vcl_string str;
    in >> str;
    img_ = vil1_load(str.c_str());
    status_info_ = str;
    status_info_ += "\n";

    if (img_2d_)
    {
      img_2d_->set_image(img_);
      instance_->post_redraw();
    }

    double x, y;
    in >> x >> y;
    if (!e_)
      e_ = new vgl_point_2d<double>;
    e_ -> set(x, y);

  if(kalman_)
    kalman_->init_epipole(x, y);
  else
    epi_recon_->init_epipole(x, y);

    vcl_stringstream ss;
    ss << x <<' '<<y;
    status_info_ += ss.str();
  }
}

static write_vrml_header(vcl_ofstream& str)
{
  str << "#VRML V2.0 utf8 \n";
  str << "Background {  \n";
  str << "  skyColor [ 1 1 1 ]\n";
  str << "  groundColor [ 1 1 1 ]\n";
  str << "}\n";
  str << "PointLight {\n";
  str << "  on FALSE\n";
  str << "  intensity 1 \n";
  str << "ambientIntensity 0 \n";
  str << "color 1 1 1 \n";
  str << "location 0 0 0\n"; 
  str << "attenuation 1 0 0 \n";
  str << "radius 100  \n";
  str << "}\n";
  str << "Shape {\n";
  str << " #make the points white\n";
  str << "  appearance Appearance {\n";
  str << "   material Material { emissiveColor 1 0 0 }\n";
  str << " } \n";
  str << " geometry PointSet {\n";
  str << "  coord Coordinate{\n";
  str << "   point[\n";
}
static write_vrml_trailer(vcl_ofstream& str)
{
str << "  ] \n";
str << "  }\n";
str << "       color Color { color [ 1 1 1 ] }\n";
str << "  }\n";
str << " }\n";
}

void brct_windows_frame::write_results_file()
{
  vgui_dialog save_file_dlg("save vrml file");
  static vcl_string filename = "";
  static vcl_string ext = "*.*";
  save_file_dlg.file("file name", ext, filename);

  if(!save_file_dlg.ask())
    return ;

  if(filename == "")
    return;
	
  if(kalman_)
  { 
	  vcl_cout << "Not Implemented\n";
	  return;
  }
  else
    epi_recon_->write_results(filename.c_str());

}

void brct_windows_frame::write_vrml_file()
{
  vgui_dialog save_file_dlg("save vrml file");
  static vcl_string filename = "";
  static vcl_string ext = "*.*";
  save_file_dlg.file("file name", ext, filename);

  if (!save_file_dlg.ask())
    return;

  if(filename == "")
    return;
  
  vcl_ofstream out(filename.c_str());
  if(!out)
    return;
  write_vrml_header(out);
 bugl_curve_3d c3d;
  if(kalman_)
   c3d = kalman_->get_curve_3d();
  else
    c3d = epi_recon_->get_curve_3d();

  int size = c3d.get_num_points();
  vcl_vector<vgl_point_3d<double> > pts(size);
    for(int i=0; i<size; i++){
      if(!c3d.get_point(i)->exists())
        continue;//JLM
      pts[i].set(c3d.get_point(i)->x(), c3d.get_point(i)->y(), c3d.get_point(i)->z());
      out << -pts[i].x() << " " << pts[i].y() << " " << pts[i].z() << "\n";
    }
    
    write_vrml_trailer(out);
    out.close();
}
