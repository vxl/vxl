// This is brl/bmvl/bmvv/bmvv_recon_manager.cxx
#include "bmvv_recon_manager.h"
//:
// \file
// \author J.L. Mundy

#include <vcl_cstdlib.h> // for vcl_exit()
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vil1/vil1_load.h>
#include <vil1/vil1_memory_image_of.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_math.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vgl/algo/vgl_p_matrix.h>
#include <vgui/vgui.h>
#include <vgui/vgui_find.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_macro.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_soview2D.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <bgui/bgui_image_tableau.h>
#include <bgui/bgui_vtol2D_tableau.h>
#include <bgui/bgui_picker_tableau.h>
#include <brip/brip_vil1_float_ops.h>
#include <brct/brct_plane_sweeper_params.h>
#include <brct/brct_volume_processor_params.h>
#include <brct/brct_dense_reconstructor.h>
#include <brct/brct_algos.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_3d.h>
//static live_video_manager instance
bmvv_recon_manager *bmvv_recon_manager::instance_ = 0;
//===============================================================
//: The singleton pattern - only one instance of the manager can occur
//==============================================================
bmvv_recon_manager *bmvv_recon_manager::instance()
{
  if (!instance_)
  {
    instance_ = new bmvv_recon_manager();
    instance_->init();
  }
  return bmvv_recon_manager::instance_;
}

//==================================================================
//: constructors/destructor
//==================================================================
bmvv_recon_manager::
bmvv_recon_manager() : vgui_wrapper_tableau(),
                       sweep_(brct_plane_sweeper_params()),
                       vproc_(brct_volume_processor_params())
{
  images_set_ = false;
  harris_set_ = false;
  plane_ = 0;
}

bmvv_recon_manager::~bmvv_recon_manager()
{
}

//======================================================================
//: set up the tableaux at each grid cell
//  the vtol2D_tableaux have been initialized in the constructor
//======================================================================
void bmvv_recon_manager::init()
{
  grid_ = vgui_grid_tableau_new(2,1);
  grid_->set_grid_size_changeable(true);
  for (unsigned int col=0, row=0; col<2; ++col)
  {
    vgui_image_tableau_sptr itab = bgui_image_tableau_new();
    bgui_vtol2D_tableau_sptr btab = bgui_vtol2D_tableau_new(itab);
    vtol_tabs_.push_back(btab);
    bgui_picker_tableau_new pcktab(btab);
    vgui_viewer2D_tableau_sptr v2d = vgui_viewer2D_tableau_new(pcktab);
    grid_->add_at(v2d, col, row);
  }
  vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(grid_);
  this->add_child(shell);
}

//=========================================================================
//: make an event handler
// note that we have to get an adaptor and set the tableau to receive events
// this handler does nothing but is a place holder for future event processing
// For now, just pass the events down to the child tableaux
//=========================================================================
bool bmvv_recon_manager::handle(const vgui_event &e)
{
  return this->child.handle(e);
}

//====================================================================
//: Gets the picker tableau at the given position.
//  Returns a null tableau if it fails
//====================================================================
bgui_picker_tableau_sptr
bmvv_recon_manager::get_picker_tableau_at(unsigned col, unsigned row)
{
  vgui_tableau_sptr top_tab = grid_->get_tableau_at(col, row);
  if (top_tab)
  {
    bgui_picker_tableau_sptr tt;
    tt.vertical_cast(vgui_find_below_by_type_name(top_tab,
                                                  vcl_string("bgui_picker_tableau")));
    if (tt)
      return tt;
  }
  vgui_macro_warning << "Unable to get bgui_picker_tableau at (" << col << ", "
                     << row << ")\n";
  return 0;
}

//====================================================================
//: Gets the picker tableau at the currently selected grid position
//  Returns a null tableau if it fails
//====================================================================
bgui_picker_tableau_sptr bmvv_recon_manager::get_selected_picker_tableau()
{
  unsigned int row=0, col=0;
  grid_->get_last_selected_position(&col, &row);
  return this->get_picker_tableau_at(col, row);
}

//====================================================================
//: Gets the picker tableau at the given position.
//  if col, row are out of bounds then null is returned
//  row is currently not used but may be when we have more than 2 cameras
//====================================================================
bgui_vtol2D_tableau_sptr bmvv_recon_manager::get_vtol2D_tableau_at(unsigned col, unsigned row)
{
  if (row!=0)
    return 0;
  bgui_vtol2D_tableau_sptr btab = 0;
  if (col==0||col==1)
    btab = vtol_tabs_[col];
  return btab;
}


//=================================================================
//: Get the vtol_2D_tableau at the currently selected grid cell.
//=================================================================
bgui_vtol2D_tableau_sptr bmvv_recon_manager::get_selected_vtol2D_tableau()
{
  unsigned int row =0, col=0;
  grid_->get_last_selected_position(&col, &row);
  return this->get_vtol2D_tableau_at(col, row);
}

int bmvv_recon_manager::get_cam()
{
  unsigned int row =0, col=0;
  grid_->get_last_selected_position(&col, &row);
  if (!row&&!col)
    return 0;
  return 1;
}

void bmvv_recon_manager::quit()
{
  vcl_exit(1);
}

//=========================================================================
//: load an image an put it in the currently selected grid cell
//=========================================================================
void bmvv_recon_manager::load_image_file(vcl_string image_filename, bool /* greyscale */, unsigned col, unsigned row)
{
  img_ = vil1_load(image_filename.c_str());
  bgui_vtol2D_tableau_sptr btab = this->get_vtol2D_tableau_at(col, row);
  if (btab)
  {
    vgui_image_tableau_sptr itab = btab->get_image_tableau();
    itab->set_image(img_);
    return;
  }
  vcl_cout << "In bmvv_recon_manager::load_image_file() - null tableau\n";
}

//=========================================================================
//: load an image an put it in the currently selected grid cell
//=========================================================================
void bmvv_recon_manager::load_image()
{
  static bool greyscale = false;
  vgui_dialog load_image_dlg("Load Image");
  static vcl_string image_filename = "";
  static vcl_string ext = "*.*";
  load_image_dlg.file("Image Filename:", ext, image_filename);
  load_image_dlg.checkbox("greyscale ", greyscale);
  if (!load_image_dlg.ask())
    return;
  vil1_image temp = vil1_load(image_filename.c_str());
  if (greyscale)
  {
    vil1_memory_image_of<unsigned char> temp1 =
    brip_vil1_float_ops::convert_to_grey(temp);
    img_ = temp1;
  }
  else
    img_ = temp;

  bgui_vtol2D_tableau_sptr btab = this->get_selected_vtol2D_tableau();
  if (btab)
  {
    vgui_image_tableau_sptr itab = btab->get_image_tableau();
    itab->set_image(img_);
    itab->post_redraw();
    images_set_ = false;
    harris_set_ = false;
    return;
  }
  vcl_cout << "In bmvv_recon_manager::load_image() - null tableau\n";
}

//===================================================================
//: Clear the display
//===================================================================
void bmvv_recon_manager::clear_display()
{
  bgui_vtol2D_tableau_sptr btab = this->get_selected_vtol2D_tableau();
  if (btab)
    btab->clear_all();
  else
    vcl_cout << "In bmvv_recon_manager::clear_display() - null tableau\n";
}

//===================================================================
//: clear all selections in both panes
//===================================================================
void bmvv_recon_manager::clear_selected()
{
  for (vcl_vector<bgui_vtol2D_tableau_sptr>::iterator bit = vtol_tabs_.begin();
       bit != vtol_tabs_.end(); bit++)
    if (*bit)
      (*bit)->deselect_all();
}


//====================================================================
//: Gets the image at the given position.
//  if col, row are out of bounds then null is returned
//  row is currently not used but may be when we have more than 2 cameras
//====================================================================
vil1_image bmvv_recon_manager::get_image_at(unsigned col, unsigned row)
{
  bgui_vtol2D_tableau_sptr btab = this->get_vtol2D_tableau_at(col, row);
  if (btab)
  {
    vgui_image_tableau_sptr itab = btab->get_image_tableau();
    return itab->get_image();
  }
  vcl_cout << "In bmvv_recon_manager::get_image_at() - null tableau\n";
  return 0;
}

void bmvv_recon_manager::read_3d_points()
{
  vgui_dialog load_3d_points_dlg("Load 3d Points");
  static vcl_string points_filename = "";
  static vcl_string ext = "*.*";
  load_3d_points_dlg.file("Planar 3-d Points File:", ext, points_filename);
  if (!load_3d_points_dlg.ask())
    return;
  cal_.read_data(points_filename);
}

void bmvv_recon_manager::initial_model_projection()
{
  bgui_vtol2D_tableau_sptr btab = this->get_selected_vtol2D_tableau();
  if (!btab)
    return;
  vgui_dialog initial_project_dlg("Project Model (Inital)");
  initial_project_dlg.field("World Plane", plane_);
  if (!initial_project_dlg.ask())
    return;
  vgui_image_tableau_sptr itab = btab->get_image_tableau();
  vil1_image image = itab->get_image();
  if (!image)
    return;
  int width = image.width(), height = image.height();
  cal_.set_image_size(brct_plane_calibrator::LEFT, width, height);
  cal_.set_image_size(brct_plane_calibrator::RIGHT, width, height);
  if (!cal_.compute_initial_homographies())
  {
    vcl_cout << "In initial_model_projection() - problem computing initial homographies\n";
    return;
  }
  vcl_vector<vgl_point_2d<double> > pts_2d =
    cal_.projected_3d_points_initial(plane_, this->get_cam());
  btab->set_point_radius(5.0f);
  int i = 0;
  for (vcl_vector<vgl_point_2d<double> >::iterator pit = pts_2d.begin();
      pit != pts_2d.end(); pit++, i++)
    {
      vgui_soview2D_point* sov = btab->add_point((*pit).x(), (*pit).y());
      int id = sov->get_id();
      point_3d_map_[id]=i;
    }
  btab->post_redraw();
}

void bmvv_recon_manager::model_projection()
{
 bgui_vtol2D_tableau_sptr btab = this->get_selected_vtol2D_tableau();
  if (!btab)
    return;
 vgui_dialog initial_project_dlg("Project Model");
  initial_project_dlg.field("World Plane", plane_);
  if (!initial_project_dlg.ask())
    return;
  vcl_vector<vgl_point_2d<double> > pts_2d =
    cal_.projected_3d_points(plane_, this->get_cam());
  btab->set_point_radius(5.0f);
  int i = 0;
  for (vcl_vector<vgl_point_2d<double> >::iterator pit = pts_2d.begin();
      pit != pts_2d.end(); pit++, i++)
    {
      vgui_soview2D_point* sov = btab->add_point((*pit).x(), (*pit).y());
      int id = sov->get_id();
      point_3d_map_[id]=i;
    }
  btab->post_redraw();
}

brct_plane_corr_sptr bmvv_recon_manager::get_selected_corr()
{
  bgui_vtol2D_tableau_sptr btab = this->get_selected_vtol2D_tableau();
  if (!btab)
    return (brct_plane_corr*)0;
  vcl_vector<unsigned> ids = btab->get_selected();
  //take the last selected
  int n = ids.size();
  if (!n)
  {
    vcl_cout << "Nothing selected\n";
    return (brct_plane_corr*)0;
  }
  int i = point_3d_map_[ids[n-1]];
  return cal_.corr(plane_, i);
}

void bmvv_recon_manager::print_selected_corr()
{
  brct_plane_corr_sptr corr = this->get_selected_corr();
  vcl_cout << *corr << vcl_endl;
}

void bmvv_recon_manager::draw_corr_point(const float x, const float y)
{
 bgui_vtol2D_tableau_sptr btab = this->get_selected_vtol2D_tableau();
 if (!btab)
    return;
 btab->set_point_radius(3.0f);
 btab->set_foreground(0.0f, 1.0f, 0.0f);
 btab->add_point(x, y);
 btab->post_redraw();
}

void bmvv_recon_manager::pick_corr()
{
  brct_plane_corr_sptr corr = this->get_selected_corr();
  if (!corr)
    return;
  bgui_picker_tableau_sptr ptab = this->get_selected_picker_tableau();
  if (!ptab)
    return;
  float x=0, y=0;
  ptab->pick_point(&x,&y);
  corr->set_match(this->get_cam(), x, y);
  this->draw_corr_point(x, y);
}

void bmvv_recon_manager::write_corrs()
{
  vgui_dialog corr_dlg("Write Correspondences");
  static vcl_string corr_file = "";
  static vcl_string ext = "*.*";
  corr_dlg.file("Correspondence File", ext, corr_file);
  if (!corr_dlg.ask())
    return;
  if (!cal_.write_corrs(corr_file))
    vcl_cout << "In bmvv_recon_manager::write_corrs() - failed write\n";
}

void bmvv_recon_manager::read_corrs()
{
  vgui_dialog corr_dlg("Read Correspondences");
  static vcl_string corr_file = "";
  static vcl_string ext = "*.*";
  corr_dlg.file("Correspondence File", ext, corr_file);
  if (!corr_dlg.ask())
    return;
  if (!cal_.read_corrs(corr_file))
    vcl_cout << "In bmvv_recon_manager::read_corrs() - failed read\n";
}

void bmvv_recon_manager::compute_homographies()
{
  if (!cal_.compute_homographies())
    vcl_cout << "In bmvv_recon_manager::compute_homographies() - failed\n";
}

void bmvv_recon_manager::write_homographies()
{
  vgui_dialog homg_dlg("Write Homographies");
  static vcl_string homg_file = "";
  static vcl_string ext = "*.*";
  homg_dlg.file("Homgraphy File", ext, homg_file);
  if (!homg_dlg.ask())
    return;
  if (!cal_.write_homographies(homg_file))
    vcl_cout << "In bmvv_recon_manager::write_homgraphies() - failed\n";
}

void bmvv_recon_manager::read_homographies()
{
  vgui_dialog homg_dlg("Read Homographies");
  static vcl_string homg_file = "";
  static vcl_string ext = "*.*";
  homg_dlg.file("Homgraphy File", ext, homg_file);
  if (!homg_dlg.ask())
    return;
  if (!sweep_.read_homographies(homg_file))
    vcl_cout << "In bmvv_recon_manager::read_homgraphies() - failed\n";
}

void bmvv_recon_manager::project_image()
{
 bgui_vtol2D_tableau_sptr btab = this->get_selected_vtol2D_tableau();
 if (!btab)
   return;
 vgui_image_tableau_sptr itab = btab->get_image_tableau();
 vil1_image image = itab->get_image();
 if (!image)
 {
   vcl_cout << "In bmvv_recon_manager::project_image() -"
            << " no image loaded in selected pane\n";
   return;
 }
 vgui_dialog image_project_dlg("Project Image");
  image_project_dlg.field("World Plane", plane_);
  if (!image_project_dlg.ask())
    return;
  int cam = this->get_cam();
  sweep_.set_image(cam, image);
  vil1_memory_image_of<unsigned char> pimg =
    sweep_.project_image_to_plane(plane_, cam);
  itab->set_image(pimg);
  itab->post_redraw();
}

void bmvv_recon_manager::set_images()
{
  for (int cam = 0; cam<2; cam++)
  {
    bgui_vtol2D_tableau_sptr btab = vtol_tabs_[cam];
    vgui_image_tableau_sptr itab = btab->get_image_tableau();
    vil1_image image = itab->get_image();
    if (!image)
    {
      vcl_cout << "In bmvv_recon_manager::set_images()-"
               << " not enought images\n";
      return;
    }
    if (!sweep_.set_image(cam, image))
    {
      vcl_cout << "In bmvv_recon_manager::set_images()-"
               << " can't set image "<< cam << vcl_endl;
      return;
    }
  }
  images_set_ = true;
  harris_set_ = false;
}

void bmvv_recon_manager::overlapping_projections()
{
  if (!images_set_)
  {
    vcl_cout << "In bmvv_recon_manager::overlapping_projections() -"
             << " images not set\n";
    return;
  }
  vgui_dialog image_project_dlg("Overlapping Projections");
  image_project_dlg.field("World Plane", plane_);
  if (!image_project_dlg.ask())
    return;
  vcl_vector<vil1_memory_image_of<float> > imgs;
  if (!sweep_.overlapping_projections(plane_, imgs))
  {
    vcl_cout << "In bmvv_recon_manager::overlapping_projections()-"
             << " overlap failed\n";
        return;
  }
  for (int cam = 0; cam<2; cam++)
  {
    bgui_vtol2D_tableau_sptr btab = vtol_tabs_[cam];
    vgui_image_tableau_sptr itab = btab->get_image_tableau();
    vil1_memory_image_of<unsigned char> temp =
      brip_vil1_float_ops::convert_to_byte(imgs[cam], 0, 255);
    itab->set_image(temp);
    itab->post_redraw();
  }
}

void bmvv_recon_manager::overlapping_projections_z()
{
  if (!images_set_)
  {
    vcl_cout << "In bmvv_recon_manager::overlapping_projections_z() -"
             << " images not set\n";
    return;
  }
  static double z;
  vgui_dialog image_project_dlg("Overlapping Projections at Z");
  image_project_dlg.field("Depth z value", z);
  if (!image_project_dlg.ask())
    return;
  vcl_vector<vil1_memory_image_of<float> > imgs;
  if (!sweep_.overlapping_projections(z, imgs))
  {
    vcl_cout << "In bmvv_recon_manager::overlapping_projections()-"
             << " overlap failed\n";
        return;
  }
  for (int cam = 0; cam<2; cam++)
  {
    bgui_vtol2D_tableau_sptr btab = vtol_tabs_[cam];
    vgui_image_tableau_sptr itab = btab->get_image_tableau();
    vil1_memory_image_of<unsigned char> temp =
      brip_vil1_float_ops::convert_to_byte(imgs[cam], 0, 255);
    itab->set_image(temp);
    itab->post_redraw();
  }
}

void bmvv_recon_manager::
draw_vsol_points(const int cam, vcl_vector<vsol_point_2d_sptr> const & points,
                 bool clear, const float r, const float g, const float b)
{
  bgui_vtol2D_tableau_sptr btab = vtol_tabs_[cam];
  if (!btab)
  {
    vcl_cout << "In bmvv_recon_manager::draw_vsol_points(..) -"
             << " null btol tableau for pane " << cam << vcl_endl;
    return;
  }
  if (clear)
    btab->clear_all();
  vgui_style_sptr style = vgui_style::new_style(r, g, b, 3, 0);
  for (vcl_vector<vsol_point_2d_sptr>::const_iterator pit = points.begin();
      pit != points.end(); pit++)
    btab->add_vsol_point_2d(*pit, style);
}

void bmvv_recon_manager::
draw_vsol_point(const int cam, vsol_point_2d_sptr const & point,
                bool clear, const float r, const float g, const float b)
{
  bgui_vtol2D_tableau_sptr btab = vtol_tabs_[cam];
  if (!btab)
  {
    vcl_cout << "In bmvv_recon_manager::draw_vsol_point(..) -"
             << " null btol tableau for pane " << cam << vcl_endl;
    return;
  }
  if (clear)
    btab->clear_all();
  vgui_style_sptr style = vgui_style::new_style(r, g, b, 3, 0);
  btab->add_vsol_point_2d(point,style);
}

//: color is determined by depth
void bmvv_recon_manager::
draw_vsol_3d_points(const int cam, vcl_vector<vsol_point_3d_sptr> const& pts3d,
                    bool clear)
{
  bgui_vtol2D_tableau_sptr btab = vtol_tabs_[cam];
  if (!btab)
  {
    vcl_cout << "In bmvv_recon_manager::draw_vsol_3d_points(..) -"
             << " null btol tableau for pane " << cam << vcl_endl;
    return;
  }
  if (clear)
    btab->clear_all();
  double zmin = vnl_numeric_traits<double>::maxval, zmax = -zmin;
  for (vcl_vector<vsol_point_3d_sptr>::const_iterator pit = pts3d.begin();
      pit != pts3d.end(); pit++)
  {
    zmin = vnl_math_min(zmin, (*pit)->z());
    zmax = vnl_math_max(zmax, (*pit)->z());
  }
  double d = zmax-zmin, s = 1;
  if (d)
    s = 1/d;
  for (vcl_vector<vsol_point_3d_sptr>::const_iterator pit = pts3d.begin();
      pit != pts3d.end(); pit++)
  {
    float f = (float)(((*pit)->z()-zmin)*s);
    vcl_cout << "f(" << (*pit)->z() << ")=" << f << vcl_endl;
    vsol_point_2d_sptr p = new vsol_point_2d((*pit)->x(), (*pit)->y());
    vgui_style_sptr style = vgui_style::new_style(f, 0, 1-f, 3, 0);
    btab->add_vsol_point_2d(p,style);
  }
}

void bmvv_recon_manager::compute_harris_corners()
{
  if (!images_set_)
  {
    vcl_cout << "In bmvv_recon_manager::compute_harris_corners() -"
             << " images not set\n";
    return;
  }
  vgui_dialog harris_dialog("Compute Harris Corners");
  harris_dialog.field("sigma", sweep_.hdp_.sigma_);
  harris_dialog.field("thresh", sweep_.hdp_.thresh_);
  harris_dialog.field("N = 2n+1, (n)", sweep_.hdp_.n_);
  harris_dialog.field("Max No Corners(percent)", sweep_.hdp_.percent_corners_);
  harris_dialog.field("scale_factor", sweep_.hdp_.scale_factor_);
  if (!harris_dialog.ask())
    return;
  if (!sweep_.compute_harris())
    return;
  for (int cam = 0; cam<2; cam++)
  {
    vcl_vector<vsol_point_2d_sptr> points = sweep_.harris_corners(cam);
    this->draw_vsol_points(cam, points);
  }
  harris_set_ = true;
}

void bmvv_recon_manager::overlapping_harris_proj_z()
{
  if (!images_set_||!harris_set_)
  {
    vcl_cout << "In bmvv_recon_manager::overlapping_projections_z() -"
             << " images not set or harris not ready\n";
    return;
  }
  static double z;
  vgui_dialog image_project_dlg("Overlapping Harris Projections at Z");
  image_project_dlg.field("Depth z value", z);
  if (!image_project_dlg.ask())
    return;
  vcl_vector<vil1_memory_image_of<float> > imgs;
  vcl_vector<vcl_vector<vsol_point_2d_sptr> > harris_corners;
  if (!sweep_.overlapping_projections(z, imgs, harris_corners))
  {
    vcl_cout << "In bmvv_recon_manager::overlapping_projections()-"
             << " overlap failed\n";
        return;
  }
  for (int cam = 0; cam<2; cam++)
  {
    bgui_vtol2D_tableau_sptr btab = vtol_tabs_[cam];
    vgui_image_tableau_sptr itab = btab->get_image_tableau();
    vil1_memory_image_of<unsigned char> temp =
      brip_vil1_float_ops::convert_to_byte(imgs[cam], 0, 255);
    itab->set_image(temp);
    itab->post_redraw();
    this->draw_vsol_points(cam, harris_corners[cam]);
  }
}

void bmvv_recon_manager::cross_correlate_plane()
{
  if (!images_set_)
  {
    vcl_cout << "In bmvv_recon_manager::overlapping_projections() -"
             << " images not set\n";
    return;
  }
 vgui_dialog cc_plane_dlg("Cross Correlate Plane");
  cc_plane_dlg.field("World Plane", plane_);
  cc_plane_dlg.field("Corr Display Range (min)", sweep_.corr_min_);
  cc_plane_dlg.field("Corr Display Range (max)", sweep_.corr_max_);
  cc_plane_dlg.field("Correlation Sigma", sweep_.corr_sigma_);

  if (!cc_plane_dlg.ask())
    return;
  vil1_memory_image_of<unsigned char> cc =
    sweep_.cross_correlate_projections(plane_);
  if (!cc)
  {
    vcl_cout << "In bmvv_recon_manager::cross_correlate_plane()-"
             << " correlation failed\n";
    return;
  }
  bgui_vtol2D_tableau_sptr btab = vtol_tabs_[0];
  vgui_image_tableau_sptr itab = btab->get_image_tableau();
  itab->set_image(cc);
  itab->post_redraw();
}

void bmvv_recon_manager::cross_correlate_z()
{
  if (!images_set_)
  {
    vcl_cout << "In bmvv_recon_manager::overlapping_projections() -"
             << " images not set\n";
    return;
  }
  static double z = 0;
  vgui_dialog cc_z_dlg("Cross Correlate Z");
  cc_z_dlg.field("World Plane", z);
  cc_z_dlg.field("Corr Display Range (min)", sweep_.corr_min_);
  cc_z_dlg.field("Corr Display Range (max)", sweep_.corr_max_);
  cc_z_dlg.field("Corr Sigma", sweep_.corr_sigma_);
  if (!cc_z_dlg.ask())
    return;
  vil1_memory_image_of<unsigned char> cc =
    sweep_.cross_correlate_projections(z);
  if (!cc)
  {
    vcl_cout << "In bmvv_recon_manager::cross_correlate_z()-"
             << " correlation failed\n";
    return;
  }
  bgui_vtol2D_tableau_sptr btab = vtol_tabs_[0];
  vgui_image_tableau_sptr itab = btab->get_image_tableau();
  itab->set_image(cc);
  itab->post_redraw();
}

void bmvv_recon_manager::cross_correlate_harris_z()
{
  if (!images_set_||!harris_set_)
  {
    vcl_cout << "In bmvv_recon_manager::overlapping_projections() -"
             << " images or harris corners not set\n";
    return;
  }
  static double z = 0;
  static bool reset = true;
  vgui_dialog cc_z_harris_dlg("Cross Correlate Harris Corners at Z");
  cc_z_harris_dlg.field("World Plane", z);
  cc_z_harris_dlg.field("Match radius", sweep_.point_radius_);
  cc_z_harris_dlg.field("Corr radius", sweep_.corr_radius_);
  cc_z_harris_dlg.field("Correlation Threshold", sweep_.corr_thresh_);
  cc_z_harris_dlg.checkbox("Reset Match Flags", reset);
  if (!cc_z_harris_dlg.ask())
    return;
  vil1_image img;
  vcl_vector<vsol_point_2d_sptr> matched_corners, back_proj_cnrs, orig_cnrs0;

  if (!sweep_.cross_correlate_proj_corners(z, img,
                                          matched_corners,
                                          back_proj_cnrs,
                                          orig_cnrs0,
                                          reset))
    {
      vcl_cout << "In bmvv_recon_manager::cross_correlate_harris_z()-"
               << " correlation failed\n";
      return;
    }
  bgui_vtol2D_tableau_sptr btab = vtol_tabs_[1];
  vgui_image_tableau_sptr itab = btab->get_image_tableau();
  itab->set_image(img);
  itab->post_redraw();
  this->draw_vsol_points(0, orig_cnrs0);
  this->draw_vsol_points(0, back_proj_cnrs, false, 1, 0, 0);
  this->draw_vsol_points(1, matched_corners);
}

void bmvv_recon_manager::depth_image()
{
  if (!images_set_)
  {
    vcl_cout << "In bmvv_recon_manager::overlapping_projections() -"
             << " images not set\n";
    return;
  }
  vgui_dialog depth_dlg("Compute Depth Image");
  depth_dlg.field("Min Z", sweep_.zmin_);
  depth_dlg.field("Max Z", sweep_.zmax_);
  depth_dlg.field("N Depth Planes", sweep_.nz_);
  depth_dlg.field("Corr Display Range (min)", sweep_.corr_min_);
  depth_dlg.field("Corr Display Range (max)", sweep_.corr_max_);
  depth_dlg.field("Corr Sigma", sweep_.corr_sigma_);
  depth_dlg.field("Correlation Threshold", sweep_.corr_thresh_);
  depth_dlg.field("Mean Intensity Threshold", sweep_.intensity_thresh_);
  if (!depth_dlg.ask())
    return;
  vil1_memory_image_of<unsigned char> depth, max_corr;
  if (!sweep_.depth_image(depth, max_corr))
  {
    vcl_cout << "In bmvv_recon_manager::depth_image()-"
             << " depth image failed\n";
    return;
  }
  bgui_vtol2D_tableau_sptr btab = vtol_tabs_[0];
  vgui_image_tableau_sptr itab = btab->get_image_tableau();
  itab->set_image(depth);
  itab->post_redraw();
  btab = vtol_tabs_[1];
  itab = btab->get_image_tableau();
  itab->set_image(max_corr);
  itab->post_redraw();
}

void bmvv_recon_manager::z_corr_image()
{
 bgui_vtol2D_tableau_sptr btab = this->get_selected_vtol2D_tableau();
 if (!btab)
   return;
 vgui_image_tableau_sptr itab = btab->get_image_tableau();
 static int z = 0;
 vgui_dialog z_corr_dlg("Z-Correlation Image");
  z_corr_dlg.field("Z index", z);
  z_corr_dlg.field("Corr Display Range (min)", sweep_.corr_min_);
  z_corr_dlg.field("Corr Display Range (max)", sweep_.corr_max_);
  if (!z_corr_dlg.ask())
    return;

  vil1_memory_image_of<unsigned char> z_image = sweep_.z_corr_image(z);
  if (!z_image)
  {
    vcl_cout << "No Z_corr_images available\n";
    return;
  }
  itab->set_image(z_image);
  itab->post_redraw();
}

void bmvv_recon_manager::harris_depth_match()
{
  if (!images_set_||!harris_set_)
  {
    vcl_cout << "In bmvv_recon_manager::harris_depth_match() -"
             << " images or harris corners not set\n";
    return;
  }
  vgui_dialog depth_dlg("Compute Harris Depth Match");
  depth_dlg.field("Min Z", sweep_.zmin_);
  depth_dlg.field("Max Z", sweep_.zmax_);
  depth_dlg.field("N Depth Planes", sweep_.nz_);
  depth_dlg.field("Corr Display Range (min)", sweep_.corr_min_);
  depth_dlg.field("Corr Display Range (max)", sweep_.corr_max_);
  depth_dlg.field("Point Match Radius", sweep_.point_radius_);
  depth_dlg.field("Correlation Window Radius", sweep_.corr_radius_);
  depth_dlg.field("CorrelationSigma", sweep_.corr_sigma_);
  depth_dlg.field("Correlation Threshold", sweep_.corr_thresh_);
  depth_dlg.field("Mean Intensity Threshold", sweep_.intensity_thresh_);
  if (!depth_dlg.ask())
    return;
  vcl_vector<vsol_point_3d_sptr> points_3d;
  vcl_vector<vsol_point_2d_sptr> proj_points;
  if (!sweep_.harris_depth_match(points_3d, proj_points))
  {
    vcl_cout << "In bmvv_recon_manager::depth_image()-"
             << " depth image failed\n";
    return;
  }
  this->draw_vsol_points(1, proj_points);
}

void bmvv_recon_manager::corr_plot()
{
  bgui_picker_tableau_sptr ptab = this->get_selected_picker_tableau();
  if (!ptab)
    return;
  float x=0, y=0;
  ptab->pick_point(&x,&y);
  vcl_vector<float> z, corr;
  int ix = (int)x, iy = (int)y;
  sweep_.corr_vals(ix, iy, z, corr);
  int n = z.size();
  if (!n)
    return;
  vcl_cout << "C(z)[" << x << "][" << y << "]\n";
  for (int i = 0; i<n; i++)
    vcl_cout << z[i] << '\t' << corr[i] << '\n';
  vcl_cout << vcl_endl;
}

//: create a point in the pane corresponding to cam
void bmvv_recon_manager::create_point(int& cam, vsol_point_2d_sptr& p)
{
  cam = this->get_cam();
  bgui_picker_tableau_sptr ptab = this->get_selected_picker_tableau();
  if (!ptab)
    return;
  float x=0, y=0;
  ptab->pick_point(&x,&y);
  this->draw_corr_point(x, y);
  p = new vsol_point_2d(x, y);
}

//: map a point from cam a to cam b at depth z
void bmvv_recon_manager::map_point()
{
  static double z = 0;
  vgui_dialog z_map_dlg("Map Point at Z");
  z_map_dlg.field("Depth value (z)", z);
  if (!z_map_dlg.ask())
    return;
  vcl_cout << "Waiting to pick a point\n";
  int cam = 0;
  vsol_point_2d_sptr p, q;
  this->create_point(cam, p);
  if (!p)
  {
    vcl_cout << "Pick Failed\n";
    return;
  }
  vcl_cout << "got point at " << p->x() << ' ' << p->y() << vcl_endl;
  q = sweep_.map_point(p, cam, z);
  if (!q)
  {
    vcl_cout << "Map Failed\n";
    return;
  }
  int camb = 1-cam;
  this->draw_vsol_point(camb, q);
}

void bmvv_recon_manager::map_image()
{
  if (!images_set_)
  {
    vcl_cout << "Images not set\n";
    return;
  }
 static double z = 0;
 vgui_dialog z_map_dlg("Image to image Mapping");
 z_map_dlg.field("Depth value (z)", z);
 if (!z_map_dlg.ask())
   return;
 int from_cam = this->get_cam();
 vil1_memory_image_of<unsigned char> mapped_to_image, orig_to_image;
 if (!sweep_.map_image_to_image(from_cam, z, mapped_to_image, orig_to_image))
   return;
 bgui_vtol2D_tableau_sptr btab = vtol_tabs_[0];
 vgui_image_tableau_sptr itab = btab->get_image_tableau();
 itab->set_image(orig_to_image);
 itab->post_redraw();
 btab = vtol_tabs_[1];
 itab = btab->get_image_tableau();
 itab->set_image(mapped_to_image);
 itab->post_redraw();
}

void bmvv_recon_manager::map_harris_corners()
{
  if (!harris_set_)
  {
    vcl_cout << "No Harris corners\n";
    return;
  }
  static double z = 0;
  vgui_dialog z_map_dlg("Harris corner Mapping");
  z_map_dlg.field("Depth value (z)", z);
  if (!z_map_dlg.ask())
    return;
  int from_cam = this->get_cam();
  vcl_vector<vsol_point_2d_sptr> mapped_to_points, orig_to_points;
  if (!sweep_.map_harris_corners(from_cam, z, mapped_to_points, orig_to_points))
    return;
  this->draw_vsol_points(0, orig_to_points, true, 0, 1, 0);
  this->draw_vsol_points(0, mapped_to_points, false, 1, 0, 0);
}

void bmvv_recon_manager::match_harris_corners()
{
  if (!harris_set_)
  {
    vcl_cout << "No Harris corners\n";
    return;
  }
  static double z = 0;
  vgui_dialog z_match_dlg("Harris corner Matching");
  z_match_dlg.field("Depth value (z)", z);
  z_match_dlg.field("Point Radiuus", sweep_.point_radius_);
  if (!z_match_dlg.ask())
    return;
  int from_cam = this->get_cam();
  vcl_vector<vsol_point_2d_sptr> matched_to_points, orig_to_points;
  sweep_.init_harris_match(from_cam);
  if (!sweep_.match_harris_corners(from_cam, z, matched_to_points,
                                   orig_to_points))
    return;
  this->draw_vsol_points(0, orig_to_points, true, 0, 1, 0);
  this->draw_vsol_points(0, matched_to_points, false, 1, 0, 0);
}

void bmvv_recon_manager::harris_sweep()
{
  static int from_cam=1;
  if (!images_set_||!harris_set_)
  {
    vcl_cout << "In bmvv_recon_manager::harris_sweep() -"
             << " images or harris corners not set\n";
    return;
  }
  vgui_dialog harris_sweep_dlg("Harris Sweep");
  harris_sweep_dlg.field("From CAM", from_cam);
  harris_sweep_dlg.field("Min Z", sweep_.zmin_);
  harris_sweep_dlg.field("Max Z", sweep_.zmax_);
  harris_sweep_dlg.field("N Depth Planes", sweep_.nz_);
  harris_sweep_dlg.field("Point Match Radius", sweep_.point_radius_);
  harris_sweep_dlg.field("Correlation Thresh", sweep_.corr_thresh_);
  harris_sweep_dlg.field("Correlation Radius", sweep_.corr_radius_);
  if (!harris_sweep_dlg.ask())
    return;
  if (!sweep_.harris_sweep(from_cam))
  {
    vcl_cout << "Sweep failed\n"<< vcl_flush;
    return;
  }
}

void bmvv_recon_manager::display_matched_corners()
{
  static int z_index;
  vgui_dialog hmatch_dlg("Harris Sweep");
  hmatch_dlg.field("Zindex", z_index);
  if (!hmatch_dlg.ask())
    return;
  vcl_vector<vsol_point_2d_sptr> matched_points =
    sweep_.matched_points_at_z_index(z_index);
  int n = matched_points.size();
  if (!n)
    return;
  //for now assume we display on pane 0
  this->draw_vsol_points(0, matched_points, true, 0, 1, 0);
}

void bmvv_recon_manager::display_harris_3d()
{
  if (!harris_set_)
    return;
  vcl_vector<vsol_point_3d_sptr> points = sweep_.proj_points_3d();
  int n = points.size();
  if (!n)
    return;
  //for now assume we display on pane 0
  this->draw_vsol_3d_points(0, points, true);
}

void bmvv_recon_manager::write_points_vrml()
{
  vgui_dialog save_vrml_dlg("Save VRML File");
  static vcl_string filename = "";
  static vcl_string ext = "*.*";
  save_vrml_dlg.file("VRML file name", ext, filename);
  if (!save_vrml_dlg.ask())
    return;
  if (filename == "")
  {
    vcl_cout << "Need a file name\n";
    return;
  }
  if (!sweep_.save_world_points(filename))
  {
    vcl_cout << "VRML Save Failed\n";
    return;
  }
}

void bmvv_recon_manager::read_points_vrml()
{
  vgui_dialog read_vrml_dlg("Read VRML points File");
  static vcl_string filename = "";
  static vcl_string ext = "*.*";
  read_vrml_dlg.file("VRML file name", ext, filename);
  if (!read_vrml_dlg.ask())
    return;
  if (filename == "")
  {
    vcl_cout << "Need a file name\n";
    return;
  }
  if (!vproc_.read_points_3d_vrml(filename))
  {
    vcl_cout << "VRML read failed\n";
    return;
  }
}

void bmvv_recon_manager::write_volumes_vrml()
{
  vgui_dialog write_volumes_dlg("Write VRML Volume File");
  static vcl_string filename = "";
  static vcl_string ext = "*.*";
  write_volumes_dlg.file("VRML file name", ext, filename);
  if (!write_volumes_dlg.ask())
    return;
  if (filename == "")
  {
    vcl_cout << "Need a file name\n";
    return;
  }
  if (!vproc_.write_prob_volumes_vrml(filename))
  {
    vcl_cout << "VRML volume write failed\n";
    return;
  }
}

void bmvv_recon_manager::read_change_data()
{
  static vcl_string filename = "";
  static vcl_string ext = "*.*";
  vgui_dialog change_vrml_dlg("Change data File(VRML)");
  change_vrml_dlg.file("Change data file name (VRML)", ext, filename);
  if (!change_vrml_dlg.ask())
    return;
  if (filename == "")
  {
    vcl_cout << "Need a file name\n";
    return;
  }
  if (!vproc_.read_change_data_vrml(filename))
  {
    vcl_cout << "VRML read failed\n";
    return;
  }
}

void bmvv_recon_manager::write_change_volumes_vrml()
{
  vgui_dialog write_change_volumes_dlg("Write Change Volumes(VRML)");
  static vcl_string filename = "";
  static vcl_string ext = "*.*";
  write_change_volumes_dlg.file("VRML file name", ext, filename);
  if (!write_change_volumes_dlg.ask())
    return;
  if (filename == "")
  {
    vcl_cout << "Need a file name\n";
    return;
  }
  if (!vproc_.write_changed_volumes_vrml(filename))
  {
    vcl_cout << "VRML volume write failed\n";
    return;
  }
}

void bmvv_recon_manager::compute_change()
{
  vgui_dialog change_dlg("Change Detection");
  change_dlg.field("Cell Thresh", vproc_.cell_thresh_);
  if (!change_dlg.ask())
    return;
  vproc_.compute_change();
}

//: display a line of the dense match by drawing tokens
void bmvv_recon_manager::display_dense_match()
{
  vil1_image img0 = this->get_image_at(0,0);
  vil1_image img1 = this->get_image_at(1,0);
  if (!img0||!img1)
  {
    vcl_cout << "In bmvv_recon_manager::display_dense_match()"
             << " - need two images\n";
    return;
  }
  vtol_tabs_[0]->clear_all();
  vtol_tabs_[1]->clear_all();
  static int raster = 200;
  static int range = 10;
  static int radius = 5;
  static double inner = 1.0;
  static double outer = 0.5;
  static double continuity = 0.1;
  static bool print_corr = false;
  vgui_dialog dense_recon_dlg("Dense Reconstruction");
  dense_recon_dlg.field("Raster No.", raster);
  dense_recon_dlg.field("Search Range.", range);
  dense_recon_dlg.field("Corr. Window Radius", radius);
  dense_recon_dlg.field("Inner Cost", inner);
  dense_recon_dlg.field("Outer Cost", outer);
  dense_recon_dlg.field("Continuity", continuity);
  dense_recon_dlg.checkbox("Print Corr", print_corr);
  if (!dense_recon_dlg.ask())
    return;

  brct_dense_reconstructor dr(img0, img1);
  dr.set_search_range(range);
  dr.set_correlation_window_radius(radius);
  dr.set_inner_cost(inner);
  dr.set_outer_cost(outer);
  dr.set_continuity_cost(continuity);
  dr.print_params();
  dr.initial_calculations();
  dr.evaluate_raster(raster);
  vcl_vector<vsol_point_2d_sptr> points0 = dr.points0(raster);
  vcl_vector<vsol_point_2d_sptr> points1 = dr.points1(raster);
  int k = 0, ip = 0;
  for (vcl_vector<vsol_point_2d_sptr>::iterator pit = points0.begin();
       pit != points0.end(); ++pit, ++ip, ++k)
  {
    if (k==3)
      k = 0;
    if (points1[ip]->x()<0)
    {
      this->draw_vsol_point(0, *pit, false, 0, 0, 0);
      this->draw_vsol_point(1, points1[ip], false, 1, 1, 1);
      continue;
    }
    if (!k)
    {
      this->draw_vsol_point(0, *pit, false, 1, 0, 0);
      this->draw_vsol_point(1, points1[ip], false, 1, 0, 0);
    }
    if (k==1)
    {
      this->draw_vsol_point(0, *pit, false, 0, 1, 0);
      this->draw_vsol_point(1, points1[ip], false, 0, 1, 0);
    }
    if (k==2)
    {
      this->draw_vsol_point(0, *pit, false, 0, 0, 1);
      this->draw_vsol_point(1, points1[ip], false, 0, 0, 1);
    }
  }
  vtol_tabs_[0]->post_redraw();
  vtol_tabs_[1]->post_redraw();
  if (print_corr)
  {
    static double x0 = 100;
    vgui_dialog corr_dlg("Print Corr");
    corr_dlg.field("Xlocation", x0);
    if (!corr_dlg.ask())
      return;
    vcl_vector<int> xpos;
    vcl_vector<double> corr;
    dr.get_correlation(int(x0), raster, xpos, corr);
    vcl_cout << "Correlation\n";
    unsigned int n = xpos.size();
    for (unsigned int i = 0; i<n; ++i)
      vcl_cout << xpos[i] << '\t' << corr[i] << '\n';
    vcl_cout << vcl_flush;
  }
}

//===================================================================
//: read the fundamental matrix
//
//===================================================================
void bmvv_recon_manager::read_f_matrix()
{
  vgui_dialog read_F_dlg("Read F Matrix");
  static vcl_string filename = "";
  static vcl_string ext = "*.*";
  read_F_dlg.file("F Matrix file name", ext, filename);
  if (!read_F_dlg.ask())
    return;
  if (filename == "")
  {
    vcl_cout << "Need a file name\n";
    return;
  }
  vcl_ifstream istr(filename.c_str());
  vnl_matrix<double> fm;
  istr >> fm;
  FMatrix F(fm);
  vcl_cout << "F =\n" << F << vcl_endl;
  f_matrix_ = F;
}

//===================================================================
//: pick a point in the left image and show the corresponding epipolar
//  line in the right image
//===================================================================
void bmvv_recon_manager::show_epipolar_line()
{
  this->clear_display();
  vgui::out << "pick point in left image\n";
  unsigned int col=0, row=0;//left image
  bgui_picker_tableau_sptr pkt = this->get_picker_tableau_at(col, row);
  if (!pkt)
  {
    vcl_cout << "In bmvv_multiview_manager::show_epipolar_line() - null tableau\n";
    return;
  }
  float x = 0, y=0;
  pkt->pick_point(&x, &y);
  vgui::out << "p(" << x << ' ' << y << ")\n";
  vcl_cout << "p(" << x << ' ' << y << ")\n";

  col = 1;//right image
  bgui_vtol2D_tableau_sptr v2D = this->get_vtol2D_tableau_at(col,row);

  //temporary test for FMatrix
  vgl_homg_point_2d<double> pl(x,y);
  vgl_homg_line_2d<double> lr = f_matrix_.image2_epipolar_line(pl);
  //end test
  if (v2D)
  {
    v2D->add_infinite_line(lr.a(), lr.b(), lr.c());
    v2D->post_redraw();
  }
}

void bmvv_recon_manager::show_world_homography()
{
  vgui_dialog read_homg_dlg("Read Correspondence File");
  static double mag = 10.0;
  static vcl_string corr_filename = "";
  static vcl_string corr_ext = "*.cm";
  static vcl_string cam_filename = "";
  static vcl_string cam_ext = "*.cam";
  read_homg_dlg.file("correspondence file", corr_ext, corr_filename);
  read_homg_dlg.file("camera file", cam_ext, cam_filename);
  read_homg_dlg.field("Magnification",mag);
  if (!read_homg_dlg.ask())
    return;
  if (corr_filename == ""||cam_filename == "")
  {
    vcl_cout << "Need a file name\n";
    return;
  }
  vcl_ifstream istr(corr_filename.c_str());
  vcl_ofstream ostr(cam_filename.c_str());
  vcl_vector<vgl_point_2d<double> > image_points;
  vcl_vector<vgl_point_3d<double> > world_points;
  vcl_vector<bool> valid(image_points.size(), true);
  if (!brct_algos::read_target_corrs(istr, valid, image_points, world_points))
  {
    vcl_cout << "Failed to read correspondences\n";
    return;
  }
  vgl_h_matrix_2d<double> H, Hs, Hm;
  brct_algos::homography(world_points, image_points, H);
  vcl_cout << "H\n" << H << vcl_endl;
  brct_algos::scale_and_translate_world(-120.4,-8.29, mag, Hs);
  vnl_double_3x3 M, Ms, Mm;
  M = H.get_inverse().get_matrix();
  Ms = Hs.get_matrix();
  vcl_cout << "Ms\n" << Ms << vcl_endl;
  Mm = Ms*M;
  Hm = vgl_h_matrix_2d<double>(Mm);
  vcl_cout << "Back-projected and scaled world points\n";
  for (unsigned int i = 0; i<world_points.size(); i++)
  {
    vgl_point_2d<double>pi = image_points[i];
    vgl_homg_point_2d<double> hpi(pi.x(), pi.y()), hpw;
    hpw = Hm(hpi);
    vgl_point_2d<double> pw(hpw);
    vcl_cout << pw << vcl_endl;
  }
  bgui_vtol2D_tableau_sptr v2d = this->get_selected_vtol2D_tableau();
  if (!v2d)
  {
    vcl_cout << "Null Tableau\n";
    return;
  }
  int cam = this->get_cam();
  bgui_vtol2D_tableau_sptr other_tab = vtol_tabs_[1-cam];
  if (!other_tab)
  {
    vcl_cout << "Null Tableau\n";
    return;
  }
  vil1_image img = v2d->get_image_tableau()->get_image();
  vil1_memory_image_of<float> fimg = brip_vil1_float_ops::convert_to_float(img);
  vil1_memory_image_of<float> warped(1600, 450);
  if (brip_vil1_float_ops::homography(fimg, Hm, warped, true))
  {
    vil1_memory_image_of<unsigned char> cimg =
      brip_vil1_float_ops::convert_to_byte(warped, 0, 255);
    vgui_image_tableau_sptr itab = other_tab->get_image_tableau();
    itab->set_image(cimg);
  }
  else
    vcl_cout << "Warping failed\n";
  vcl_vector<vgl_point_2d<double> > proj_image_points;
  vgl_p_matrix<double> P =  brct_algos::p_from_h(H);
  brct_algos::project(world_points, P, proj_image_points);

  vgui_style_sptr r_style = vgui_style::new_style(1.0, 0.0, 0.0, 5, 0);
  vgui_style_sptr g_style = vgui_style::new_style(0.0, 1.0, 0.0, 5, 0);
  for (unsigned int i = 0; i<world_points.size(); i++)
  {
    vgl_point_2d<double> ip = image_points[i];
    vgl_point_2d<double> pip = proj_image_points[i];
    vsol_point_2d_sptr vip = new vsol_point_2d(ip.x(),ip.y());
    vsol_point_2d_sptr vpip = new vsol_point_2d(pip.x(),pip.y());
    vcl_cout << ip << " == " << pip << vcl_endl;
    v2d->add_vsol_point_2d(vip,r_style);
    v2d->add_vsol_point_2d(vpip,g_style);
  }
  v2d->post_redraw();
  //write out the camera
  ostr << P;
  //Also as a target camera for interest sake
  vcl_string target_file = cam_filename + "-target";
  vcl_ofstream ostr_tar(target_file.c_str());
  brct_algos::write_target_camera(ostr_tar, P.get_matrix());
}
