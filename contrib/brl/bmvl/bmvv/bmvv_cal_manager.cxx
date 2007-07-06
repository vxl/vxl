// This is brl/bmvl/bmvv/bmvv_cal_manager.cxx
#include "bmvv_cal_manager.h"
//:
// \file
// \author J.L. Mundy

#include <vcl_iostream.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_math.h>
#include <vil/vil_new.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vgl/algo/vgl_p_matrix.h>
#include <vgui/vgui.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_macro.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_range_map_params.h>
#include <vgui/vgui_projection_inspector.h>
#include <bgui/bgui_image_tableau.h>
#include <bgui/bgui_range_adjuster_tableau.h>
#include <bgui/bgui_vsol_soview2D.h>
#include <bgui/bgui_vtol2D_tableau.h>
#include <bgui/bgui_picker_tableau.h>
#include <brip/brip_vil_float_ops.h>
#include <brct/brct_algos.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_3d.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/algo/vpgl_camera_compute.h>
#include <vpgl/algo/vpgl_optimize_camera.h>
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
  delete instance_;
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
  height_ = 0.0;
}

//=========================================================================
//: make an event handler
// note that we have to get an adaptor and set the tableau to receive events
// this handler does nothing but is a place holder for future event processing
// For now, just pass the events down to the child tableaux
//=========================================================================
bool bmvv_cal_manager::handle(const vgui_event &e)
{
  vgui_projection_inspector proj;
  if (e.type == vgui_KEY_PRESS)
  {
    if (btab_)
      if (e.key == 'u')
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
  vgui::quit();
}

//=========================================================================
//: load an image an put it in the currently selected grid cell
//=========================================================================
void bmvv_cal_manager::load_image_file(vcl_string image_filename, bool /* greyscale */)
{
  //add greyscale later
  img_ = vil_load_image_resource(image_filename.c_str());
  if (!img_)
  {
    vcl_cout << "Null image resource - couldn't load from "
             << image_filename << '\n';
    return;
  }
  vgui_range_map_params_sptr rmps = range_params(img_);
  if (itab_)
  {
    itab_->set_image_resource(img_, rmps);
    return;
  }
  vcl_cout << "In bmvv_cal_manager::load_image_file() - null tableau\n";
}

void bmvv_cal_manager::add_image(vil_image_resource_sptr const& image,
                                 vgui_range_map_params_sptr rmps,
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
    itab_->set_image_resource(img_, rmps);
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
  vgui_range_map_params_sptr rmps = range_params(temp);
  this->add_image(temp, rmps, greyscale);
}

void bmvv_cal_manager::save_image()
{
  vgui_dialog file_dialog("Save Image");
  static vcl_string image_file;
  static vcl_string ext = "tif";
  static vcl_string type = "tiff";
  static bool byte = false;
  file_dialog.file("Image Filename:", ext, image_file);
  file_dialog.field("Image Format: ", type);
  file_dialog.checkbox("Convert to byte image", byte);
  if (!file_dialog.ask())
    return;
  vil_image_resource_sptr img = itab_->get_image_resource();
  if (!img)
  {
    vcl_cerr << "Null image in bmvv_cal_manager::save_image\n";
    return;
  }
  vil_image_resource_sptr save_image = img;
  if (byte)
  {
    vil_image_view<unsigned char> byte_view = brip_vil_float_ops::convert_to_byte(img);
    save_image = vil_new_image_resource_of_view(byte_view);
  }
  if (!vil_save_image_resource(save_image, image_file.c_str(), type.c_str() ))
    vcl_cerr << "bmvv_cal_manager::save_image operation failed\n";
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
  if (btab_)
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


//: Calculate the range parameters for the input image
vgui_range_map_params_sptr bmvv_cal_manager::
range_params(vil_image_resource_sptr const& image)
{
  float gamma = 1.0;
  bool invert = false;
  bool gl_map = false;
  bool cache = true;
  unsigned min = 0, max = 255;
  if (image->pixel_format()==VIL_PIXEL_FORMAT_BYTE)
  {
    vil_image_view<unsigned char> temp = image->get_view();
    unsigned char vmin=0, vmax=255;
    vil_math_value_range<unsigned char>(temp, vmin, vmax);
    min = static_cast<unsigned>(vmin);
    max = static_cast<unsigned>(vmax);
    return  new vgui_range_map_params(min, max, gamma, invert,
                                      gl_map, cache);
  }
  if (image->pixel_format()==VIL_PIXEL_FORMAT_UINT_16)
  {
    vil_image_view<unsigned short> temp = image->get_view();
    unsigned short vmin=0, vmax=60000;
    vil_math_value_range<unsigned short>(temp, vmin, vmax);
    min = static_cast<unsigned>(vmin);
    max = static_cast<unsigned>(vmax);
    gl_map = true;
    return  new vgui_range_map_params(min, max, gamma, invert,
                                      gl_map, cache);
  }
vcl_cout << "Image pixel format not handled\n";
 return new vgui_range_map_params(0, 255, gamma, invert,
                                      gl_map, cache);
}

void bmvv_cal_manager::set_range_params()
{
  if (!itab_||!itab_->get_image_resource())
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
  vil_image_resource_sptr img = itab_->get_image_resource();
  unsigned n_components = img->nplanes();
  vgui_range_map_params_sptr rmps;
  if (n_components == 1)
    rmps= new vgui_range_map_params(min, max, gamma, invert,
                                    gl_map, cache);
  else if (n_components == 3)
    rmps = new vgui_range_map_params(min, max, min, max, min, max,
                                     gamma, gamma, gamma, invert,
                                     gl_map, cache);
  else
    rmps = 0;
  itab_->set_mapping(rmps);
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
  vgui_range_map_params_sptr rmps = range_params(temp);
  this->add_image(temp, rmps, greyscale);
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
  //  this->draw_correspondences();
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
  static bool target = false;
  cam_dlg.file("Camera File", cam_ext, cam_file);
  cam_dlg.checkbox("Target?", target);
  if (!cam_dlg.ask())
    return;
  vcl_ofstream cam_ostr(cam_file.c_str());
  if (!cam_ostr)
  {
    vcl_cout << "Bad camera file\n";
    return;
  }
  if (!target){
  cam_ostr << cam_;
  cam_ostr.close();
  return;}
  brct_algos::write_target_camera(cam_ostr, cam_.get_matrix());
}

void bmvv_cal_manager::set_identity_camera()
{
  vnl_matrix_fixed<double,3,4> M;
  M[0][0] = 1.0;   M[0][1] = 0.0; M[0][2] = 0.0; M[0][3] = 0.0;
  M[1][0] = 0.0;   M[1][1] = 1.0; M[1][2] = 0.0; M[1][3] = 0.0;
  M[2][0] = 0.0;   M[2][1] = 0.0; M[2][2] = 0.0; M[2][3] = 1.0;
  cam_ = vgl_p_matrix<double>(M);
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
  this->clear_world();
  if (!brct_algos::read_world(world_istr, world_, polys_, indexed_face_set_))
  {
    vcl_cout << "Failed to read world\n";
    return ;
  }
  //add polygons
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

void bmvv_cal_manager::save_world()
{
  vgui_dialog world_dlg("Save World");
  static vcl_string world_file = "";
  static vcl_string world_ext = "*.wd";
  world_dlg.file("World File", world_ext, world_file);
  if (!world_dlg.ask())
    return;
  vcl_ofstream world_ostr(world_file.c_str());
  brct_algos::write_world(world_ostr, world_, indexed_face_set_);
}

void bmvv_cal_manager::save_world_ply2()
{
  vgui_dialog world_dlg("Save World Ply2");
  static vcl_string world_file = "";
  static vcl_string world_ext = "*.ply2";
  world_dlg.file("Ply2 File", world_ext, world_file);
  if (!world_dlg.ask())
    return;
  vcl_ofstream world_ostr(world_file.c_str());
  brct_algos::write_world_ply2(world_ostr, world_, indexed_face_set_);
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
    if (corrs_valid_[i])
      corr_sovs_.push_back(btab_->add_point((*pit).x(), (*pit).y()));
    else
      corr_sovs_.push_back((vgui_soview2D_point*)0);
  btab_->post_redraw();
  return true;
}

void bmvv_cal_manager::read_world_ply2()
{
  vgui_dialog world_dlg("Read World Ply2");
  static vcl_string world_file = "";
  static vcl_string world_ext = "*.ply2";
  world_dlg.file("Ply2 File", world_ext, world_file);
  if (!world_dlg.ask())
    return;
  vcl_ifstream world_istr(world_file.c_str());
  brct_algos::read_world_ply2(world_istr, world_, polys_, indexed_face_set_);
  this->project_world();
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
  for (vcl_vector<vgl_point_2d<double> >::iterator pit = proj_image_pts_.begin();
       pit != proj_image_pts_.end(); pit++, ++i)
  {
    vgui_soview2D_point* sov = btab_->add_point((*pit).x(), (*pit).y());
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
  unsigned int i = point_3d_map_[ids[n-1]];
  if (i<proj_image_pts_.size())
  {
    pt =  proj_image_pts_[i];
    return i;
  }
  vcl_cout << "Bogus correspondence\n";
  return -1;
}

void bmvv_cal_manager::pick_correspondence()
{
  if (!world_.size())
  {
    vcl_cout << "Can't pick correspondence - null 3D world\n";
    return;
  }
  vgl_point_2d<double> pt;
  int id = this->get_selected_proj_world_pt(pt);
  if (id<0)
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
  if (!corr_sovs_[id])
    corr_sovs_[id]=btab_->add_point(x, y);
  else {
    corr_sovs_[id]->x = x;
    corr_sovs_[id]->y = y;
  }
  btab_->post_redraw();
}

void bmvv_cal_manager::remove_correspondence()
{
  if (!world_.size())
  {
    vcl_cout << "Can't remove correspondence - null 3D world\n";
    return;
  }
  vgl_point_2d<double> pt;
  int id = this->get_selected_proj_world_pt(pt);
  if (id<0)
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
  //  this->draw_correspondences();
}

void bmvv_cal_manager::pick_vertical()
{
  ptab_->set_color(0.0f, 1.0f, 0.0f);
  ptab_->set_line_width(3.0f);
  float x1, y1,x2, y2;
  ptab_->pick_line(&x1, &y1, &x2, &y2);
  vgl_point_2d<double> p1(x1, y1), p2(x2,y2);
  vgl_line_segment_2d<double> vertical(p1, p2);
  verticals_.push_back(vertical);
  ptab_->set_color();//restore defaults
  ptab_->set_line_width();
  if (!btab_)
    return;
  btab_->set_foreground(0.0f, 1.0f, 0.0f);
  btab_->add_line(x1,y1,x2,y2);
  btab_->post_redraw();
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
  this->draw_polygons_3d(false, 0.0f, 1.0f, 0.0f);
  this->draw_correspondences();
}

void bmvv_cal_manager::solve_camera()
{
  static bool planar = false;
  static bool deg_planar = false;
  static bool optimize = true;
  static bool restore = false;
  vgui_dialog read_homg_dlg("Solve Camera");
  read_homg_dlg.checkbox("Planar Camera Only? ", planar);
  read_homg_dlg.checkbox("Mostly Planar Camera? ", deg_planar);
  read_homg_dlg.checkbox("Optimize? ", optimize);
  read_homg_dlg.checkbox("Restore Previous Camera? ", restore);
  if (!read_homg_dlg.ask())
    return;
  if (restore)
  {
    cam_=prev_cam_;
    return;
  }

  if(planar || deg_planar){
    vcl_vector<double> image_y;
    vcl_vector< vgl_point_2d<double> > zero_Z_image_corr;
    vcl_vector< vgl_point_3d<double> > zero_Z_pts;
    vcl_vector< vgl_point_3d<double> > non_zero_Z_pts;
    unsigned int n = world_.size();
    for (unsigned i = 0; i<n; i++)
    {
      if (!corrs_valid_[i])
        continue;
      if (vcl_fabs(world_[i].z()) < 0.01) //planar points
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
    if (!brct_algos::homography(zero_Z_pts, zero_Z_image_corr, H, optimize))
    {
      vcl_cout << "Linear SVD solve for homography failed\n";
      return;
    }
    //protect from bad solution by saving old camera
    prev_cam_ = cam_;
    if (!planar)
      cam_= brct_algos::p_from_h(H,image_y, non_zero_Z_pts);
    else
      cam_= brct_algos::p_from_h(H);
  }
  else{
    vcl_vector< vgl_homg_point_2d<double> > image_hpts;
    vcl_vector< vgl_homg_point_3d<double> > world_hpts;
    unsigned int n = world_.size();
    for (unsigned i = 0; i<n; i++)
    {
      if (!corrs_valid_[i])
        continue;
      image_hpts.push_back(vgl_homg_point_2d<double>(corrs_[i]));
      world_hpts.push_back(vgl_homg_point_3d<double>(world_[i]));
    }
    vpgl_proj_camera<double> camera;
    vpgl_proj_camera_compute().compute(image_hpts,world_hpts,camera);
    prev_cam_ = cam_;
    cam_ = vgl_p_matrix<double>(camera.get_matrix());
    
    vpgl_perspective_camera<double> p_camera;
    if(optimize && vpgl_perspective_decomposition(camera.get_matrix(),p_camera)){
      vcl_vector< vgl_point_2d<double> > image_pts;
      for (unsigned i = 0; i<image_hpts.size(); i++){
        image_pts.push_back(image_hpts[i]);
      }
      p_camera = vpgl_optimize_camera::opt_orient_pos_cal(p_camera, world_hpts, image_pts);
      cam_ = vgl_p_matrix<double>(p_camera.get_matrix());
    }
  }

  if (!btab_)
    return;
  btab_->clear_all();
  this->project_world();
  //this->draw_correspondences();
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
    if (!brct_algos::homography(world_points, image_points, H, optimize))
      show = false;
    break;
   case 1:
    if (!brct_algos::homography_ransac(world_points, image_points, H, optimize))
      show = false;
    break;
   case 2:
    if (!brct_algos::homography_muse(world_points, image_points, H, optimize))
      show = false;
    break;
  }
  vcl_vector<vgl_point_2d<double> > proj_image_points;
  vgl_p_matrix<double> P =  brct_algos::p_from_h(H);
  brct_algos::project(world_points, P, proj_image_points);

  if (!btab_)
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
    if (show) btab_->add_vsol_point_2d(vpip,g_style);
  }
  btab_->post_redraw();

  //write out the camera
  ostr << P;
  //Also as a target camera for interest sake
  brct_algos::write_target_camera(ostr_tar, P.get_matrix());
}

//map the image back to cartesian world coordinates
void bmvv_cal_manager::map_image_to_world()
{
  vil_image_resource_sptr img = itab_->get_image_resource();
  if (!img)
    return;
  vil_image_resource_sptr world_image =
    brct_algos::map_image_to_world(img, cam_);
  if (world_image)
  {
    vgui_range_map_params_sptr rmps = range_params(world_image);
    this->add_image(world_image, rmps);
  }
}

void bmvv_cal_manager::save_constraints()
{
  vgui_dialog cnst_dlg("Save Constraints");
  static vcl_string con_file = "";
  static vcl_string con_ext = "*.con";
  cnst_dlg.file("Constraint File", con_ext, con_file);
  if (!cnst_dlg.ask())
    return;
  vcl_ofstream con_ostr(con_file.c_str());
  if (!con_ostr)
  {
    vcl_cout << "Bad constraint file\n";
    return;
  }
  if (!brct_algos::save_constraint_file(corrs_, corrs_valid_, world_, verticals_, con_ostr))
  {
    vcl_cout << "Couldn't save constraints\n";
    return;
  }
}

// add polygon vertices as world points to enable correspondence for
// additional camera constraints.
void bmvv_cal_manager::add_poly_vertices_to_world_pts(vsol_polygon_3d_sptr const& poly)
{
  unsigned nv = poly->size();
  vcl_vector<unsigned> vert_indices(nv);
  for (unsigned i = 0; i<nv; ++i)
  {
    vsol_point_3d_sptr v3d = poly->vertex(i);
    vgl_point_3d<double> p(v3d->x(), v3d->y(), v3d->z());
    world_.push_back(p);
    vert_indices[i]=world_.size()-1;
  }
  indexed_face_set_.push_back(vert_indices);
}

// Add the most recently created polygon to the world
void bmvv_cal_manager::confirm_polygon()
{
  if (!new_poly_)
    return;
  add_poly_vertices_to_world_pts(new_poly_);
  polys_.push_back(new_poly_);
  this->project_world();
}

void bmvv_cal_manager::pick_polygon()
{
  vsol_polygon_2d_sptr poly;
  ptab_->pick_polygon(poly);
  new_poly_ = brct_algos::back_project(poly, cam_, height_);
  this->draw_vsol_polygon_3d(new_poly_, false, 1.0f, 0.0f, 1.0f);
  btab_->post_redraw();
}

void bmvv_cal_manager::draw_vsol_polygon_3d(vsol_polygon_3d_sptr const & poly,
                                            bool clear,
                                            const float r,
                                            const float g,
                                            const float b)
{
  if (!btab_)
  {
    vcl_cerr << "In bmvv_cal_manager::draw_vsol_polygon() - null btol tableau\n";
    return;
  }
  vsol_polygon_2d_sptr poly_2d = brct_algos::project(poly,cam_);
  vgui_style_sptr style = vgui_style::new_style(r, g, b, 1, 3);
 bgui_vsol_soview2D_polygon* sov =  btab_->add_vsol_polygon_2d(poly_2d,style);
 sov->set_selectable(false);
}

void bmvv_cal_manager::draw_polygons_3d(bool clear,
                                        const float r,
                                        const float g,
                                        const float b)
{
  if (clear)
    btab_->clear_all();
  for (vcl_vector<vsol_polygon_3d_sptr>::iterator pit = polys_.begin();
       pit != polys_.end(); ++pit)
    this->draw_vsol_polygon_3d(*pit, false, r, g, b);
}

//Create a box in the world from three top face corner points
void bmvv_cal_manager::create_box()
{
  //the polygon must have exactly 3 vertices
  vsol_polygon_2d_sptr poly;
  ptab_->pick_polygon(poly);
  if (!poly||poly->size()!=3)
  {
    vcl_cout << "bad box corner pick\n";
    return;
  }
  vsol_polygon_3d_sptr top = brct_algos::back_project(poly, cam_, height_);
  vsol_point_3d_sptr p0=top->vertex(0), p1=top->vertex(1), p2=top->vertex(2);
  vgl_point_3d<double> c0(p0->x(), p0->y(), p0->z());
  vgl_point_3d<double> c1(p1->x(), p1->y(), p1->z());
  vgl_point_3d<double> c2(p2->x(), p2->y(), p2->z());

  vcl_vector<vgl_point_3d<double> > points;
  vcl_vector<vsol_polygon_3d_sptr> polys;
  vcl_vector<vcl_vector<unsigned> > indexed_face_set;
  brct_algos::box_3d(c0, c1, c2, points, polys, indexed_face_set);

  unsigned vi0 = world_.size();//starting index for new points
  for (vcl_vector<vgl_point_3d<double> >::iterator pt = points.begin();
       pt != points.end(); ++pt)
    world_.push_back(*pt);

  for (vcl_vector<vsol_polygon_3d_sptr>::iterator pl = polys.begin();
       pl != polys.end(); ++pl)
    polys_.push_back(*pl);

  for (vcl_vector<vcl_vector<unsigned> >::iterator ifs=indexed_face_set.begin();
       ifs != indexed_face_set.end(); ++ifs)
  {
    vcl_vector<unsigned> vi = *ifs;
    vcl_vector<unsigned> index(4);
    for (unsigned i = 0; i<4; ++i)
      index[i] = vi[i]+vi0;
    indexed_face_set_.push_back(index);
  }
  this->project_world();
}

//Convert boxes in a merged indexed face set to individual boxes in VRML
void bmvv_cal_manager::ply2_to_vrml()
{
  static float r = 0.0, g = 0.8f, b = 0.2f;
  vgui_dialog trans_dlg("Ply2 to VRML2.0");
  static vcl_string ply2_file = "";
  static vcl_string ply2_ext = "*.ply2";
  trans_dlg.file("Ply2 File", ply2_ext, ply2_file);
  static vcl_string vrml_file = "";
  static vcl_string vrml_ext = "*.wrl";
  trans_dlg.file("VRML File", vrml_ext, vrml_file);
  trans_dlg.field("r ", r);
  trans_dlg.field("g ", g);
  trans_dlg.field("b ", b);
  if (!trans_dlg.ask())
    return;
  vcl_ifstream istr(ply2_file.c_str());
  vcl_ofstream ostr(vrml_file.c_str());
  if (!brct_algos::translate_ply2_to_vrml(istr, ostr, r, g, b))
    vcl_cout << "Convert Ply to VRML Failed\n";
}

//Set the height of the X-Y create plane
void bmvv_cal_manager::set_height()
{
  static double height = 0.0;
  vgui_dialog box_dlg("Create Plane Z");
  box_dlg.field("Plane Z", height);
  if (!box_dlg.ask())
    return;
  height_ = height;
}

void bmvv_cal_manager::clear_world()
{
  world_.clear();
  world_.resize(0);
  polys_.clear();
  polys_.resize(0);
  indexed_face_set_.clear();
  indexed_face_set_.resize(0);
  this->project_world();
}
