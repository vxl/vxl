#ifndef xcv_geometry_h_
#define xcv_geometry_h_

//--------------------------------------------------------------------------------
// .NAME    xcv_geometry
// .INCLUDE xcv/xcv_geometry.h
// .FILE    xcv_geometry.cxx
// .SECTION Description:
//   Creates a menu for, and provides an interface to geometric operations
//   contained in vxl.
// .SECTION Author
//   K.Y.McGaul
// .SECTION Modifications:
//   K.Y.McGaul     27-JAN-2000    Initial version.
//   Marko Bacic    21-AUG-2000    Added menu for deleting specific type of object
//   Marko Bacic    01-SEP-2000    Added save_...
//   Marko Bacic    02-SEP-2000    Added load_...
//--------------------------------------------------------------------------------

class vgui_menu;

class xcv_geometry
{
  static void save(const char *,const char *);
  static void load(const char *,const char *);
public:
  static void create_point();
  static void create_line();
  static void create_infinite_line();
  static void create_circle();
  static void create_polygon();
  static void create_linestrip();
  static void create_box();
  static void change_sel_color();
  static void change_sel_radius();
  static void change_sel_width();
  static void delete_sel_objs();
  static void delete_all();
  static void delete_points();
  static void delete_lines();
  static void delete_inf_lines();
  static void delete_circles();
  static void delete_linestrips();

  static void change_default_color();
  static void change_default_radius();
  static void change_default_width();
  static void save_points();
  static void save_lines();
  static void save_linestrips();
  static void save_circles();
  static void save_polygons();
  static void save_infinite_lines();
  static void save_geometry();
  static void load_points();
  static void load_lines();
  static void load_linestrips();
  static void load_circles();
  static void load_polygons();
  static void load_infinite_lines();
  static void load_geometry();

  static void polygon_intersect();

  static vgui_menu create_geometry_menu();

private:

};

#endif // xcv_geometry_
