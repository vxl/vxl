#ifndef vplayer_geometry_h_
#define vplayer_geometry_h_

//--------------------------------------------------------------------------------
// .NAME	vplayer_geometry
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
  static void create_point(const void*);
  static void create_line(const void*);
  static void create_infinite_line(const void *);
  static void create_circle(const void*);
  static void create_polygon(const void*);
  static void create_linestrip(const void *);
  static void create_box(const void *);
  static void change_sel_color(const void*);
  static void change_sel_radius(const void*);
  static void change_sel_width(const void*);
  static void delete_sel_objs(const void*);
  static void delete_all(const void *);
  static void delete_points(const void *);
  static void delete_lines(const void *);
  static void delete_inf_lines(const void *);
  static void delete_circles(const void *);
  static void delete_linestrips(const void *);

  static void change_default_color(const void*);
  static void change_default_radius(const void*);
  static void change_default_width(const void*);
  static void save_points(const void *);
  static void save_lines(const void *);
  static void save_linestrips(const void *);
  static void save_circles(const void *);
  static void save_polygons(const void *);
  static void save_infinite_lines(const void *);
  static void load_points(const void *);
  static void load_lines(const void *);
  static void load_linestrips(const void *);
  static void load_circles(const void *);
  static void load_polygons(const void *);
  static void load_infinite_lines(const void *);

  static vgui_menu create_geometry_menu();
};

#endif // vplayer_file_
