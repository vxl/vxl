// This is brl/bseg/boxm2/view/boxm2_cam_ground_trajectory_tableau.h
#ifndef boxm2_cam_ground_trajectory_tableau_h_
#define boxm2_cam_ground_trajectory_tableau_h_
//:
// \file
// \brief A tableau with a single camera that moves on a given trajectory
// \author Vishal Jain
// \date Jan 14, 2014

#include <vgui/vgui_tableau.h>
#include <vgui/vgui_drag_mixin.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_tableau_sptr.h>
#include <vgui/vgui_grid_tableau.h>
#include <boxm2/view/boxm2_trajectory.h>

class boxm2_cam_ground_trajectory_tableau:  public vgui_tableau
{
 public:

    //: Constructor - don't use this, use vgui_viewer3D_tableau_new.
    boxm2_cam_ground_trajectory_tableau();
    virtual ~boxm2_cam_ground_trajectory_tableau() {}

    void setup_gl_matrices();
    //tableau handle - moves camera around
    virtual bool handle( vgui_event const& e );
     //event conditions
    vgui_event_condition c_mouse_rotate_left;
    vgui_event_condition c_mouse_rotate_right;
    vgui_event_condition c_mouse_rotate_up;
    vgui_event_condition c_mouse_rotate_down;
    vgui_event_condition c_mouse_translate_fwd;
    vgui_event_condition c_mouse_translate_bwd;
    vgui_event_condition c_mouse_zoom_in;
    vgui_event_condition c_mouse_zoom_out;
 protected:
     //Current Cam, init cam and current stare point
     vpgl_perspective_camera<double> cam_;
     vpgl_perspective_camera<double> default_cam_;
     vgl_homg_point_3d<double> stare_point_;
     vgl_homg_point_3d<double> default_stare_point_;

     std::list<vgl_point_3d<double> > waypoints_;
     std::list<vgl_point_3d<double> >::iterator waypoint_iter;
     double scale_;

};

//: declare smart pointer
typedef vgui_tableau_sptr_t<boxm2_cam_ground_trajectory_tableau> boxm2_cam_ground_trajectory_tableau_sptr;

//: Create a smart-pointer to a boxm2_cam_ground_trajectory_tableau tableau.
struct boxm2_cam_ground_trajectory_tableau_new : public boxm2_cam_ground_trajectory_tableau_sptr
{
  // Constructor - create an empty boxm2_cam_ground_trajectory_tableau.
  typedef boxm2_cam_ground_trajectory_tableau_sptr base;
  boxm2_cam_ground_trajectory_tableau_new() : base( new boxm2_cam_ground_trajectory_tableau ) { }
};
#endif // boxm2_cam_ground_trajectory_tableau_h_
