//
// vsrl_manager.cxx
//
// This program was written to test the Dense Matching software
//
// G.W. Brooksby
// 02/13/03

#include <vcl_cstdlib.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vil1/vil1_load.h>
#include <vil1/vil1_scale_intensities.h>
#include <vil1/vil1_save.h>
#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_soview2D.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_projection_inspector.h>
#include <vsrl/vsrl_stereo_dense_matcher.h>
#include <sdet/sdet_region_proc_params.h>
#include <sdet/sdet_region_proc.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_vertex.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_intensity_face.h>
#include <vepl/vepl_gradient_mag.h>
#include <vepl/vepl_gaussian_convolution.h>
#include <vsrl/vsrl_menus.h>
#include <vsrl/vsrl_manager.h>
#include <vsrl/vsrl_point_picker.h>
#include <vsrl/vsrl_results_dense_matcher.h>
#include <vsrl/vsrl_3d_output.h>

extern "C"
{
#include <jseg/jseg.h>
}

// static manager instance
vsrl_manager* vsrl_manager::instance_=0;

//ensure only one instance is created
vsrl_manager *vsrl_manager::instance()
{
  if (!instance_)
    {
      instance_ = new vsrl_manager();
      instance_->init();
    }
  return vsrl_manager::instance_;
}

vsrl_manager::vsrl_manager():vgui_wrapper_tableau(){}

vsrl_manager::~vsrl_manager(){}

void vsrl_manager::init()
{
  shadow_mean_ = 50.0;
  shadows_only_ = true;
  shadow_metric_ = NULL;
  // Load the image tableaux
  itabL_ = vgui_image_tableau_new();
  itabR_ = vgui_image_tableau_new();
  dimg_tab_ = vgui_image_tableau_new();  // disparity image tableau
  disparity_bias_=0;

  // Put the image tableaux into an easy2D tableau
  e2d0_ = vgui_easy2D_tableau_new(itabL_);
  e2d1_ = vgui_easy2D_tableau_new(itabR_);
  e2d2_ = vgui_easy2D_tableau_new(dimg_tab_);

  // Set up characteristics of points to be drawn
  e2d0_->set_foreground(1,0,0);
  e2d1_->set_foreground(1,0,0);
  e2d2_->set_foreground(1,0,0);
  e2d0_->set_point_radius(5);
  e2d1_->set_point_radius(5);
  e2d2_->set_point_radius(5);
  e2d0_->set_line_width(2);
  e2d1_->set_line_width(2);
  e2d2_->set_line_width(2);

  // Put the easy2D tableaux into the viewers
  vgui_viewer2D_tableau_sptr viewer0 = vgui_viewer2D_tableau_new(e2d0_);
  vgui_viewer2D_tableau_sptr viewer1 = vgui_viewer2D_tableau_new(e2d1_);
  vgui_viewer2D_tableau_sptr viewer2 = vgui_viewer2D_tableau_new(e2d2_);

  // Put the viewers into tableaux for picking points
  vpicker0_=new vsrl_point_picker(viewer0);
  vpicker1_=new vsrl_point_picker(viewer1);
  vpicker2_=new vsrl_point_picker(viewer2);

  //Put the viewers into a grid
  grid_ = new vgui_grid_tableau(3,1);
  grid_->add_at(vpicker0_, 0,0);
  grid_->add_at(vpicker1_, 1,0);
  grid_->add_at(vpicker2_, 2,0);
  grid_->set_selected(0,0);
  grid_->set_uses_paging_events(false); // disable paging
  grid_->set_grid_size_changeable(false); // disable adding panes

  // Put the grid into a shell tableau at the top the hierarchy
  vgui_shell_tableau_new shell(grid_);

  // Get a parameters object
  params_ = vsrl_parameters::instance();

  this->add_child(shell);
}

void vsrl_manager::quit()
{
  vcl_exit(1);
}

void vsrl_manager::load_left_image()
{
  vgui_dialog load_image_dlg("Load Image file");
  static vcl_string image_filename = "";
  static vcl_string ext = "*.*";
  load_image_dlg.file("Image Filename:", ext, image_filename);
  if (!load_image_dlg.ask()) return;
  imgL_ = vil1_load(image_filename.c_str());
  itabL_->set_image(imgL_);
  this->post_redraw();
  return;
}

void vsrl_manager::load_right_image()
{
  vgui_dialog load_image_dlg("Load Image file");
  static vcl_string image_filename = "";
  static vcl_string ext = "*.*";
  load_image_dlg.file("Image Filename:", ext, image_filename);
  if (!load_image_dlg.ask()) return;
  imgR_ = vil1_load(image_filename.c_str());
  itabR_->set_image(imgR_);
  this->post_redraw();
  return;
}

void vsrl_manager::load_disparity_image()
{
  vgui_dialog load_image_dlg("Load Disparity Image file");
  static vcl_string image_filename = "";
  static vcl_string ext = "*.*";
  load_image_dlg.file("Disparity Image Filename:", ext, image_filename);
  if (!load_image_dlg.ask()) return;
  disp_img_ = vil1_load(image_filename.c_str());
  vil1_memory_image_of<unsigned char> real_image(disp_img_);

  vil1_image scaled_image = scale_image(real_image);

  // Display the scaled image
  dimg_tab_->set_image(scaled_image);
  this->post_redraw();
}

void vsrl_manager::save_disparity_image()
{
  vgui_dialog save_image_dlg("Save Disparity Image file");
  static vcl_string image_filename = "";
  static vcl_string ext = "*.tif";
  save_image_dlg.file("Disparity Image Filename", ext, image_filename);
  if (!save_image_dlg.ask()) return;
  vil1_memory_image_of<unsigned char> disp(disp_img_);
  if (!vil1_save(disp,image_filename.c_str())) {
    vcl_cout << "Error saving disparity image!\n";
  }
  return;
}

void vsrl_manager::load_params_file()
{
  //
  vgui_dialog load_params_dlg("Load Dense Matcher Parameters file");
  static vcl_string params_filename = "";
  static vcl_string ext = "*.*";
  load_params_dlg.file("Dense Matcher Parameter Filename:", ext, params_filename);
  if (!load_params_dlg.ask()) return;

  // Due to the need for a non-const char* being fed to the .load function
  // we have to construct a filename that is NOT const!
  char* filename = new char[params_filename.length()+1]; // get a new char* array
  params_filename.copy(filename,params_filename.length()); // copy the string into it
  filename[params_filename.length()]=0; //add null terminator
  params_->load(filename); // load the parameters file
  disparity_bias_ = params_->correlation_range;
  delete [] filename;  // delete the filename
}

void vsrl_manager::point_pick()
{
  vcl_cerr << "vsrl_manager::point_pick() not yet implemented\n";
  return;
}

void vsrl_manager::clear_all()
{
  e2d0_->clear();
  e2d1_->clear();
  e2d2_->clear();
  this->post_redraw();
  return;
}

bool vsrl_manager::handle(vgui_event const & event)
{
  this->child.handle(event);

  if (event.type == vgui_BUTTON_DOWN &&
      event.button == vgui_LEFT &&
      !event.modifier)
    {
      put_points();
    }
  else if (event.type == vgui_BUTTON_DOWN &&
           event.button == vgui_LEFT &&
           event.modifier == vgui_SHIFT)
    {
      put_lines();
    }
  else if (event.type == vgui_BUTTON_DOWN &&
           event.button == vgui_MIDDLE &&
           event.modifier == vgui_SHIFT)
    {
      vgl_point_2d<float>  pos = vpicker0_->get_point();  // get the last point picked
      vcl_cout << "handle: pos = " << pos << vcl_endl;
      int x = pos.x(); int y = pos.y(); // convert to int.
      show_correlations(x,y);
    }
  return true;
}

bool vsrl_manager::validate_point(vgl_point_2d<float> const& pt)
{
  if (pt.x() < 0 ||
      pt.y() < 0 ||
      pt.x() >= disp_img_.cols() ||
      pt.y() >= disp_img_.rows() )
    {
      vcl_cout << "Error: point out of range of disparity image.\n";
      return false;
    }
  else
    return true;
}

int vsrl_manager::get_disparity(vgl_point_2d<float> const& pt)
{
  vil1_memory_image_of<unsigned char> disp(disp_img_);
  int pixel_val = disp(int(pt.x()),int(pt.y()));
  if (pixel_val > 0) {
    // we subtract the disparity bias, plus 1 for the indexing offset
    //    pixel_val -= (disparity_bias_ + 1);
    pixel_val -= (params_->correlation_range + 1);
    vcl_cout << "Disparity: " << pixel_val << vcl_endl;
  }
  return pixel_val;
}

bool vsrl_manager::put_points()
{
  unsigned r=0, c=0;
  grid_->get_last_selected_position(&c, &r);

  // determine which grids to update
  vgl_point_2d<float> pos;
  // Point Picked in Left Pane
  if (c==0)
    {
      pos = vpicker0_->get_point();  // get the last point picked
      vcl_cout << "put_points: pos = " << pos << vcl_endl;
      if (!validate_point(pos)) return true;
      int disp = get_disparity(pos);
      vpicker1_->put_point(pos.x()+disp,pos.y());
      vpicker2_->put_point(pos.x(),     pos.y());
    }
  // Point Picked in Right Pane
  if (c==1)
    {
      pos = vpicker1_->get_point();  // get the last point picked
      if (!validate_point(pos)) return true;
      int disp = get_disparity(pos);
      vpicker0_->put_point(pos.x()-disp,pos.y());
      vpicker2_->put_point(pos.x(),     pos.y());
    }
  // Point Picked in Disparity Pane
  if (c==2)
    {
      pos = vpicker2_->get_point();  // get the last point picked
      if (!validate_point(pos)) return true;
      int disp = get_disparity(pos);
      vpicker0_->put_point(pos.x(),     pos.y());
      vpicker1_->put_point(pos.x()+disp,pos.y());
    }
  this->post_redraw();
  return true;
}

bool vsrl_manager::put_lines()
{
  unsigned r=0, c=0;
  grid_->get_last_selected_position(&c, &r);

  // determine which grids to update
  vgl_point_2d<float> pos;
  // Point Picked in Left Pane
  if (c==0)
    {
      pos = vpicker0_->get_point();  // get the last point picked
      vpicker1_->put_H_line(pos.x(),pos.y());
      vpicker2_->put_H_line(pos.x(),pos.y());
    }
  // Point Picked in Right Pane
  if (c==1)
    {
      pos = vpicker1_->get_point();  // get the last point picked
      vpicker0_->put_H_line(pos.x(),pos.y());
      vpicker2_->put_H_line(pos.x(),pos.y());
    }
  // Point Picked in Disparity Pane
  if (c==2)
    {
      pos = vpicker2_->get_point();  // get the last point picked
      vpicker0_->put_H_line(pos.x(),pos.y());
      vpicker1_->put_H_line(pos.x(),pos.y());
    }
  this->post_redraw();
  return true;
}

bool vsrl_manager::do_dense_matching()
{
  if (!imgL_ || !imgR_) return false;
  
  static float sig = 1.0;
  static float cutoff = 0.01;
  static bool smoothing = false;
  vgui_dialog gs_dialog("Gaussian Smoothing");
  gs_dialog.field("Sigma:",sig);
  gs_dialog.field("Cutoff:",cutoff);
  gs_dialog.checkbox("Perform Gaussian Smoothing",smoothing);
  if (!gs_dialog.ask()) return false;

  // If desired, we can do Gaussian smoothing.  This can be handy for 
  // badly interlaced images.
  if (smoothing) {
    vil1_image left = vepl_gaussian_convolution(imgL_,sig,cutoff);
    vil1_image right = vepl_gaussian_convolution(imgR_,sig,cutoff);
    imgL_=left;
    imgR_=right;
  }

  itabL_->set_image(imgL_);
  itabR_->set_image(imgR_);

  // The parameters used will be the default parameters or
  // the parameters loaded manually from a file.
  // Now create a dense matcher with the images that are loaded.
  vcl_cout << "Begin Stereo Dense Matcher...";
  vsrl_stereo_dense_matcher matcher(imgL_,imgR_);
  vcl_cout << "Setting Correlation Range...";
  matcher.set_correlation_range(params_->correlation_range);
  vcl_cout << "Running Dense Matcher.\n";
  // Run the dense matcher.
  matcher.execute();

  // Get & display the disparity image
  // Get a buffer the size of the left image
  vil1_memory_image_of<unsigned char> buffer(imgL_.cols(),imgL_.rows());
  // Zero out the buffer
  for (int x=0;x<buffer.width();x++)
    for (int y=0;y<buffer.height();y++)
      buffer(x,y)=0;
  // Get the disparities into the buffer
  for (int y=0;y<buffer.height();y++) {
    for (int x=0;x<buffer.width();x++) {
      int disparity = matcher.get_disparity(x,y);
      int value = disparity + params_->correlation_range+1;
      if (value < 0)
        value = 0;
      if (value>2*params_->correlation_range+1)
        value=0;
      buffer(x,y)=value;
    }
  }

  // Display the disparity image
  disp_img_ = buffer;
  vil1_image scaled_image = scale_image(buffer);
  dimg_tab_->set_image(scaled_image);

  vcl_cout << "Dense Matcher complete.\n";
  this->post_redraw();
  return true;
}

vil1_image vsrl_manager::scale_image(vil1_memory_image_of<unsigned char> img)
{
  double maxval = 0;
  double minval = 1e15;
  for (int x=0;x<img.width();x++) {
    for (int y=0;y<img.height();y++) {
      if (img(x,y) > maxval) {
        maxval = img(x,y);
      }
      if (img(x,y) < minval) {
        minval = img(x,y);
      }
    }
  }
  vcl_cout << "vsrl_manager::scale_image<unsigned char> - Max = " << maxval
           << "  Min = " << minval << vcl_endl;

  double scale = 255.0/maxval;
  double shift = 0;
  vil1_image scaled_image = vil1_scale_intensities(img, scale, shift);
  return scaled_image;
}

vil1_image vsrl_manager::scale_image(vil1_memory_image_of<double> img)
{
  double maxval = 0;
  double minval = 1e15;
  for (int x=0;x<img.width();x++) {
    for (int y=0;y<img.height();y++) {
      if (img(x,y) > maxval) {
        maxval = img(x,y);
      }
      if (img(x,y) < minval) {
        minval = img(x,y);
      }
    }
  }
  vcl_cout << "vsrl_manager::scale_image<double> - Max = " << maxval
           << "  Min = " << minval << vcl_endl;

  double scale = 255.0/maxval;
  double shift = 0;
  vil1_image scaled_image = vil1_scale_intensities(img, scale, shift);
  return scaled_image;
}

void vsrl_manager::find_regions()
{
  this->clear_all();
  static bool debug = false;
  static bool agr = true;
  static bool residual = false;
  static sdet_detector_params dp;
  vgui_dialog region_dialog("Edgel Regions");
  region_dialog.field("Gaussian sigma", dp.smooth);
  region_dialog.field("Noise Threshold", dp.noise_multiplier);
  region_dialog.checkbox("Automatic Threshold", dp.automatic_threshold);
  region_dialog.checkbox("Agressive Closure", agr);
  region_dialog.checkbox("Compute Junctions", dp.junctionp);
  region_dialog.checkbox("Debug", debug);
  region_dialog.checkbox("Residual Image", residual);
  region_dialog.field("Shadow Mean Intensity Threshold", shadow_mean_);
  region_dialog.checkbox("Display Only Shadow Regions", shadows_only_);
  if (!region_dialog.ask())
    return;
  if (agr)
    dp.aggressive_junction_closure=1;
  else
    dp.aggressive_junction_closure=0;

  sdet_region_proc_params rpp(dp, true, debug, 2);
  sdet_region_proc rp(rpp);
  rp.set_image(imgL_);
  rp.extract_regions();
  if (debug)
    {
      vil1_image ed_img = rp.get_edge_image();
      vgui_image_tableau_sptr itab =  e2d0_->get_image_tableau();
      if (!itab)
        {
          vcl_cout << "In segv_segmentation_manager::regions() - null image tableau\n";
          return;
        }
      itab->set_image(ed_img);
    }
  if (!debug)
    {
      vcl_vector<vtol_intensity_face_sptr>& regions = rp.get_regions();
      this->find_shadows(regions);
      this->draw_regions(regions, true);
    }
  if (residual)
    {
      vil1_image res_img = rp.get_residual_image();
      vgui_image_tableau_sptr itab =  e2d0_->get_image_tableau();
      if (!itab)
        {
          vcl_cout << "In segv_segmentation_manager::regions() - null image tableau\n";
          return;
        }
      itab->set_image(res_img);
    }
}

void vsrl_manager::draw_regions(vcl_vector<vtol_intensity_face_sptr>& regions,
                                bool verts)
{
  // This segment of code is ripped from various places in brl...bgui.
  
  vcl_vector<float>::iterator sm = shadow_metric_->begin();

   for (vcl_vector<vtol_intensity_face_sptr>::iterator rit = regions.begin();
        rit != regions.end(); rit++)
     {
       vtol_face_2d_sptr f = (*rit)->cast_to_face_2d();
       edge_list* edges = f->edges();

       vgui_soview2D_group* vsovg = new vgui_soview2D_group();

       for (edge_list::iterator eit = edges->begin(); eit != edges->end(); eit++) {

         vtol_edge_2d_sptr e = (*eit)->cast_to_edge_2d();

         vgui_soview2D_linestrip* e_line = new vgui_soview2D_linestrip();

         vsol_curve_2d_sptr c = e->curve();

         if (!c) {
           vcl_cout << "vsrl_manager::draw_regions - null curve.\n";
           return;
         }
         if (c->cast_to_digital_curve())
           {
             vdgl_digital_curve_sptr dc = c->cast_to_digital_curve();
             //get the edgel chain
             vdgl_interpolator_sptr itrp = dc->get_interpolator();
             vdgl_edgel_chain_sptr ech = itrp->get_edgel_chain();

             //n, x, and y are in the parent class vgui_soview2D_linestrip
             e_line->n = ech->size();
             //offset the coordinates for display (may not be needed)
             e_line->x = new float[e_line->n], e_line->y = new float[e_line->n];
             for (unsigned int i=0; i<e_line->n;i++)
               {
                 vdgl_edgel ed = (*ech)[i];
                 e_line->x[i]=ed.get_x();
                 e_line->y[i]=ed.get_y();
               }
           }
         else {
           vcl_cout << "vsrl_manager::draw_regions -"
                    << " attempt to draw an edge with unknown curve geometry\n";
         }

         vsovg->ls.push_back(e_line);
       }

       e2d0_->add(vsovg);

       if (verts)
         {
           vcl_vector<vtol_vertex_sptr>* vts = f->vertices();
           for (vcl_vector<vtol_vertex_sptr>::iterator vit = vts->begin();
                vit != vts->end(); vit++)
             {
               vtol_vertex_2d_sptr v = (*vit)->cast_to_vertex_2d();
               e2d0_->add_point(v->x(),v->y());
             }
           delete vts;
         }
     }
}

void
vsrl_manager::set_params()
{
  // establish the variables.  Should already have instantiated params_
  int corr_range=params_->correlation_range;
  double inner_cost=params_->inner_cost;
  double outer_cost=params_->outer_cost;
  double continuity_cost=params_->continuity_cost;
  int correlation_window_width=params_->correlation_window_width;
  int correlation_window_height=params_->correlation_window_height;
  double bias_cost=params_->bias_cost;
  double common_intensity_diff=params_->common_intensity_diff;

  vgui_dialog params_dialog("Dense Matcher Parameters");
  params_dialog.field("Correlation Range:", corr_range);
  params_dialog.field("Inner Cost:", inner_cost);
  params_dialog.field("Outer Cost:", outer_cost);
  params_dialog.field("Continuity Cost:",continuity_cost);
  params_dialog.field("Correlation Window Width:", correlation_window_width);
  params_dialog.field("Correlation Window Height:", correlation_window_height);
  params_dialog.field("Bias Cost:",bias_cost);
  params_dialog.field("Common Intensity Difference:",common_intensity_diff);
  if (!params_dialog.ask()) {
    return;
  }
  else {
    params_->correlation_range = corr_range;
    params_->inner_cost = inner_cost;
    params_->outer_cost = outer_cost;
    params_->continuity_cost = continuity_cost;
    params_->correlation_window_width = correlation_window_width;
    params_->correlation_window_height = correlation_window_height;
    params_->bias_cost = bias_cost;
    params_->common_intensity_diff = common_intensity_diff;
  }
  return;
}

void vsrl_manager::draw_north_arrow()
{
  // use a vector of length 10 for the north arrow
  north_.set(0.0,10.0);

  // (0,0) is the origin for the north vector
  /* vgui_soview2D_lineseg* line = */ draw_vector_at(&north_, 0.0f, 0.0f, 0.0);

  this->post_redraw();
  return;
}

//
// Draw the given vector at the specified point with the specified rotation.
// (x,y) = where to draw the vector
// theta = the angle through which the vector is rotated before drawing.
//
vgui_soview2D_lineseg*
vsrl_manager::draw_vector_at(vgl_vector_2d<float>* vec, float x, float y, float theta)
{
  // make the vector green
  e2d0_->set_foreground(0,1,0);

  // Apply rotation to get rot_vec
  vgl_vector_2d<float> rot_vec;
  rot_vec.x_ =  vec->x() * vcl_cos(theta) + vec->y() * vcl_sin(theta);
  rot_vec.y_ = -vec->x() * vcl_sin(theta) + vec->y() * vcl_cos(theta);

  // Apply translation
  float endx = rot_vec.x()+x;
  float endy = rot_vec.y()+y;

  // Create the lineseg & draw it
  vgui_soview2D_lineseg* vector = new vgui_soview2D_lineseg(x,y,endx,endy);
  e2d0_->add(vector);

  e2d0_->set_foreground(1,0,0);
  this->post_redraw();
  return vector;
}

// Calculate & Display Image Gradient magnitudes
//
vil1_image
vsrl_manager::show_gradient_mag(vil1_image* im_in)
{
  vil1_image im_out = vepl_gradient_mag(*im_in);
  disp_img_ = im_out;  // this line lets us save out the image
  vil1_memory_image_of<unsigned char> tmp(im_out);
  vil1_image scaled_image = scale_image(tmp);
  dimg_tab_->set_image(scaled_image);
  this->post_redraw();
  return im_out;
}

// Calculate & Display Image Gradient directions
//
vil1_image
vsrl_manager::show_gradient_dir(vil1_memory_image_of<double> im_in)
{
  // calculate the gradient
  // I prefer to implement my own gradient operation because I want to handle the edges
  // more carefully and I want better control of what happens at inflection points of ATAN2.
  // The scale (40) and offset (128) keep the range of the ATAN2 function within the range of
  // the <unsigned char> image.
  vcl_cout << "vsrl_manager::show_gradient_dir() - Begin\n";

  vil1_memory_image_of<double> im_out(im_in.width(),im_in.height());

  const double shift=128;
  const double scale=40; // actually: 127/pi
  register double dx, dy;
  for (int x=0; x<im_out.width(); x++) {
    for (int y=0; y<im_out.height(); y++) {
      if (x==0) {
        dx = im_in(x+1,y) - im_in(x,y); // image edge
      }
      else {
        dx = im_in(x,y) - im_in(x-1,y);
      }
      if (y==0) {
        dy = im_in(x,y+1) - im_in(x,y); // image edge
      }
      else {
        dy = im_in(x,y) - im_in(x,y-1);
      }
      im_out(x,y) = vcl_atan2(dy,dx) * scale + shift;
    }
  }

  vil1_image scaled_image = scale_image(im_out);
  dimg_tab_->set_image(scaled_image);
  this->post_redraw();
  vcl_cout << "vsrl_manager::show_gradient_dir() - End\n";
  return im_out;
}

// Generic function called by menu to make testing easier
void
vsrl_manager::test_left_func()
{
  this->boundary_matching();
  return;
}

// Generic function called by menu to make testing easier
void
vsrl_manager::test_right_func()
{
  // get the regions from jseg algorithm
  vcl_vector<vdgl_digital_region*> regions = run_jseg(imgR_);
  find_shadows(regions);
  this->post_redraw();
  return;
}

// Generate 3D output & display range image
vil1_memory_image_of<double>
vsrl_manager::make_3d()
{
  // set up the dense matcher
  if (!disp_img_ || !imgL_ || !imgR_) return NULL; // Sanity check.
  vsrl_results_dense_matcher matcher(imgL_,disp_img_);
  matcher.set_correlation_range(params_->correlation_range);
  vsrl_3d_output output(imgL_,imgR_);
  output.set_matcher(&matcher);
  //  Write the 3D output & triangles to out.dat
  output.write_output("out.dat");
  vil1_image scaled_image = scale_image(output.range_image_);
  itabR_->set_image(scaled_image);  // put the image in the viewer
  this->post_redraw();
  return output.range_image_;
}

// Make a vector of digital regions and pass the problem to
// find_shadows(vcl_vector<vdgl_digital_region*>) below.
//
void vsrl_manager::find_shadows(vcl_vector<vtol_intensity_face_sptr>& faces)
{
  vcl_vector<vdgl_digital_region*> reg_vec;

  for (vcl_vector<vtol_intensity_face_sptr>::iterator fit = faces.begin();
       fit != faces.end(); fit++) {
    vdgl_digital_region* reg = (*fit)->cast_to_digital_region();
    reg_vec.push_back(reg);
  }
  find_shadows(reg_vec);
  return;
}

//--  This algorithm taken pretty much verbatim from the DDB/RegionSaliency class.
//    Repeated here for digital regions instead of intensity faces...
//

void vsrl_manager::find_shadows(vcl_vector<vdgl_digital_region*> regions)
{
  // Create an array to hold shadow metric
  // (First get rid of anything there already)
  if (shadow_metric_ != NULL) delete shadow_metric_;
  shadow_metric_ = new vcl_vector<float>(regions.size());

  e2d1_->set_foreground(0,0,1);
  e2d1_->set_point_radius(5);
  
  int i=0;
  
  vcl_vector<float>::iterator sm = shadow_metric_->begin();
  vcl_cout << "Shadow Threshold: " << shadow_mean_ << vcl_endl;
  
  for (vcl_vector<vdgl_digital_region*>::iterator rit = regions.begin();
       rit != regions.end(); rit++) {
    
    if ((*rit)->Npix() > 0) {
      (*rit)->ComputeIntensityStdev();
      vcl_cout << "Intensity Face: " << i << "  Io = " << (*rit)->Io()
               << "  I_stdev = " << (*rit)->Io_sd();
      if ((*rit)->Io() == 0) {
        *sm=0.0;
      }
      else if ((*rit)->Io() < shadow_mean_) {
        *sm=1.0;
      }
      else {
        
        // This segment of code is taken from the HistEntropy class
        // in TargetJr, GeneralUtility/Basics.  It is used to mimic
        // the operation of the shadow detection used in the DDB
        // RegionSaliency class.
        
        float m1=(*rit)->Io(); // Get the region mean
        float v1=(*rit)->Io_sd(); // Get the region Stdev.
        
        // Later we can adjust the shadow reference parameters.
        
        // For now, this is what we'll use...
        float m2 = shadow_mean_; // m2 is the reference shadow mean.
        float v2 = 1.0; // v2 is the reference shadow stdev.
        
        if (m1==0 || m2==0) {
          *sm=0.0;
        }
        else if ( (v1 < 1e-6 ) || (v2 < 1e-6) ) {
          *sm=0;
        }
        else {
          *sm = exp(- fabs( 0.693 * (m1 - m2) * sqrt(1.0/(v1*v1) + 1.0/(v2*v2))));
        }
      }
      vcl_cout << "  Shadow = " << *sm << vcl_endl;
    }
    
    if (*sm ==1) {
      e2d1_->add_point((*rit)->Xo(),(*rit)->Yo());
    }

    sm++; // move to the next face
    i++;
  }
  e2d1_->set_foreground(1,0,0);
  e2d1_->set_point_radius(5);

}

// Though called "run_jseg" this routine does more than that.  After the jseg routine
// executes, the output of jseg is partitioned into digital_regions and a vector of
// the resulting digital regions is returned.

vcl_vector<vdgl_digital_region*> 
vsrl_manager::run_jseg(vil1_image image_in)
{
  // Set up the JSEG parameters
  static float TQUAN=-1.0;
  static int NSCALE=-1;
  static float threshcolor=-1.0;
  vgui_dialog jseg_dialog("JSEG Parameters");
  jseg_dialog.field("Color Quantization Threshold (-1=automatic): ",TQUAN);
  jseg_dialog.field("Number of Scales (-1=automatic): ",NSCALE);
  jseg_dialog.field("Region Merge Threshold (-1=automatic): ",threshcolor);
  jseg_dialog.field("Shadow Mean Intensity Threshold: ", shadow_mean_);
  if (!jseg_dialog.ask()) return NULL;

  // Get the image for segmentation
  vil1_memory_image_of<unsigned char> im(image_in);
  unsigned char* cp = im.get_buffer();
  int nx = im.cols();
  int ny = im.rows();
  int dim = 1; // grayscale image

  // Run JSEG
  unsigned char* js_img = jseg(cp,dim,nx,ny,TQUAN,NSCALE,threshcolor);
  vil1_memory_image_of<unsigned char>* js_out = new vil1_memory_image_of<unsigned char>(js_img,nx,ny);

  // Now we have an "image" of the regions.  We need to sort it into
  // vdgl_digital_regions. We'll hold the regions in a vector.

  vcl_vector<vdgl_digital_region*> reg_vec;

  int reg_cntr = 0;  // A counter for regions

  // Initialize things with one region
  // We'll use a 3D region and use the Z coord. for the class label.
  vcl_cout << "Creating initial region.  #0" << vcl_endl;
  vdgl_digital_region* region1 = new vdgl_digital_region();

  // vdgl_digital_region(x,y,z,pix)
  // x & y are pixel coords. z is class label. pix=Intensity value of orignal image pixel.
  region1->IncrementMeans(0,0,(*js_out)(0,0),im(0,0));
  reg_vec.push_back(region1);  // Add the region to the vector.

  // Loop through all the image pixels, assigning them to regions
  // This pass only counts the pixels in regions and calculates their means.
  // Another pass is required later to actually assign inividual pixels to
  // their respective regions.
  // (See vdgl_digital_region::IncrementMeans(float,float,float,unsigned short)
  // for a description.
  for (int x=0;x<im.cols();x++) {
    for (int y=0;y<im.rows();y++) {
      if (x==0 && y==0) break; // we've already done pixel (0,0)
      unsigned char reg_num = (*js_out)(x,y); // get the region class number for this pixel

      // Now loop through the regions and see if we already have a region of
      // this class.
      bool found = false;  // use this flag to determine if we find a match
      for (vcl_vector<vdgl_digital_region*>::iterator rit = reg_vec.begin();
           rit<reg_vec.end();rit++){
        if (reg_num == (*rit)->Zo()) {
          // Found a matching region so put this pixel in the region
          (*rit)->IncrementMeans(x,y,reg_num,im(x,y));
          found = true;
          break; // When you find a match, don't keep looking, just quit.
        }
      }
      // We've gone through all the regions. If we didn't find a match
      // for this pixel, make a new region and insert it.
      if (!found) {
        reg_cntr++;
        vcl_cout << "Creating new region.  #" << reg_cntr << vcl_endl;
        vdgl_digital_region* new_region = new vdgl_digital_region();
        new_region->IncrementMeans(x,y,reg_num,im(x,y));
        reg_vec.push_back(new_region);
        found=false; //reset the flag
      }
    }
  }
  // Initialize the regions to accept pixels...
  for (vcl_vector<vdgl_digital_region*>::iterator rit = reg_vec.begin();
       rit<reg_vec.end();rit++){
    (*rit)->InitPixelArrays();
  }
  // Loop through all the image pixels again,
  for (int x=0;x<im.cols();x++) {
    for (int y=0;y<im.rows();y++) {
      unsigned char reg_num = (*js_out)(x,y); // get the region class number for this pixel
      // Now loop through the regions make pixel assignments
      for (vcl_vector<vdgl_digital_region*>::iterator rit = reg_vec.begin();
           rit<reg_vec.end();rit++){
        if (reg_num == (*rit)->Zo()) {
          // Found a matching region so put this pixel in the region
          (*rit)->InsertInPixelArrays(x,y,reg_num,im(x,y));
          break; // When you find a match, don't keep looking, just quit.
        }
      }
      // We've gone through all the regions. If we didn't find a match
      // for this pixel, make a new region and insert it.
    }
  }
  vcl_cout << "Finished Creating Regions." << vcl_endl;
  // Overlay the regions on the original image
  if (image_in == imgL_){
    show_jseg_boundaries(js_out, e2d0_);
  }
  else if (image_in == imgR_) {
    show_jseg_boundaries(js_out, e2d1_);
  }
  // Scale & Display the results
  vil1_image scaled_image = scale_image(*js_out);
  disp_img_ = *js_out;
  dimg_tab_->set_image(scaled_image);
  this->post_redraw();
  return reg_vec;
}

// This routine just puts dotted lines (o.k. points) around the boundaries of the
// regions in the jseg segmentation.
void vsrl_manager::show_jseg_boundaries(vil1_memory_image_of<unsigned char>* jseg_out,
                                        vgui_easy2D_tableau_sptr tab)
{
  if (tab == e2d0_) {
    e2d0_->set_foreground(1,0,0);    
    e2d2_->set_foreground(1,0,0);    
  }
  if (tab == e2d1_) {
    e2d1_->set_foreground(0,1,0);    
    e2d2_->set_foreground(0,1,0);    
  }

  vil1_memory_image_of<unsigned char> rimg(jseg_out->width(),jseg_out->height());
  
  tab->set_point_radius(1);
  e2d2_->set_point_radius(1);
  for (int x=0;x<jseg_out->cols()-1;x++) {
    for (int y=0;y<jseg_out->rows()-1;y++) {
      rimg(x,y) = 0;
      // Vertical Boundaries...
      if ( (*jseg_out)(x,y) != (*jseg_out)(x+1,y) ){
        tab->add_point(x+0.5,y);
        e2d2_->add_point(x+0.5,y);
        rimg(x,y) = 255;
      }
      // Horizontal Boundaries...
      if ( (*jseg_out)(x,y) != (*jseg_out)(x,y+1) ) {
        tab->add_point(x,y+0.5);
        e2d2_->add_point(x,y+0.5);
        rimg(x,y) = 255;
      }
    }
  }
  vcl_string fname="regions.tif";
  vil1_save(rimg,fname.c_str());
  this->post_redraw();
  e2d0_->set_foreground(1,0,0);    
  e2d1_->set_foreground(1,0,0);    
  e2d2_->set_foreground(1,0,0);    
  tab->set_point_radius(5);
  e2d2_->set_point_radius(5);
}

// This routine is to display the results of the correlations used for dense matching
//

float* vsrl_manager::show_correlations(int x, int y)
{
  // Don't try anything funny.
  if (!imgL_ || !imgR_) {
    vcl_cerr << "vsrl_manager::show_correlations() -> 2 images not loaded. Abort."
             << vcl_endl;
    return NULL;
  }

  // Determine which grid pane was used...
  unsigned r=0, c=0;
  grid_->get_last_selected_position(&c,&r);

  // Make 'em pick the point in the left pane...
  if (c!=0) {
    vcl_cerr << "vsrl_manager::show_correlations() -> "
             << "Please pick point in left pane. "
             << vcl_endl;
    return NULL;
  }

  // Setup the correlation parameters...

  vsrl_image_correlation corr(imgL_, imgR_);
  corr.set_window_width(params_->correlation_window_width);
  corr.set_window_height(params_->correlation_window_height);
  corr.set_correlation_range(params_->correlation_range);

  // Get the correlation value at the point(s) in question
  vgl_point_2d<float> pos;
  pos = vpicker0_->get_point();
  int range = params_->correlation_range;
  float* results = new float[(2*range)+1];
  vcl_cout << "Correlation results about point: " << x << ", " << y << vcl_endl;
  vcl_cout << "X: Y: R:" << vcl_endl;
  for (int xo=-range; xo<=range; xo++) {
    results[xo+range] = corr.get_correlation(x,y,(x+xo),y);
    vcl_cout << (x+xo) << "  " << y << "  " << results[xo+range] << vcl_endl;
  }

  e2d0_->add_point(pos.x(),pos.y());
  e2d1_->add_point(pos.x(),pos.y());
  e2d1_->add_line(pos.x()-range,pos.y(),pos.x()+range,pos.y());

  return results;
}

void vsrl_manager::raw_correlation()
{
  if (!imgL_ || !imgR_) return;
  
  // Gaussian Smoothing (if needed)...
  static float sig = 1.0;
  static float cutoff = 0.01;
  static bool smoothing = false;
  vgui_dialog gs_dialog("Gaussian Smoothing");
  gs_dialog.field("Sigma:",sig);
  gs_dialog.field("Cutoff:",cutoff);
  gs_dialog.checkbox("Perform Gaussian Smoothing",smoothing);
  if (!gs_dialog.ask()) return;
  if (smoothing) {
    vil1_image left = vepl_gaussian_convolution(imgL_,sig,cutoff);
    vil1_image right = vepl_gaussian_convolution(imgR_,sig,cutoff);
    imgL_=left;
    imgR_=right;
  }
  itabL_->set_image(imgL_);
  itabR_->set_image(imgR_);

  // Set up for doing the correlations...
  vsrl_image_correlation i_corr(imgL_,imgR_ );
  i_corr.set_correlation_range(params_->correlation_range);
  i_corr.set_window_width(params_->correlation_window_width);
  i_corr.set_window_height(params_->correlation_window_height);

  // Get an appropriately sized image buffer into which to write results...
  vil1_memory_image_of<unsigned char> disp(imgL_.width(),imgL_.height());

  int range = params_->correlation_range;
  for (int y=0; y<imgL_.rows(); y++) {
    vcl_cout << "Row: " << y << vcl_endl;
    for (int x=0; x<imgL_.cols(); x++) {
      float result=0;
      float max=-1e10;
      // when the disparities are written to the buffer, they are offset by "range"
      // to keep them positive.
      disp(x,y) = range; // default value (i.e. disparity=0+offset)
      for (int i=-range; i<=range;i++) {
        // get the values of the correlation of the pixel in the left
        // image with several pixels in the right image.
        result = i_corr.get_correlation(x,y,x+i,y);
        if (result > max) {
          max = result; // if we get a new peak, update the max value...
          disp(x,y) = i+range; // ...and put the new disparity in the disparity buffer
        }
      }
    }
  }
  vil1_image scaled_image = scale_image(disp);
  disp_img_ = disp;
  dimg_tab_->set_image(scaled_image);
  this->post_redraw();
  return;
}

// This routine makes images of the boundaries found through JSEG
// segmentation.

vil1_image* vsrl_manager::make_jseg_image(vil1_memory_image_of<unsigned char>* jseg_out)
{
  vil1_memory_image_of<unsigned char>* b_image = 
    new vil1_memory_image_of<unsigned char>(jseg_out->width(),jseg_out->height());
  for (int y=0;y<jseg_out->rows()-1;y++) {
    for (int x=0;x<jseg_out->cols()-1;x++) {
      (*b_image)(x,y) = 0;  // clear out the pixels first
      // Horizontal Boundaries
      if ( (*jseg_out)(x,y) != (*jseg_out)(x+1,y) ) {
        (*b_image)(x,y) = 255;
      }
      // Vertical Boundaries
      if ( (*jseg_out)(x,y) != (*jseg_out)(x,y+1) ) {
        (*b_image)(x,y) = 255;
      }
    }
  }
  return b_image;
}
// boundary_matching: This routine takes two images, computes jseg segmentation, creates
// two "boundary" images, then uses these images to attempt "dense matching".
void vsrl_manager::boundary_matching()
{
  // Set up the JSEG parameters
  static float TQUAN=-1.0;
  static int NSCALE=-1;
  static float threshcolor=-1.0;
  vgui_dialog jseg_dialog("JSEG Parameters");
  jseg_dialog.field("Color Quantization Threshold (-1=automatic): ",TQUAN);
  jseg_dialog.field("Number of Scales (-1=automatic): ",NSCALE);
  jseg_dialog.field("Region Merge Threshold (-1=automatic): ",threshcolor);
  jseg_dialog.field("Shadow Mean Intensity Threshold: ", shadow_mean_);
  if (!jseg_dialog.ask()) return;
  // Get the image for segmentation
  // Left image first...
  vil1_memory_image_of<unsigned char> imL(imgL_);
  unsigned char* cp = imL.get_buffer();
  int nx = imL.cols();
  int ny = imL.rows();
  int dim = 1; // grayscale image

  // Run JSEG
  unsigned char* js_imgL = jseg(cp,dim,nx,ny,TQUAN,NSCALE,threshcolor);
  vil1_memory_image_of<unsigned char>* js_out_L = new vil1_memory_image_of<unsigned char>(js_imgL,nx,ny);

  // Now for the Right image...
  vil1_memory_image_of<unsigned char> imR(imgR_);
  cp = imR.get_buffer();
  nx = imR.cols();
  ny = imR.rows();
  unsigned char* js_imgR = jseg(cp,dim,nx,ny,TQUAN,NSCALE,threshcolor);
  vil1_memory_image_of<unsigned char>* js_out_R = new vil1_memory_image_of<unsigned char>(js_imgR,nx,ny);

  // We've now jseg-ed 2 images. Now get the boundary images.

  vil1_image* bi_L = make_jseg_image(js_out_L);
  vil1_image* bi_R = make_jseg_image(js_out_R);

  // Display images
  itabL_->set_image(*bi_L);
  itabR_->set_image(*bi_R);
  imgL_ = *bi_L;
  imgR_ = *bi_R;
  this->post_redraw();

  vcl_cout << "Beginning dense matching..." << vcl_endl;

  vsrl_stereo_dense_matcher matcher(*bi_L,*bi_R);
  matcher.set_correlation_range(params_->correlation_range);
  matcher.execute();

  // Get & display the disparity image
  // Get a buffer the size of the left image
  vil1_memory_image_of<unsigned char> buffer(imgL_.cols(),imgL_.rows());
  // Zero out the buffer
  for (int x=0;x<buffer.width();x++)
    for (int y=0;y<buffer.height();y++)
      buffer(x,y)=0;
  // Get the disparities into the buffer
  for (int y=0;y<buffer.height();y++) {
    for (int x=0;x<buffer.width();x++) {
      int disparity = matcher.get_disparity(x,y);
      int value = disparity + params_->correlation_range+1;
      if (value < 0)
        value = 0;
      if (value>2*params_->correlation_range+1)
        value=0;
      buffer(x,y)=value;
    }
  }

  // Display the disparity image
  disp_img_ = buffer;
  vil1_image scaled_image = scale_image(buffer);
  dimg_tab_->set_image(scaled_image);

  vcl_cout << "Dense Matcher complete.\n";
  this->post_redraw();
  return;
}
