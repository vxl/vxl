#ifndef bwm_observer_img_h_
#define bwm_observer_img_h_
//:
// \file

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_map.h>

#include <bwm/algo/bwm_soview2D_vertex.h>

#include <vgui/vgui_viewer2D_tableau.h>

#include <bgui/bgui_image_tableau.h>
#include <bgui/bgui_vsol2D_tableau.h>
#include <bgui/bgui_vsol_soview2D.h>

#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_digital_curve_2d_sptr.h>
#include <vsol/vsol_line_2d_sptr.h>

#define VERTEX_TYPE "bwm_soview2D_vertex"
#define POLYLINE_TYPE "bgui_vsol_soview2D_polyline"
#define POLYGON_TYPE "bgui_vsol_soview2D_polygon"
#define POINT_TYPE "bgui_vsol_soview2D_point"

class bwm_observer_img : public bgui_vsol2D_tableau
{
 public:

  typedef bgui_vsol2D_tableau base;

  bwm_observer_img(bgui_image_tableau_sptr const& img, vcl_string name, 
    vcl_string image_path, bool display_image_path);

  bwm_observer_img(bgui_image_tableau_sptr const& img)
    : bgui_vsol2D_tableau(img), img_tab_(img), viewer_(0),
    show_image_path_(false), start_x_(0), start_y_(0), moving_p_(0),
    moving_v_(0), moving_vertex_(false), moving_polygon_(false),
    in_jog_mode_(false), row_(0), col_(0) {  }

  virtual ~bwm_observer_img();

  bgui_image_tableau_sptr image_tableau() { return img_tab_; }

  void set_viewer(vgui_viewer2D_tableau_sptr viewer) { viewer_ = viewer; }

  virtual void set_tab_name(vcl_string name) { tab_name_.assign(name); }

  vcl_string tab_name() { return tab_name_; }

  bool handle(const vgui_event &);

  virtual vcl_string type_name() const { return "bwm_observer_img"; }

  void create_box(vsol_box_2d_sptr);

  void create_polygon(vsol_polygon_2d_sptr);

  void create_polyline(vsol_polyline_2d_sptr);

  void create_point(vsol_point_2d_sptr);

  bool get_selected_box(bgui_vsol_soview2D_polygon* &box);

  //: clears the edges inside a selected box
  void clear_box();

  void delete_selected();

  void delete_all();

  void save();

  void hist_plot();

  void intensity_profile(float start_col, float start_row,
                         float end_col, float end_row);

  void range_map();

  void step_edges_vd();

  void lines_vd();

  void recover_edges();

  void recover_lines();

  void jim_obs_process() { vcl_cout << "I am Jim Green's Process!!!!" << vcl_endl; }

  //:moves to a specified position
  void move_to_point(float x, float y);

  //: zoom out the image to fit the grid cell
  void zoom_to_fit();

  //: for menu operation, calls move_to_point
  void scroll_to_point();

  void toggle_show_image_path();

  void set_grid_location(unsigned col, unsigned row){col_=col; row_ = row;}
  unsigned row(){return row_;}
  unsigned col(){return col_;}

  vcl_vector<vsol_digital_curve_2d_sptr> edges(unsigned id)
    {return edge_list[id];}

  void display_reg_seg(vcl_vector<vsol_digital_curve_2d_sptr> const& search_edges,
                    vcl_vector<vsol_digital_curve_2d_sptr> const& model_edges); 
  void clear_reg_segmentation();

  vcl_string image_path(){return img_tab_->file_name();}
 protected:

  bwm_observer_img();

  bgui_image_tableau_sptr img_tab_;

  vgui_viewer2D_tableau_sptr viewer_;

  bool show_image_path_;

  // polygons are mapped soview ID
  vcl_map<unsigned, bgui_vsol_soview2D*> obj_list;

  // vector of vertices are mapped soview ID for each polygon
  vcl_map<unsigned, vcl_vector<bwm_soview2D_vertex* > > vert_list;

  // maps for box segmentations
  vcl_map<unsigned, vcl_vector<vsol_digital_curve_2d_sptr > > edge_list;
  vcl_map<unsigned, vcl_vector<vsol_line_2d_sptr > > line_list;
  vcl_map<unsigned, vcl_vector<bgui_vsol_soview2D*> > seg_views;
  // storage for registration edge views
  vcl_vector<bgui_vsol_soview2D*> reg_seg_views_;

  float start_x_, start_y_;
  bgui_vsol_soview2D* moving_p_;
  bwm_soview2D_vertex* moving_v_;
  bool moving_vertex_, moving_polygon_;
  bool in_jog_mode_;
  void delete_polygon(vgui_soview* obj);
  void delete_vertex(vgui_soview* vertex);

  //: returns the type of selected object iff there is one object
  vgui_soview2D* get_selected_object(vcl_string type);

  //: returns a list of selected object from the given type
  vcl_vector<vgui_soview2D*> get_selected_objects(vcl_string type);
  vil_image_resource_sptr load_image(vcl_string& filename,
                                                    vgui_range_map_params_sptr& rmps);
  vcl_string tab_name_;

  unsigned row_; //location of observer in grid
  unsigned col_;
};

#endif
