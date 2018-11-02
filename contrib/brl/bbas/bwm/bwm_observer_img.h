#ifndef bwm_observer_img_h_
#define bwm_observer_img_h_
//:
// \file

#include <vector>
#include <iostream>
#include <map>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <bwm/algo/bwm_soview2D_vertex.h>

#include <vgui/vgui_viewer2D_tableau.h>

#include <vil/vil_image_view_base.h>
#include <bgui/bgui_image_tableau.h>
#include <bgui/bgui_vsol2D_tableau.h>
#include <bgui/bgui_vsol_soview2D.h>

#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_digital_curve_2d_sptr.h>
#include <vsol/vsol_line_2d_sptr.h>

#include <bvgl/bvgl_change_obj_sptr.h>
#include <bvgl/bvgl_changes_sptr.h>

#define VERTEX_TYPE "bwm_soview2D_vertex"
#define POLYLINE_TYPE "bgui_vsol_soview2D_polyline"
#define POLYGON_TYPE "bgui_vsol_soview2D_polygon"
#define POINT_TYPE "bgui_vsol_soview2D_point"

class bwm_observer_img : public bgui_vsol2D_tableau
{
 public:

  enum BWM_2D_DRAW_MODE {MODE_2D_POLY, MODE_2D_EDGE, MODE_2D_VERTEX, MODE_2D_UNDEF};

  typedef bgui_vsol2D_tableau base;

  bwm_observer_img(bgui_image_tableau_sptr const& img, std::string name,
                   std::string image_path, bool display_image_path);

  bwm_observer_img(bgui_image_tableau_sptr const& img)
  : bgui_vsol2D_tableau(img), lock_vgui_status_(false), vgui_status_on_(false), draw_mode_(MODE_2D_POLY),
    img_tab_(img), viewer_(nullptr), mask_(nullptr), change_type_("change"), show_image_path_(false),
    start_x_(0), start_y_(0), moving_p_(nullptr), moving_v_(nullptr), moving_vertex_(false), moving_polygon_(false),
    in_jog_mode_(false), row_(0), col_(0) {}

  virtual ~bwm_observer_img();

  bgui_image_tableau_sptr image_tableau() { return img_tab_; }

  unsigned ni(){if(img_tab_)return img_tab_->width(); return 0;}
  unsigned nj(){if(img_tab_)return img_tab_->height(); return 0;}

  void set_viewer(vgui_viewer2D_tableau_sptr viewer) { viewer_ = viewer; }

  virtual void set_tab_name(std::string name) { tab_name_.assign(name); }

  std::string tab_name() const { return tab_name_; }

  bool handle(const vgui_event &);

  virtual std::string type_name() const { return "bwm_observer_img"; }

  unsigned create_box(vsol_box_2d_sptr);

  virtual unsigned create_polygon(vsol_polygon_2d_sptr);

  unsigned create_polyline(vsol_polyline_2d_sptr);

  unsigned create_point(vsol_point_2d_sptr);

  void copy();

  void paste(float x, float y);

  void clear_objects();
  //: various types of selected objects
  bool get_selected_box(bgui_vsol_soview2D_polygon* &box);
  bool get_selected_poly(bgui_vsol_soview2D_polygon* &poly);

  //: clears the edges inside a selected box
  void clear_box();

  void delete_selected();

  virtual void delete_all();

  void hist_plot();
  void hist_plot_in_poly();

  void intensity_profile(float start_col, float start_row,
                         float end_col, float end_row);

  void range_map();

  void step_edges_vd();

  void lines_vd();

  void recover_edges();

  void recover_lines();

  bool crop_image(vil_image_resource_sptr& chip);

  //:moves to a specified position
  void move_to_point(float x, float y);

  //: zoom out the image to fit the grid cell
  void zoom_to_fit();

  //: for menu operation, calls move_to_point
  void scroll_to_point();

  void toggle_show_image_path();

  void set_grid_location(unsigned col, unsigned row){col_=col; row_ = row;}
  unsigned row() const {return row_;}
  unsigned col() const {return col_;}

  std::vector<vsol_digital_curve_2d_sptr> edges(unsigned id)
    {return edge_list[id];}

  void display_reg_seg(std::vector<vsol_digital_curve_2d_sptr> const& search_edges,
                       std::vector<vsol_digital_curve_2d_sptr> const& model_edges);
  void clear_reg_segmentation();

  std::string image_path() const {return img_tab_->file_name();}

  virtual void init_mask();
  //: sets the change type for ground truth areas
  void set_change_type();
  virtual void add_poly_to_mask();
  //void add_dontcare_poly_to_mask();
  virtual void remove_poly_from_mask();
  //void create_mask();
  vil_image_view_base_sptr mask();
  virtual bool save_changes_binary();
  virtual bool load_changes_binary();

  //: lock/unlock the status display
  void lock_vgui_status(bool lock){lock_vgui_status_ = lock;}
  //: is status locked?
  bool vgui_status_locked() const {return lock_vgui_status_;}

  //is vgui status being displayed by *this observer?
  bool vgui_status_on() const {return vgui_status_on_;}

  void set_vgui_status_on(bool status_on){vgui_status_on_ = status_on;}

  //: returns a list of all the existing spatial objects
  std::vector<vsol_spatial_object_2d_sptr> get_spatial_objects_2d();

  //: set the draw mode to either polygon or vertex
  void set_draw_mode(BWM_2D_DRAW_MODE mode);

 protected:
  //:flags to indicate vgui status displays by observers

  //: lock status display when drawing or other motion tracking
  bool lock_vgui_status_;

  //: observer status is being displayed,
  //  so image pixel values should be blocked
  bool vgui_status_on_;

  enum BWM_2D_DRAW_MODE draw_mode_;

  bwm_observer_img() {}

  bgui_image_tableau_sptr img_tab_;

  vgui_viewer2D_tableau_sptr viewer_;

  vil_image_view_base_sptr mask_;

  //: change areas, mapped to the soviewID for easy deletion
  std::map<unsigned int, bvgl_change_obj_sptr> change_polys_;

  //: ground truth areas
  bvgl_changes_sptr ground_truth_;

  //: the current ground truth change type
  std::string change_type_;

  bool show_image_path_;

  // polygons are mapped soview ID
  std::map<unsigned, bgui_vsol_soview2D*> obj_list;

  // vector of vertices are mapped soview ID for each polygon
  std::map<unsigned, std::vector<bwm_soview2D_vertex* > > vert_list;

  // maps for box segmentations
  std::map<unsigned, std::vector<vsol_digital_curve_2d_sptr > > edge_list;
  std::map<unsigned, std::vector<vsol_line_2d_sptr > > line_list;
  std::map<unsigned, std::vector<bgui_vsol_soview2D*> > seg_views;
  // storage for registration edge views
  std::vector<bgui_vsol_soview2D*> reg_seg_views_;

  float start_x_, start_y_;
  bgui_vsol_soview2D* moving_p_;
  bwm_soview2D_vertex* moving_v_;
  bool moving_vertex_, moving_polygon_;
  bool in_jog_mode_;

  //: polygon selected to copy
  vgui_soview2D* copy_obj_;

  virtual void delete_polygon(vgui_soview* obj);
  void delete_vertex(vgui_soview* vertex);

  //: returns the type of selected object iff there is one object. If warn is true, then issue a warning if the object is not found.
  vgui_soview2D* get_selected_object(std::string type, bool warn = false);

  //: returns a list of selected object from the given type
  std::vector<vgui_soview2D*> get_selected_objects(std::string type);

  std::string tab_name_;

  std::vector<std::string> change_choices_;

  unsigned row_; //location of observer in grid
  unsigned col_;
};

#endif
