// This is brl/bmvl/bmvv/bmvv_cal_manager.cxx
#include "bmvv_cal_manager.h"
//:
// \file
// \author J.L. Mundy

#include <vcl_cstdlib.h> // for vcl_exit()
#include <vcl_iostream.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_math.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vgui/vgui.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_button.h>
#include <vgui/vgui_find.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_macro.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_projection_inspector.h>
#include <bgui/bgui_image_tableau.h>
#include <bgui/bgui_range_adjuster_tableau.h>
#include <bgui/bgui_vtol2D_tableau.h>
#include <bgui/bgui_picker_tableau.h>
#include <brip/brip_vil1_float_ops.h>
#include <brct/brct_algos.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_3d.h>
//static live_video_manager instance
bmvv_cal_manager *bmvv_cal_manager::instance_ = 0;
//===============================================================
//: The singleton pattern - only one instance of the manager can occur
//==============================================================
bmvv_cal_manager *bmvv_cal_manager::instance()
{
  if (!instance_)
  {
    instance_ = new bmvv_cal_manager();
    instance_->init();
  }
  return bmvv_cal_manager::instance_;
}

//==================================================================
//: constructors/destructor
//==================================================================
bmvv_cal_manager::
bmvv_cal_manager() : vgui_wrapper_tableau(), cam_(vgl_p_matrix<double>()),
                     prev_cam_(vgl_p_matrix<double>())
{                       
  corrs_.resize(0);
  corrs_valid_.resize(0);
  proj_image_pts_.resize(0);
  corr_sovs_.resize(0);
}

bmvv_cal_manager::~bmvv_cal_manager()
{
}

//======================================================================
//: set up the tableaux at each grid cell
//  the vtol2D_tableaux have been initialized in the constructor
//======================================================================
void bmvv_cal_manager::init()
{
  itab_ = bgui_image_tableau_new();
  btab_ = bgui_vtol2D_tableau_new(itab_);
  ptab_ = bgui_picker_tableau_new(btab_); 
  vgui_viewer2D_tableau_sptr vtab =  vgui_viewer2D_tableau_new(ptab_);
  vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(vtab);
  this->add_child(shell);
}

//=========================================================================
//: make an event handler
// note that we have to get an adaptor and set the tableau to receive events
// this handler does nothing but is a place holder for future event processing
// For now, just pass the events down to the child tableaux
//=========================================================================
bool bmvv_cal_manager::handle(const vgui_event &e)
{
  float ix=0, iy=0;
  vgui_projection_inspector proj;
  if (e.type == vgui_KEY_PRESS)
    {
      if(btab_)
        if(e.key == 'u')
          {
            btab_->deselect_all();
            return true;
          }
    }
  return this->child.handle(e);
}

//=========================================================================
//: Quit
//=========================================================================
void bmvv_cal_manager::quit()
{
  vcl_exit(1);
}

//=========================================================================
//: load an image an put it in the currently selected grid cell
//=========================================================================
void bmvv_cal_manager::load_image_file(vcl_string image_filename, bool /* greyscale */, unsigned col, unsigned row)
{
  img_ = vil_load_image_resource(image_filename.c_str());
  if(!img_)
    {
      vcl_cout << "Null image resource - couldn't load from " 
               << image_filename << '\n';
      return;
    }
  if (itab_)
  {
    itab_->set_image_resource(img_);
    return;
  }
  vcl_cout << "In bmvv_cal_manager::load_image_file() - null tableau\n";
}

void bmvv_cal_manager::add_image(vil_image_resource_sptr const& image,
                                 bool greyscale)
{

  if (greyscale)
  {
#if 0// do nothing right now
    vil1_memory_image_of<unsigned char> temp1 =
    brip_vil1_float_ops::convert_to_grey(image);
    img_ = temp1;
#endif 
  }
  else
    img_ = image;

  if (itab_)
  {
    itab_->set_image_resource(img_);
    itab_->post_redraw();
    return;
  }
  vcl_cout << "In bmvv_cal_manager::load_image() - null tableau\n";
}
//=========================================================================
//: load an image an put it in the currently selected grid cell
//=========================================================================
void bmvv_cal_manager::load_image()
{
  static bool greyscale = false;
  vgui_dialog load_image_dlg("Load Image");
  static vcl_string image_filename = "";
  static vcl_string ext = "*.*";
  load_image_dlg.file("Image Filename:", ext, image_filename);
  load_image_dlg.checkbox("greyscale ", greyscale);
  if (!load_image_dlg.ask())
    return;
  vil_image_resource_sptr temp = vil_load_image_resource(image_filename.c_str());
  this->add_image(temp, greyscale);
}

void bmvv_cal_manager::adjust_contrast()
{
  if (!itab_)
    return;
  bgui_range_adjuster_tableau_sptr h= bgui_range_adjuster_tableau_new(itab_);
  h->set_hardware(true);
  h->update();
  vgui_viewer2D_tableau_sptr v = vgui_viewer2D_tableau_new(h);
  vgui_shell_tableau_sptr s = vgui_shell_tableau_new(v);
  //popup adjuster
  vgui_dialog contrast("Histogram Range Adjuster");
  contrast.inline_tableau(s, 280, 200);
  if (!contrast.ask())
    return;
}
//===================================================================
//: Clear the display
//===================================================================
void bmvv_cal_manager::clear_display()
{
  if (btab_)
    btab_->clear_all();
  else
    vcl_cout << "In bmvv_cal_manager::clear_display() - null tableau\n";
}

//===================================================================
//: clear all selections in both panes
//===================================================================
void bmvv_cal_manager::clear_selected()
{
  if(btab_)
    btab_->clear_all();
}

void bmvv_cal_manager::draw_corr_point(const float x, const float y)
{
 if (!btab_)
    return;
 btab_->set_point_radius(5.0f);
 btab_->set_foreground(0.0f, 1.0f, 0.0f);
 btab_->add_point(x, y);
 btab_->post_redraw();
}


void bmvv_cal_manager::load_image_and_camera()
{
  static bool greyscale = false;
  static vcl_string cam_file = "";
  static vcl_string cam_ext = "*.cam";
  static vcl_string image_file = "";
  static vcl_string ext = "*.*";
  vgui_dialog image_cam_dlg("Read Image and Cam");
  image_cam_dlg.file("Image Filename:", ext, image_file);
  image_cam_dlg.file("Camera File", cam_ext, cam_file);
  image_cam_dlg.checkbox("greyscale ", greyscale);
  if (!image_cam_dlg.ask())
    return;
  vil_image_resource_sptr temp = vil_load_image_resource(image_file.c_str());
  this->add_image(temp, greyscale);
  vcl_ifstream cam_istr(cam_file.c_str());
  cam_istr >> cam_;
  vcl_cout << "Read Camera " << cam_ << '\n';
   this->project_world();
}

void bmvv_cal_manager::load_corrs()
{
  vgui_dialog corr_dlg("Load Correspondences");
  static vcl_string corr_file = "";
  static vcl_string corr_ext = "*.cm";
  corr_dlg.file("Correspondence File", corr_ext, corr_file);
  if (!corr_dlg.ask())
    return;
  vcl_ifstream corr_istr(corr_file.c_str());
  this->clear_correspondences();
  world_.clear();
  world_.resize(0);
  if (!brct_algos::read_target_corrs(corr_istr, corrs_valid_, corrs_, world_))
  {
    vcl_cout << "Failed to read correspondences\n";
    return ;
  }
  if (!btab_)
    return;
  btab_->clear_all();
 this->project_world();
 this->draw_correspondences();
}  

void bmvv_cal_manager::save_corrs()
{
 vgui_dialog corr_dlg("Save Correspondences");
  static vcl_string corr_file = "";
  static vcl_string corr_ext = "*.cm";
  corr_dlg.file("Correspondence File", corr_ext, corr_file);
  if (!corr_dlg.ask())
    return;
  vcl_ofstream corr_ostr(corr_file.c_str());
  if (!brct_algos::write_corrs(corr_ostr, corrs_valid_, corrs_, world_))
  {
    vcl_cout << "Failed to write correspondences\n";
    return ;
  }
}

void bmvv_cal_manager::save_camera()
{
  vgui_dialog cam_dlg("Save Camera");
  static vcl_string cam_file = "";
  static vcl_string cam_ext = "*.cam";
  cam_dlg.file("Camera File", cam_ext, cam_file);
  if (!cam_dlg.ask())
    return;
  vcl_ofstream cam_ostr(cam_file.c_str());
  if(!cam_ostr)
    {
      vcl_cout << "Bad camera file\n";
      return;
    }
  cam_ostr << cam_;
  cam_ostr.close();
}

void bmvv_cal_manager::load_world()
{
  vgui_dialog world_dlg("Load World Points3D");
  static vcl_string world_file = "";
  static vcl_string world_ext = "*.wd";
  world_dlg.file("World Point File", world_ext, world_file);
  if (!world_dlg.ask())
    return;
  vcl_ifstream world_istr(world_file.c_str());
  this->clear_correspondences();
  world_.clear();
  world_.resize(0);
  if (!brct_algos::read_world_points(world_istr, world_))
  {
    vcl_cout << "Failed to read world points\n";
    return ;
  }
  if (!btab_)
    return;
  btab_->clear();
  //make sure the correspondence point array is the same size as the world
  int n = world_.size();
  corrs_.resize(n, vgl_point_2d<double>(0,0));
  corr_sovs_.resize(n, (vgui_soview2D_point*)0);
  corrs_valid_.resize(n, false);
  this->project_world();
}

bool bmvv_cal_manager::draw_correspondences()
{
  if (!btab_)
    return false;
  btab_->set_point_radius(5.0f);
  btab_->set_foreground(0.0f, 1.0f, 1.0f);
  unsigned i = 0;
  for (vcl_vector<vgl_point_2d<double> >::iterator pit = corrs_.begin();
       pit != corrs_.end(); ++pit, ++i)
    if(corrs_valid_[i])
      corr_sovs_.push_back(btab_->add_point((*pit).x(), (*pit).y()));
    else
      corr_sovs_.push_back((vgui_soview2D_point*)0);
  btab_->post_redraw();
  return true;
}  
void bmvv_cal_manager::clear_correspondences()
{
  if (!btab_)
    return;
  btab_->clear_all();
  corrs_.clear();
  corr_sovs_.clear();
  corrs_valid_.clear();
  corrs_.resize(0);
  corrs_valid_.resize(0);
  corr_sovs_.resize(0);
}

bool bmvv_cal_manager::draw_projected_world_points()
{
  if (!btab_)
    return false;
  int i = 0;
  point_3d_map_.clear();
  btab_->set_point_radius(5.0f);
  btab_->set_foreground(1.0f, 1.0f, 0.2f);
  for (vcl_vector<vgl_point_2d<double> >::iterator pit = proj_image_pts_.begin();      pit != proj_image_pts_.end(); pit++, i++)
    {
      vgui_soview2D_point* sov = 
        btab_->add_point((*pit).x(), (*pit).y());
      int id = sov->get_id();
      point_3d_map_[id]=i;
    }
  btab_->post_redraw();
  return true;
}

int  bmvv_cal_manager::get_selected_proj_world_pt(vgl_point_2d<double>&  pt)
{
   
  if (!btab_)
    return  -1;
  vcl_vector<unsigned> ids = btab_->get_selected();
  //take the last selected
  int n = ids.size();
  if (!n)
  {
    vcl_cout << "Nothing selected\n";
    return -1;
  }
  int i = point_3d_map_[ids[n-1]];
  if(i<proj_image_pts_.size())
    {
      pt =  proj_image_pts_[i];
	  return i;
    }
  vcl_cout << "Bogus correspondence\n";
  return -1;
}

void bmvv_cal_manager::pick_correspondence()
{
  if(!world_.size())
    {
      vcl_cout << "Can't pick correspondence - null 3D world\n";
      return;
    }
  vgl_point_2d<double> pt;
  int id = this->get_selected_proj_world_pt(pt);
	if(id<0)
    return;
  vcl_cout << "Projected world point " << pt << '\n';
  if (!ptab_)
    {
      vcl_cout << "Can't pick correspondence - null picker tableau\n";
      return;
    }
  ptab_->set_color(0.0f, 1.0f, 1.0f);
  ptab_->set_line_width(3.0f);
  float x, y;
  ptab_->anchored_pick_point(pt.x(), pt.y(), &x, &y);
  corrs_[id].set(x, y);
  corrs_valid_[id]=true;
  ptab_->set_color();//restore defaults
  ptab_->set_line_width();

  if (!btab_)
    return;
  btab_->set_foreground(0.0f, 1.0f, 1.0f);
  btab_->set_point_radius(5.0);
  if(!corr_sovs_[id])
    corr_sovs_[id]=btab_->add_point(x, y);
  else {
    corr_sovs_[id]->x = x;
    corr_sovs_[id]->y = y;
  }
  btab_->post_redraw();
}

void bmvv_cal_manager::remove_correspondence()
{
  if(!world_.size())
    {
      vcl_cout << "Can't remove correspondence - null 3D world\n";
      return;
    }
  vgl_point_2d<double> pt;
  int id = this->get_selected_proj_world_pt(pt);
	if(id<0)
    return;
  corrs_[id].set(0, 0);
  corrs_valid_[id]=false;
  corr_sovs_[id]=(vgui_soview2D_point*)0;
  if (!btab_)
    {
      vcl_cout << "Correspondence removal aborted in progress- null btab\n";
      return;
    }
  this->project_world();
  this->draw_correspondences();
}

void bmvv_cal_manager::project_world()
{
  if (!btab_)
    {
      vcl_cout << "Can't project world - null btab\n";
      return;
    }
	btab_->clear_all();
  brct_algos::project(world_, cam_, proj_image_pts_);
  this->draw_projected_world_points();
  this->draw_correspondences();
}

void bmvv_cal_manager::solve_camera()
{
  static bool planar = false;
  static bool optimize = true;
  static bool restore = false;
  vgui_dialog read_homg_dlg("Solve Camera");
  read_homg_dlg.checkbox("Planar Camera Only? ", planar);
  read_homg_dlg.checkbox("Optimize? ", optimize);
  read_homg_dlg.checkbox("Restore Previous Camera? ", restore);
  if (!read_homg_dlg.ask())
    return;
  if(restore)
    {
      cam_=prev_cam_;
      return;
    }
  vcl_vector<double> image_y;
  vcl_vector< vgl_point_2d<double> > zero_Z_image_corr;
  vcl_vector< vgl_point_3d<double> > zero_Z_pts;
  vcl_vector< vgl_point_3d<double> > non_zero_Z_pts;
  int n = world_.size();
  for(unsigned i = 0; i<n; i++)
    {
      if(!corrs_valid_[i])
        continue;
      if(vcl_fabs(world_[i].z())<0.01)//planar points
        {
          zero_Z_pts.push_back(world_[i]);
          zero_Z_image_corr.push_back(corrs_[i]);
        }
      else // non-planar point for camera elevation constraint
        {
          image_y.push_back(corrs_[i].y());
          non_zero_Z_pts.push_back(world_[i]);
        }
    }
  vgl_h_matrix_2d<double> H;
  if(!brct_algos::homography(zero_Z_pts, zero_Z_image_corr, H, optimize))
    {
      vcl_cout << "Linear SVD solve for homography failed \n";
      return;
    }
  //protect from bad solution by saving old camera
  prev_cam_ = cam_;
  if(!planar)
    cam_= brct_algos::p_from_h(H,image_y, non_zero_Z_pts);
  else
    cam_= brct_algos::p_from_h(H);

  if (!btab_)
    return;
  btab_->clear_all();
 this->project_world();
 this->draw_correspondences();
}
  




void bmvv_cal_manager::
draw_vsol_points(const int cam, vcl_vector<vsol_point_2d_sptr> const & points,
                 bool clear, const float r, const float g, const float b)
{
  if (!btab_)
  {
    vcl_cout << "In bmvv_cal_manager::draw_vsol_points(..) -"
             << " null btol tableau for pane " << cam << vcl_endl;
    return;
  }
  if (clear)
    btab_->clear_all();
  vgui_style_sptr style = vgui_style::new_style(r, g, b, 3, 0);
  for (vcl_vector<vsol_point_2d_sptr>::const_iterator pit = points.begin();
      pit != points.end(); pit++)
    btab_->add_vsol_point_2d(*pit, style);
}

void bmvv_cal_manager::
draw_vsol_point(const int cam, vsol_point_2d_sptr const & point,
                bool clear, const float r, const float g, const float b)
{
  if (!btab_)
  {
    vcl_cout << "In bmvv_cal_manager::draw_vsol_point(..) -"
             << " null btol tableau for pane " << cam << vcl_endl;
    return;
  }
  if (clear)
    btab_->clear_all();
  vgui_style_sptr style = vgui_style::new_style(r, g, b, 3, 0);
  btab_->add_vsol_point_2d(point,style);
}







void bmvv_cal_manager::compute_ransac_homography()
{
  vgui_dialog read_homg_dlg("Read Correspondence File");
  static vcl_string corr_filename = "";
  static vcl_string corr_ext = "*.cm";
  static vcl_string cam_filename = "";
  static vcl_string cam_ext = "*.cam";
  static bool show_projected_points = true;
  static bool optimize = true;
  vcl_vector<vcl_string> choices;
  static int choice=0;
  choices.push_back("Linear SVD");
  choices.push_back("RANSAC");
  choices.push_back("MUSE");
  read_homg_dlg.file("TargetJr correspondence file", corr_ext, corr_filename);
  read_homg_dlg.file("TargetJr camera file", cam_ext, cam_filename);
  read_homg_dlg.checkbox("Show Projected Points ",show_projected_points);
  read_homg_dlg.choice("Homography Algo.", choices, choice);
  read_homg_dlg.checkbox("Optimize? ", optimize);
  if (!read_homg_dlg.ask())
    return;
  if (corr_filename == ""||cam_filename == "")
  {
    vcl_cout << "Need a file name\n";
    return;
  }
 
  this->clear_display();
  vcl_ifstream istr(corr_filename.c_str());
  vcl_ofstream ostr(cam_filename.c_str());
   vcl_string target_file = cam_filename + "-target";
  vcl_ofstream ostr_tar(target_file.c_str());
vcl_vector<bool> valid;
  vcl_vector<vgl_point_2d<double> > image_points;
  vcl_vector<vgl_point_3d<double> > world_points;
  if (!brct_algos::read_target_corrs(istr, valid, image_points, world_points))
  {
    vcl_cout << "Failed to read correspondences\n";
    return;
  }
  bool show = show_projected_points;
  // compute the homography using ransac  
  vgl_h_matrix_2d<double> H;
  switch (choice)
    {
    case 0:
      if(!brct_algos::homography(world_points, image_points, H, optimize))
        show = false;
      break;
    case 1:
      if(!brct_algos::homography_ransac(world_points, image_points, H, optimize))
        show = false;
      break;
    case 2:
      if(!brct_algos::homography_muse(world_points, image_points, H, optimize))
        show = false;
      break;
    }
  vcl_vector<vgl_point_2d<double> > proj_image_points;
  vgl_p_matrix<double> P =  brct_algos::p_from_h(H);
  brct_algos::project(world_points, P, proj_image_points);

if(!btab_)
	return;
  vgui_style_sptr r_style = vgui_style::new_style(1.0, 0.0, 0.0, 5, 0);
  vgui_style_sptr g_style = vgui_style::new_style(0.0, 1.0, 0.0, 5, 0);
  for (unsigned int i = 0; i<world_points.size(); i++)
  {
    vgl_point_2d<double> ip = image_points[i];
    vgl_point_2d<double> pip = proj_image_points[i];
    vsol_point_2d_sptr vip = new vsol_point_2d(ip.x(),ip.y());
    vsol_point_2d_sptr vpip = new vsol_point_2d(pip.x(),pip.y());
    vcl_cout << ip << " == " << pip << vcl_endl;
    btab_->add_vsol_point_2d(vip,r_style);
    if(show)btab_->add_vsol_point_2d(vpip,g_style);
  }
  btab_->post_redraw();

  //write out the camera
  ostr << P;
  //Also as a target camera for interest sake
  brct_algos::write_target_camera(ostr_tar, P.get_matrix());
}
