#include "bwm_tableau_img.h"
#include "bwm_tableau_text.h"
#include "bwm_command_macros.h"
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>

#include <vgui/vgui_dialog.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_command.h>


void bwm_tableau_img::get_popup(vgui_popup_params const &params, vgui_menu &menu) {
    
  vgui_menu submenu;
  submenu.add("Polygon..",
    new vgui_command_simple<bwm_tableau_img>(this,&bwm_tableau_img::create_polygon),
    vgui_key('p'), 
    vgui_modifier(vgui_SHIFT) );
  submenu.separator();
  submenu.add("PolyLine..",
    new vgui_command_simple<bwm_tableau_img>(this,&bwm_tableau_img::create_polyline),
    vgui_key('l'), 
    vgui_modifier(vgui_SHIFT) );
  submenu.separator();

 
  submenu.add("Box..",
    new vgui_command_simple<bwm_tableau_img>(this,&bwm_tableau_img::create_box),
    vgui_key('b'), 
    vgui_modifier(vgui_SHIFT) );

  submenu.separator();
  submenu.add("Point..",
    new vgui_command_simple<bwm_tableau_img>(this,&bwm_tableau_img::create_point),
    vgui_key('t'), 
    vgui_modifier(vgui_SHIFT) );
  menu.add( "DRAW..", submenu);
  menu.separator();

  menu.add( "Deselect All", 
    new vgui_command_simple<bwm_tableau_img>(this,&bwm_tableau_img::deselect_all),
    vgui_key('-'));
  menu.separator();
  menu.add( "Delete Selected", 
    new vgui_command_simple<bwm_tableau_img>(this,&bwm_tableau_img::clear_poly), 
    vgui_key('d'), vgui_modifier(vgui_SHIFT));
  menu.separator();
  menu.add( "Delete All", 
    new vgui_command_simple<bwm_tableau_img>(this,&bwm_tableau_img::clear_all),
    vgui_key('a'), vgui_modifier(vgui_SHIFT));
  menu.separator();
  menu.add( "Detect Edges", 
    new vgui_command_simple<bwm_tableau_img>(this,&bwm_tableau_img::step_edges_vd),
    vgui_key('e'), vgui_modifier(vgui_SHIFT));

  menu.add( "Detect Lines ", 
    new vgui_command_simple<bwm_tableau_img>(this,&bwm_tableau_img::lines_vd),
    vgui_key('v'), vgui_modifier(vgui_SHIFT));

  menu.separator();

  vgui_menu image_submenu;
  MENU_TAB_ADD_PROCESS("Range Map", "range_map", image_submenu, this);
  MENU_TAB_ADD_PROCESS("Intensity Profile", "intensity_profile", image_submenu, this);
  MENU_TAB_ADD_PROCESS("Histogram Plot", "histogram", image_submenu, this);
  MENU_TAB_ADD_PROCESS("Step Edges VD", "step_edge", image_submenu, this);
  MENU_TAB_ADD_PROCESS("JIMs item", "jim's process", image_submenu, this);
  menu.add("Image Processing...", image_submenu);
  menu.separator();

  menu.add( "HELP..." , 
    new vgui_command_simple<bwm_tableau_img>(this,&bwm_tableau_img::help_pop), 
    vgui_key('h'),vgui_modifier(vgui_SHIFT));

  menu.add( "Show Path" , 
    new vgui_command_simple<bwm_tableau_img>(this,
                                             &bwm_tableau_img::
                                             toggle_show_image_path), 
    vgui_key('i'));

}

void bwm_tableau_img::create_box()
{
  // first lock the bgui_image _tableau
  my_observer_->image_tableau()->lock_linenum(true);
  set_color(1, 0, 0);
  float x1=0, y1=0, x2=0, y2=0;
  pick_box(&x1, &y1, &x2, &y2);
  vsol_box_2d_sptr box2d = new vsol_box_2d();
  box2d->add_point(x1, y1);
  box2d->add_point(x2, y2);
  my_observer_->image_tableau()->lock_linenum(false);

  // add the box to the list
  my_observer_->create_box(box2d);
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

void bwm_tableau_img::toggle_show_image_path(){
  my_observer_->toggle_show_image_path();
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
void bwm_tableau_img::step_edges_vd()
{
  my_observer_->step_edges_vd();
}

void bwm_tableau_img::lines_vd()
{
  my_observer_->lines_vd();
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
