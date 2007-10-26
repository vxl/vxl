#include "bwm_tableau_img.h"
#include "bwm_tableau_text.h"

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>

#include <vgui/vgui_dialog.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_command.h>


void bwm_tableau_img::get_popup(vgui_popup_params const &params, vgui_menu &menu) {
    
  vgui_menu submenu;

  vgui_menu poly_submenu;
  poly_submenu.add("Polygon..",
    new vgui_command_simple<bwm_tableau_img>(this,&bwm_tableau_img::create_polygon),
    vgui_key('p'), 
    vgui_modifier(vgui_SHIFT) );
  poly_submenu.add("PolyLine..",
    new vgui_command_simple<bwm_tableau_img>(this,&bwm_tableau_img::create_polyline),
    vgui_key('l'), 
    vgui_modifier(vgui_SHIFT) );
  poly_submenu.add("Point..",
    new vgui_command_simple<bwm_tableau_img>(this,&bwm_tableau_img::create_point),
    vgui_key('l'), 
    vgui_modifier(vgui_SHIFT) );
  submenu.add( "DRAW..", poly_submenu);
  
  submenu.add( "Deselect All", 
    new vgui_command_simple<bwm_tableau_img>(this,&bwm_tableau_img::deselect_all),
    vgui_key('-'));
  submenu.add( "Delete Object", 
    new vgui_command_simple<bwm_tableau_img>(this,&bwm_tableau_img::clear_poly), 
    vgui_key('d'), vgui_modifier(vgui_SHIFT));
  submenu.add( "Delete All", 
    new vgui_command_simple<bwm_tableau_img>(this,&bwm_tableau_img::clear_all),
    vgui_key('a'), vgui_modifier(vgui_SHIFT));

  vgui_menu image_submenu;
  image_submenu.add("Histogram Plot", new vgui_command_simple<bwm_tableau_img>(this,&bwm_tableau_img::hist_plot));
  image_submenu.add("Intensity Profile", new vgui_command_simple<bwm_tableau_img>(this,&bwm_tableau_img::intensity_profile));
  image_submenu.add("Range Map", new vgui_command_simple<bwm_tableau_img>(this,&bwm_tableau_img::range_map));
  submenu.add("Image...", image_submenu);

  submenu.add( "HELP..." , 
    new vgui_command_simple<bwm_tableau_img>(this,&bwm_tableau_img::help_pop), 
    vgui_key('h'),vgui_modifier(vgui_SHIFT));

  //add this submenu to the popup menu
  //menu.separator();
  menu.add("Modeling Tools", submenu);
}

void bwm_tableau_img::create_polygon()
{
  // first lock the bgui_image _tableau
  my_observer_->image_tableau()->lock_linenum(true);
  vsol_polygon_2d_sptr poly2d;
  set_color(1, 0, 0);
  pick_polygon(poly2d);

  my_observer_->image_tableau()->lock_linenum(false);

  // add the polygon to the list
  my_observer_->create_polygon(poly2d);
}

void bwm_tableau_img::create_polyline()
{
  // first lock the bgui_image _tableau
  my_observer_->image_tableau()->lock_linenum(true);
  vsol_polyline_2d_sptr poly2d;
  set_color(1, 0, 0);
  this->pick_polyline(poly2d);

  my_observer_->image_tableau()->lock_linenum(false);

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

void bwm_tableau_img::deselect_all()
{
  my_observer_->deselect_all();
}

void bwm_tableau_img::clear_poly()
{
  my_observer_->delete_selected();
}

void bwm_tableau_img::clear_all()
{
  my_observer_->delete_all();
}

void bwm_tableau_img::intensity_profile()
{
  float x1, y1, x2, y2;

  pick_line(&x1, &y1, &x2, &y2);
  vcl_cout << x1 << "," << y1 << "-->" << x2 << "," << y2 << vcl_endl;
  my_observer_->intensity_profile(x1, y1, x2, y2);
}

void bwm_tableau_img::range_map()
{
  my_observer_->range_map();
}

void bwm_tableau_img::save()
{
  my_observer_->save();
}

void bwm_tableau_img::help_pop()
{
  bwm_tableau_text* text = new bwm_tableau_text(500, 500);
  
  text->set_text("C:\\lems\\lemsvxlsrc\\lemsvxlsrc\\contrib\\bwm\\doc\\doc\\HELP_cam.txt");
  vgui_tableau_sptr v = vgui_viewer2D_tableau_new(text);
  vgui_tableau_sptr s = vgui_shell_tableau_new(v);
  vgui_dialog popup("CAMERA TABLEAU HELP");
  popup.inline_tableau(s, 550, 550);
  if (!popup.ask())
    return;
}

bool bwm_tableau_img::handle(const vgui_event& e)
{
  //vcl_cout << "Key:" << e.key << " modif: " << e.modifier << vcl_endl;
 /* if (e.key == 'p' && e.modifier == vgui_SHIFT) {
    create_polygon_mesh();
    return true;
  } else if (e.key == 't' && e.modifier == vgui_SHIFT) {
    this->triangulate_mesh();
    return true;
  } else if ( e.key == 'm' && e.modifier == vgui_SHIFT) {
    this->move_obj_by_vertex();
    return true;
  } else if ( e.key == 'e' && e.modifier == vgui_SHIFT) {
    this->extrude_face();
    return true;
  } else if ( e.key == 's' && e.modifier == vgui_SHIFT) {
    this->save();
    return true;
  } else if ( e.key == '-' && e.modifier == vgui_SHIFT) {
    this->deselect_all();
    return true;
  } else if ( e.key == 'd' && e.modifier == vgui_SHIFT) {
    this->clear_object();
    return true;
  } else if ( e.key == 'a' && e.modifier == vgui_SHIFT) {
    this->clear_all();
    return true;
  } else if ( e.key == 'h' && e.modifier == vgui_SHIFT) {
    this->help_pop();
    return true;
  }*/
  return bgui_picker_tableau::handle(e);
}
