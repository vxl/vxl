#ifndef vplayer_geometry_h_
#define vplayer_geometry_h_

//--------------------------------------------------------------------------------
// .NAME    vplayer_geometry
// .SECTION Description:
//
// .SECTION Author
//   Marko Bacic, Oxford RRG
// .SECTION Modifications:
//   Marko Bacic    05-SEP-2000    Initial version.
//--------------------------------------------------------------------------------

class vgui_menu;
class vplayer_geometry {

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
  static void load_points();
  static void load_lines();
  static void load_linestrips();
  static void load_circles();
  static void load_polygons();
  static void load_infinite_lines();

  static vgui_menu create_geometry_menu();
};

#endif // vplayer_file_
