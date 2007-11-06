#ifndef bwm_observer_vgui_h
#define bwm_observer_vgui_h

#include "bwm_observer.h"
#include "bwm_observer_img.h"
#include "bwm_observable_sptr.h"
#include "algo/bwm_soview2D_cross.h"
#include "algo/bwm_soview2D_vertex.h"

#include <vcl_vector.h>
#include <vcl_utility.h>

#include <vgui/vgui_easy2d_tableau.h>
#include <bgui/bgui_image_tableau.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_plane_3d.h>
#include <vgl/vgl_homg_point_2d.h>

#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_3d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_3d_sptr.h>
#include <vsol/vsol_line_2d_sptr.h>

#include <vpgl/vpgl_camera.h>

class bwm_observer_vgui : public bwm_observer_img, public bwm_observer//, public vgui_easy2D_tableau
{
public:

  typedef bwm_observer_img base;

  bwm_observer_vgui(bgui_image_tableau_sptr const& img)
    : bwm_observer_img(img) {corr_.second = 0;}

  virtual ~bwm_observer_vgui() {}

  bool handle(const vgui_event &);

  virtual vcl_string type_name() const { return "bwm_observer_vgui"; }

  void handle_update(vgui_message const& msg, bwm_observable_sptr observable);

  void update_all();

  void translate(vgl_vector_3d<double> T, bwm_observable_sptr object=0);

  void delete_object();

  void delete_all();

  void connect_inner_face(vsol_polygon_2d_sptr poly);

  void create_interior();

  bwm_observable_sptr selected_face(unsigned& face_id);

  void label_roof();

  void label_wall();

  //void hist_plot();

  void intensity_profile(float start_col, float start_row,
                         float end_col, float end_row);

  //void range_map();

  virtual void proj_poly(vsol_polygon_3d_sptr poly3d, 
                         vsol_polygon_2d_sptr& poly2d) = 0;

  virtual void backproj_poly(vsol_polygon_2d_sptr poly2d, 
                             vsol_polygon_3d_sptr& poly3d) = 0;

  void update_corr_pt(float x, float y);

  void remove_corr_pt();

  void add_cross(float x, float y, float r);

protected:

  bwm_observer_vgui() { corr_.second = 0; }

  //: the current correspondence point
  vcl_pair<vgl_point_2d<double>, bwm_soview2D_cross * > corr_;

  //: objects are kept as a triple (bwm_observable *, face_id, bgui_vsol_soview2D_polygon*)
  vcl_map<bwm_observable_sptr, vcl_map<unsigned, bgui_vsol_soview2D_polygon* > > objects_;

  //: vertices are kept as a triple (bwm_observable *, face_id, vector<bwm_soview2D_vertex*> )
  vcl_map<bwm_observable_sptr, vcl_map<unsigned, vcl_vector<bwm_soview2D_vertex* > > > object_verts_;

  bwm_observable_sptr find_object(unsigned soview2d_id, unsigned &face_id);

  unsigned get_selected_3d_vertex_index(unsigned poly_id);

  vsol_point_3d_sptr selected_vertex();

  void print_selected_vertex();

};

#endif

