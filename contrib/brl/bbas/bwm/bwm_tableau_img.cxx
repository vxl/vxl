#include "bwm_tableau_img.h"
#include "bwm_tableau_text.h"
#include "bwm_command_macros.h"
#include "bwm_observer_mgr.h"
#include "bwm_tableau_mgr.h"
#include "bwm_popup_menu.h"
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_vector_io.h>
#include <vil/vil_save.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_digital_curve_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_oriented_box_2d.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_command.h>


void bwm_tableau_img::get_popup(vgui_popup_params const &params, vgui_menu &menu)
{
  menu.clear();

  bwm_popup_menu pop(this);
  pop.get_menu(menu);
}

void bwm_tableau_img::lock()
{
  my_observer_->image_tableau()->lock_linenum(true);
  my_observer_->lock_vgui_status(true);
}

void bwm_tableau_img::unlock()
{
  my_observer_->lock_vgui_status(false);
  if (!my_observer_->vgui_status_on())
    my_observer_->image_tableau()->lock_linenum(false);
}

void bwm_tableau_img::create_box()
{
  // first lock the bgui_image _tableau
  this->lock();
  set_color(1, 0, 0);
  float x1, y1, x2, y2;
  pick_box(&x1, &y1, &x2, &y2);
  vsol_box_2d_sptr box2d = new vsol_box_2d();
  box2d->add_point(x1, y1);
  box2d->add_point(x2, y2);
  this->unlock();
  // add the box to the list
  my_observer_->create_box(box2d);
}

void bwm_tableau_img::create_polygon()
{
  // first lock the bgui_image _tableau
  bwm_observer_mgr::instance()->stop_corr();
  this->lock();
  vsol_polygon_2d_sptr poly2d;
  set_color(1, 0, 0);
  pick_polygon(poly2d);
  if (!poly2d)
  {
    std::cerr << "In bwm_tableau_img::create_polygon() - picking failed\n";
    return;
  }
  this->unlock();

  // add the polygon to the list
  my_observer_->create_polygon(poly2d);
}

void bwm_tableau_img::create_polyline()
{
  // first lock the bgui_image _tableau
  this->lock();
  bwm_observer_mgr::instance()->stop_corr();

  vsol_polyline_2d_sptr poly2d;
  set_color(1, 0, 0);
  this->pick_polyline(poly2d);
  if (!poly2d)
  {
    std::cerr << "In bwm_tableau_img::create_polyline() - picking failed\n";
    return;
  }

  this->unlock();
  // add the polygon to the list
  my_observer_->create_polyline(poly2d);
}

void bwm_tableau_img::create_point()
{
  float x, y;

  set_color(1, 0, 0);
  this->pick_point(&x, &y);
  my_observer_->create_point(new vsol_point_2d(x, y));
}

void bwm_tableau_img::create_pointset()
{
  std::vector<vsol_point_2d_sptr> pts;

  bool picked = this->pick_point_set(pts, 1000);
  if (!picked) return; // failed!

  for (std::vector<vsol_point_2d_sptr>::iterator pit = pts.begin();
       pit != pts.end(); ++pit)
    my_observer_->create_point(*pit);
  this->post_redraw();
}

void bwm_tableau_img::create_vsol_spatial_object(vsol_spatial_object_2d_sptr sos)
{
  if (sos->cast_to_point()) {
    vsol_point_2d_sptr p = sos->cast_to_point();
    my_observer_->create_point(p);
  }
  else if (sos->cast_to_curve())
  {
    if (sos->cast_to_curve()->cast_to_digital_curve())
    {
      //std::cerr << "bwm_observer does not have support to add digital curve!! skipping this object!\n";
                // just for display purposes, maybe later actually "create" an interactive view (JLM)
                vsol_digital_curve_2d_sptr dc = sos->cast_to_curve()->cast_to_digital_curve();
                my_observer_->add_digital_curve(dc);
    }
    else if (sos->cast_to_curve()->cast_to_vdgl_digital_curve())
    {
      std::cerr << "bwm_observer does not have support to add vdgl digital curve!! skipping this object!\n";
    }
    else if (sos->cast_to_curve()->cast_to_line())
    {
      //vsol_line_2d_sptr line =
      //  sos->cast_to_curve()->cast_to_line();
      std::cerr << "bwm_observer does not have support to add vsol_line_2d !! skipping this object!\n";
    }
    else if (sos->cast_to_curve()->cast_to_polyline())
    {
      vsol_polyline_2d_sptr pline =
        sos->cast_to_curve()->cast_to_polyline();
      my_observer_->create_polyline(pline);
    }
    else if (sos->cast_to_curve()->cast_to_conic())
    {
      //vsol_conic_2d_sptr conic = sos->cast_to_curve()->cast_to_conic();
      // make sure the endpoints are already defined
      //assert(conic->p0() && conic->p1());
      //this->add_vsol_conic_2d(conic, style);
      std::cerr << "bwm_observer does not have support to add vsol_conic_2d_sptr !! skipping this object!\n";
    }
    else
      assert(!"unknown curve type in bgui_vsol2D_tableau::add_spatial_object()");
  }
  else if (sos->cast_to_region()) {
    if (sos->cast_to_region()->cast_to_polygon())
    {
      vsol_polygon_2d_sptr pline =
        sos->cast_to_region()->cast_to_polygon();
      my_observer_->create_polygon(pline);
    }
    else
      assert(!"unknown region type in bgui_vsol2D_tableau::add_spatial_object()");
  }
  else
    assert(!"unknown spatial object type in bgui_vsol2D_tableau::add_spatial_object()");
  return;

}

void bwm_tableau_img::copy()
{
  my_observer_->copy();
}

void bwm_tableau_img::paste()
{
  float x, y;
  this->pick_point(&x, &y);
  my_observer_->paste(x, y);
}

void bwm_tableau_img::deselect_all()
{
  my_observer_->deselect_all();
}

void bwm_tableau_img::clear_poly()
{
  my_observer_->delete_selected();
}

void bwm_tableau_img::clear_box()
{
  my_observer_->clear_box();
}

void bwm_tableau_img::clear_all()
{
  my_observer_->delete_all();
}

//if only an image tableau is loaded this will effectively
//be the same command. Only to be overridden when a video tableau is loaded.
void bwm_tableau_img::clear_all_frames()
{
  my_observer_->delete_all();
}

void bwm_tableau_img::intensity_profile()
{
  float x1, y1, x2, y2;
  this->lock();
  pick_line(&x1, &y1, &x2, &y2);
  std::cout << x1 << ',' << y1 << "-->" << x2 << ',' << y2 << std::endl;
  my_observer_->intensity_profile(x1, y1, x2, y2);
  this->unlock();
}

void bwm_tableau_img::range_map()
{
  my_observer_->range_map();
}

void bwm_tableau_img::toggle_show_image_path()
{
  my_observer_->toggle_show_image_path();
}

void bwm_tableau_img::zoom_to_fit()
{
  my_observer_->zoom_to_fit();
}

void bwm_tableau_img::scroll_to_point()
{
  my_observer_->scroll_to_point();
}

void bwm_tableau_img::save_mask()
{
  vil_image_view_base_sptr mask = my_observer_->mask();
  if (!mask)
    return;
  vgui_dialog save_dlg("Save Mask");
  std::string ext, file_path;
  save_dlg.file("Mask Filename", ext, file_path);
  if (!save_dlg.ask())
    return;
  if (file_path =="")
    return;
  bool result = vil_save(*mask,file_path.c_str());
  if ( !result ) {
    std::cerr << "Failed to save image to " << file_path << '\n';
  }
}

void bwm_tableau_img::save_spatial_objects_2d()
{
  std::vector<vsol_spatial_object_2d_sptr> sos =
    my_observer_->get_spatial_objects_2d();
  if (sos.size() == 0)
    return;
  vgui_dialog save_dlg("Save Spatial Objects 2d");
  std::string ext, binary_filename;
  save_dlg.file("Binary Filename", ext, binary_filename);
  if (!save_dlg.ask())
    return;
  if (binary_filename == "")
    return;
  vsl_b_ofstream ostr(binary_filename);
  if (!ostr) {
    std::cerr << "Failed to open output stream "
             << binary_filename << std::endl;
    return;
  }
  vsl_b_write(ostr, sos);
}
void bwm_tableau_img::load_spatial_objects_2d(){
  vgui_dialog save_dlg("Load Spatial Objects 2d");
  std::string ext, binary_filename;
  save_dlg.file("Binary Filename", ext, binary_filename);
  if (!save_dlg.ask())
    return;
  if (binary_filename == "")
    return;
  vsl_b_ifstream istr(binary_filename);
  if (!istr) {
    std::cerr << "Failed to open inputt stream "
             << binary_filename << std::endl;
    return;
  }
  std::vector<vsol_spatial_object_2d_sptr> sos;
  vsl_b_read(istr, sos);
  //my_observer_->add_spatial_objects(sos);
  for (unsigned i = 0; i < sos.size(); i++)
    this->create_vsol_spatial_object(sos[i]);
  my_observer_->post_redraw();
}

void bwm_tableau_img::save_pointset_2d_ascii()
{
  std::vector<vsol_spatial_object_2d_sptr> sos =
    my_observer_->get_spatial_objects_2d();
  if (sos.size() == 0)
    return;
  std::vector<vsol_point_2d_sptr> pts;
  for (unsigned i=0; i<sos.size(); ++i) {
    vsol_spatial_object_2d_sptr so = sos[i];
    vsol_point_2d_sptr pt = so->cast_to_point();
    if (pt)
      pts.push_back(pt);
  }
  vgui_dialog save_dlg("Save Pointset");
  std::string ext, pt_filename;
  save_dlg.file("Point Filename", ext, pt_filename);
  if (!save_dlg.ask())
    return;
  std::ofstream os(pt_filename.c_str());
  if (os.is_open()) {
    os << pts.size()<< '\n';
    for (unsigned i=0; i<pts.size(); ++i)
      os << pts[i]->x() << ' ' << pts[i]->y() << '\n';
    os.close();
  }
}
void bwm_tableau_img::load_pointset_2d_ascii()
{
  // the style toggling is just to
  // allow comparison of two or three point sets
  static float r = 0.0;
  if(r == 1.5f)
    r = 0.0f;
  vgui_style_sptr sty = vgui_style::new_style( r, 1.0f, 0.0f, 3.0, 2.0);

  vgui_dialog load_dlg("Load Pointset");
  std::string ext, pt_filename;
  load_dlg.file("Point Filename", ext, pt_filename);
  if (!load_dlg.ask())
    return;
  std::ifstream istr(pt_filename.c_str());
  if (istr.is_open()) {
    unsigned n;
    istr >> n; //number of pts
    double x, y;
    for(unsigned i = 0; i<n; ++i){
      istr >> x >> y;
      vsol_point_2d_sptr pt = new vsol_point_2d(x, y);
      my_observer_->add_vsol_point_2d(pt, sty);
    }
    istr.close();
  }
  r = r+0.5f;
}
void bwm_tableau_img::load_bounding_boxes_2d_ascii()
{
  // the style toggling is just to
  // allow comparison of two or three oriented boxes
  static float r = 0.0;
  if(r == 1.5f)
    r = 0.0f;
  vgui_style_sptr sty = vgui_style::new_style( r, 1.0f, 0.0f, 3.0, 2.0);

  vgui_dialog load_dlg("Load Axis-aligned Boxes");
  std::string ext, pt_filename;
  load_dlg.file("bbox Filename", ext, pt_filename);
  if (!load_dlg.ask())
    return;
  std::ifstream istr(pt_filename.c_str());
  if(!istr.is_open()){
    std::cout << "Failed to open " << pt_filename << std::endl;
    return;
  }
  while(!istr.eof()){
    vgl_box_2d<double> bbox;
    bbox.read(istr);
        if(istr.eof())
          continue;
    if(bbox.is_empty())
      continue;
    vgl_point_2d<double> pmin = bbox.min_point();
    vgl_point_2d<double> pmax = bbox.max_point();
    std::vector<vsol_point_2d_sptr> verts;
    verts.push_back(new vsol_point_2d(pmin.x(), pmin.y()));
    verts.push_back(new vsol_point_2d(pmax.x(), pmin.y()));
    verts.push_back(new vsol_point_2d(pmax.x(), pmax.y()));
    verts.push_back(new vsol_point_2d(pmin.x(), pmax.y()));
    vsol_polygon_2d_sptr poly = new vsol_polygon_2d(verts);
    my_observer_->add_vsol_polygon_2d(poly, sty);
  }
  istr.close();
  r = r+0.5f;
}
void bwm_tableau_img::load_oriented_boxes_2d_ascii()
{
  // the style toggling is just to
  // allow comparison of two or three oriented boxes
  static float r = 0.0;
  if(r == 1.5f)
    r = 0.0f;
  vgui_style_sptr sty = vgui_style::new_style( r, 1.0f, 0.0f, 3.0, 2.0);

  vgui_dialog load_dlg("Load Oriented Boxes");
  std::string ext, pt_filename;
  load_dlg.file("Obox Filename", ext, pt_filename);
  if (!load_dlg.ask())
    return;
  std::ifstream istr(pt_filename.c_str());
  if(!istr.is_open()){
    std::cout << "Failed to open " << pt_filename << std::endl;
    return;
  }
  while(!istr.eof()){
    vgl_oriented_box_2d<float> obox;
    istr >> obox;
    if(istr.eof())
      continue;
    std::vector<vgl_point_2d<float> > corns = obox.corners();
    if(!corns.size())
      continue;
    std::vector<vsol_point_2d_sptr> verts;
    for(std::vector<vgl_point_2d<float> >::iterator cit = corns.begin();
        cit != corns.end(); ++cit){
      verts.push_back(new vsol_point_2d(cit->x(), cit->y()));
    }
    vsol_polygon_2d_sptr poly = new vsol_polygon_2d(verts);
    my_observer_->add_vsol_polygon_2d(poly, sty);
  }
  istr.close();
  r = r+0.5f;
}
void bwm_tableau_img::help_pop()
{
  bwm_tableau_text* text = new bwm_tableau_text(500, 500);

  text->set_text("C:/lems/lemsvxlsrc/contrib/bwm/doc/HELP_cam.txt");
  vgui_tableau_sptr v = vgui_viewer2D_tableau_new(text);
  vgui_tableau_sptr s = vgui_shell_tableau_new(v);
  vgui_dialog popup("CAMERA TABLEAU HELP");
  popup.inline_tableau(s, 550, 550);
  if (!popup.ask())
    return;
}

void bwm_tableau_img::step_edges_vd()
{
  my_observer_->step_edges_vd();
}

void bwm_tableau_img::lines_vd()
{
  my_observer_->lines_vd();
}

void bwm_tableau_img::recover_edges()
{
  my_observer_->recover_edges();
}

void bwm_tableau_img::recover_lines()
{
  my_observer_->recover_lines();
}

void  bwm_tableau_img::crop_image()
{
  vgui_dialog crop_dlg("Crop Image Path");
  std::string ext = "tiff";
  std::string filename;
  crop_dlg.file("Point Filename", ext, filename);
  if (!crop_dlg.ask())
    return;
  vil_image_resource_sptr chip;
  if (!my_observer_->crop_image(chip))
    return;
  vil_save_image_resource(chip, filename.c_str());
}

bool bwm_tableau_img::handle(const vgui_event& e)
{
  return bgui_picker_tableau::handle(e);
}

void bwm_tableau_img::init_mask()
{
  my_observer_->init_mask();
}

void bwm_tableau_img::add_poly_to_mask()
{
  my_observer_->add_poly_to_mask();
}

#if 0
void bwm_tableau_img::add_dontcare_poly_to_mask()
{
  my_observer_->add_dontcare_poly_to_mask();
}
#endif

void bwm_tableau_img::remove_poly_from_mask()
{
  my_observer_->remove_poly_from_mask();
}

#if 0
void bwm_tableau_img::create_mask()
{
  my_observer_->create_mask();
}
#endif
