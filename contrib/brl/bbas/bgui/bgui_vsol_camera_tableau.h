// This is brl/bbas/bgui/bgui_vsol_camera_tableau.h
#ifndef bgui_vsol_camera_tableau_h_
#define bgui_vsol_camera_tableau_h_
//-----------------------------------------------------------------------------
//:
// \file
// \brief A tableau that knows how to project 3-d vsol objects with a camera
// \author
//   J.L. Mundy
//
// \verbatim
//  Created June 2, 2003
//  Modifications:
//   Peter Vanroose - 8 May 2004 - re-implemented add_vsol_box_3d() with a single polygon
// \endverbatim
//-----------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vgl/algo/vgl_p_matrix.h>
#include <vgui/vgui_style.h>
#include <vgl/vgl_point_2d.h>
#include <vsol/vsol_spatial_object_3d_sptr.h>
#include <vsol/vsol_point_3d_sptr.h>
#include <vsol/vsol_line_3d_sptr.h>
#include <vsol/vsol_polygon_3d_sptr.h>
#include <vsol/vsol_box_3d_sptr.h>
#include <vgui/vgui_tableau_sptr.h>
#include <vgui/vgui_style_sptr.h>
#include <vgui/vgui_image_tableau_sptr.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <bgui/bgui_vsol_camera_tableau_sptr.h>

class bgui_vsol_soview2D_point;
class bgui_vsol_soview2D_line_seg;
class bgui_vsol_soview2D_polygon;

class bgui_vsol_camera_tableau : public vgui_easy2D_tableau
{
 public:
  bgui_vsol_camera_tableau(const char* n="unnamed");

  bgui_vsol_camera_tableau(vgui_image_tableau_sptr const& it,
                           const char* n="unnamed");

  bgui_vsol_camera_tableau(vgui_tableau_sptr const& t,
                           const char* n="unnamed");

  ~bgui_vsol_camera_tableau();

  //:virtual handle method for events
  virtual bool handle(vgui_event const &);

  //:the projection camera
  void set_camera(vgl_p_matrix<double> const& camera){camera_=camera;}
  vgl_p_matrix<double> get_camera(){return camera_;}

  //: display for projected vsol_point_3d
  bgui_vsol_soview2D_point*
    add_vsol_point_3d(vsol_point_3d_sptr const& point);

  //: display for vsol_line_3d
  bgui_vsol_soview2D_line_seg*
    add_vsol_line_3d(vsol_line_3d_sptr const& line);

  //: display for vsol_polygon_3d
  bgui_vsol_soview2D_polygon*
    add_vsol_polygon_3d(vsol_polygon_3d_sptr const& poly);

  //:display for a 3D box
  bgui_vsol_soview2D_polygon*
    add_vsol_box_3d(vsol_box_3d_sptr const& box);

  //: display for general spatial object
  void add_spatial_object_3d(vsol_spatial_object_3d_sptr const& sos);

  //: display methods for sets of objects
  void add_spatial_objects_3d(vcl_vector<vsol_spatial_object_3d_sptr> const& sos);

  void add_points_3d(vcl_vector<vsol_point_3d_sptr> const & lines);

  void add_lines_3d(vcl_vector<vsol_line_3d_sptr> const & lines);

  void add_polygons_3d(vcl_vector<vsol_polygon_3d_sptr> const & lines);

  void add_boxes_3d(vcl_vector<vsol_box_3d_sptr> const & lines);


  //: clear the tableau including the highlight map
  void clear_all();

  //: Methods for getting mapped objects
  void enable_highlight(){highlight_ = true;}
  void disable_highlight(){highlight_ = false;}

  //: Methods for changing the default style of displayable objects

  void set_vsol_point_3d_style(const float r, const float g, const float b,
                               const float point_radius);

  void set_vsol_line_3d_style(const float r, const float g, const float b,
                              const float line_width);

  void set_vsol_polygon_3d_style(const float r, const float g, const float b,
                                 const float line_width);

  void set_vsol_box_3d_style(const float r, const float g, const float b,
                             const float line_width);

 protected:
  //internal methods
  vgl_point_2d<double>  project(vsol_point_3d_sptr const& p3d);
  //members
  bool highlight_;
  void init();
  vcl_map<int, vsol_spatial_object_3d_sptr> obj_map_;
  vcl_map<vcl_string, vgui_style_sptr> style_map_;
  int old_id_;
  vgui_style_sptr highlight_style_;
  vgui_style_sptr old_style_;
  vgl_p_matrix<double> camera_;
};

//this stuff is needed to establish inheritance between tableau  smart pointers
//cloned from xcv_image_tableau
struct bgui_vsol_camera_tableau_new : public bgui_vsol_camera_tableau_sptr
{
  typedef bgui_vsol_camera_tableau_sptr base;

  bgui_vsol_camera_tableau_new(const char* n="unnamed") :
    base(new bgui_vsol_camera_tableau(n)) { }
  bgui_vsol_camera_tableau_new(vgui_image_tableau_sptr const& it,
                               const char* n="unnamed") :
    base(new bgui_vsol_camera_tableau(it,n)) { }

  bgui_vsol_camera_tableau_new(vgui_tableau_sptr const& t, const char* n="unnamed")
    :  base(new bgui_vsol_camera_tableau(t, n)) { }

  operator vgui_easy2D_tableau_sptr () const { vgui_easy2D_tableau_sptr tt; tt.vertical_cast(*this); return tt; }
};

#endif // bgui_vsol_camera_tableau_h_
