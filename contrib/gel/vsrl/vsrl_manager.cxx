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
#include <vsrl/vsrl_region_disparity.h>

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
  if (!vil1_save(disp,image_filename.c_str()))
    vcl_cout << "Error saving disparity image!\n";
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
  if (pixel_val == 0)
    vcl_cout << "No disparity mapping found for this point!\n";
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
  vil1_memory_image_of<unsigned char> buffer(imgL_);
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
  dp.noise_multiplier=1.0f;
  vgui_dialog region_dialog("Edgel Regions");
  region_dialog.field("Gaussian sigma", dp.smooth);
  region_dialog.field("Noise Threshold", dp.noise_multiplier);
  region_dialog.checkbox("Automatic Threshold", dp.automatic_threshold);
  region_dialog.checkbox("Agressive Closure", agr);
  region_dialog.checkbox("Compute Junctions", dp.junctionp);
  region_dialog.checkbox("Debug", debug);
  region_dialog.checkbox("Residual Image", residual);
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
         vcl_cout << "vsrl_manager::draw_regions - attempt to draw an edge with unknown curve geometry\n";
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
  north_.set(0.0f,10.0f);

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
  vil1_memory_image_of<double> img = make_3d();
  show_gradient_dir(img);
  return;
}

// Generic function called by menu to make testing easier
void
vsrl_manager::test_right_func()
{
  vil1_memory_image_of<double> img = make_3d();
  show_gradient_dir(img);
  return;
}

// Generate 3D output & display range image
vil1_memory_image_of<double>
vsrl_manager::make_3d()
{
  // set up the dense matcher
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
// This routine is to display the results of the correlations used for dense matching
//

float* vsrl_manager::show_correlations(int x, int y)
{
  // Don't try anything funny.
  if (!imgL_ || !imgR_) {
    vcl_cerr << "vsrl_manager::show_correlations() -> 2 images not loaded. Abort.\n";
    return NULL;
  }

  // Determine which grid pane was used...
  unsigned r=0, c=0;
  grid_->get_last_selected_position(&c,&r);

  // Make 'em pick the point in the left pane...
  if (c!=0) {
    vcl_cerr << "vsrl_manager::show_correlations() -> Please pick point in left pane.\n";
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
  vcl_cout << "Correlation results about point: " << x << ", " << y << vcl_endl
           << "X: Y: R:\n";
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
  static float sig = 1.0f;
  static float cutoff = 0.01f;
  static bool smoothing = false;
  vgui_dialog gs_dialog("Gaussian Smoothing");
  gs_dialog.field("Sigma:",sig);
  gs_dialog.field("Cutoff:",cutoff);
  gs_dialog.checkbox("Perform Gaussian Smoothing",smoothing);
  //  if (!gs_dialog.ask()) return;
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
  vcl_cout << vcl_endl;
  for (int y=0; y<imgL_.rows(); y++) {
    vcl_cout << "\rRow: " << y;
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
  vcl_cout << vcl_endl;
  disp_img_ = disp;
  vil1_image scaled_image = scale_image(disp);
  dimg_tab_->set_image(scaled_image);
  this->post_redraw();
  return;
}

void vsrl_manager::region_disparity()
{
  // This method is currently ifdef'd out because of its
  // dependency on JSEG.  Later this method will be included by
  // either including the JSEG algorithm or by proving out
  // the use of other region segmentation algorithms.
#ifdef INCLUDE_JSEG
  // First do segmentation - one way or another
  vcl_vector<vdgl_digital_region*> dregs = run_jseg(imgL_);  // JSEG method

  // Next, find disparity on a pixel-by-pixel basis
  // raw correlation is used because the region segmentation should obviate
  // the dynamic program.
  this->raw_correlation();

  // Find disparity using regions
  vsrl_region_disparity r_disp(&imgL_, &imgR_);
  vil1_memory_image_of<unsigned char> disp(disp_img_);
  vcl_cout << "vsrl_manager::region_disparity\n";
  r_disp.SetDisparityImage(&disp);
  r_disp.SetRegions(&dregs);
  r_disp.Execute();

  // Display results
  vil1_memory_image_of<double>* d2 = r_disp.GetRegionDisparities();
  // As nice as it is to have these floating point results, the rest of the code
  // assumes everything is <unsigned char> **sigh**.  So we convert...
  //
  for (int c=0; c<disp_img_.width(); c++) {
    for (int r=0; r<disp_img_.height(); r++) {
      disp(c,r)= (*d2)(c,r);
    }
  }

  dimg_tab_->set_image(scale_image(disp));
  this->post_redraw();
#endif
  return;
}
