// This is oxl/xcv/xcv_geometry.h
#ifndef xcv_geometry_h_
#define xcv_geometry_h_
//:
// \file
// \author   K.Y.McGaul
// \brief  Interface to geometry functions.
//
//  Creates a menu for, and provides an interface to geometric operations
//  contained in vxl.
//
// \verbatim
//  Modifications:
//   K.Y.McGaul     27-JAN-2000    Initial version.
//   Marko Bacic    21-AUG-2000    Added menu for deleting specific type of object
//   Marko Bacic    01-SEP-2000    Added save_...
//   Marko Bacic    02-SEP-2000    Added load_...
// \endverbatim

class vgui_menu;

class xcv_geometry
{
 public:
  //: Draw a point onto the currently selected tableau.
  static void create_point();
  //: Rubberband a line on the currently selected tableau.
  static void create_line();
  //: Rubberband a circle on the currently selected tableau.
  static void create_circle();
  //: Rubberband a polygon on the currently selected tableau.
  static void create_polygon();
  //: Rubberband a linestrip on the currently selected tableau.
  static void create_linestrip();
  //: Rubberband infinite line on the currently selected tableau.
  static void create_infinite_line();
  //:
  static void create_box();
  //: Change the color of all the selected geometric objects.
  static void change_sel_color();
  //: Change the radius of all the selected points.
  static void change_sel_radius();
  //: Change the line widths of all the selected geometric objects
  static void change_sel_width();
  //: Delete all the selected geometric objects.
  static void delete_sel_objs();
  //: Delete all geometric objects.
  static void delete_all();
  //: Delete all points.
  static void delete_points();
  //: Delete all lines.
  static void delete_lines();
  //: Delete all infinite lines.
  static void delete_inf_lines();
  //: Delete all circles.
  static void delete_circles();
  //: Delete all linestrips.
  static void delete_linestrips();

  //: Change the default color for geometric objects.
  static void change_default_color();
  //: Change the default radius for points.
  static void change_default_radius();
  //: Change the default line width for geometric objects.
  static void change_default_width();
  //: Save all points into a file.
  static void save_points();
  //: Save all lines into a file.
  static void save_lines();
  //: Save all linestrips into a file.
  static void save_linestrips();
  //: Save all circles into a file.
  static void save_circles();
  //: Save all geometric objects into a file.
  static void save_geometry();
  //: Load geometric objects from a file.
  static void load_geometry();

  //: Intersect two polygons to get a set of non-intersecting polygons.
  static void polygon_intersect();

  //: Move geometric objects apart.
  static vgui_menu create_geometry_menu();

 private:
  //: Write coordinates into a file.
  static void save(const char *,const char *);
  //: Load coordinates from a file
  static void load(const char *,const char *);
};

#endif // xcv_geometry_h_
