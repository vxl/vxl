// This is brl/bseg/bstm/view/bstm_cam_tableau.h
#ifndef bstm_cam_tableau_h_
#define bstm_cam_tableau_h_
//:
// \file
// \brief A tableau with a single camera that changes viewpoints based on mouse and keyboard manipulations.
// \author Ali Osman Ulusoy
// \date November 2, 2012

#include <vgui/vgui_tableau.h>
#include <vgui/vgui_drag_mixin.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_tableau_sptr.h>
#include <vgui/vgui_grid_tableau.h>

#include <boxm2/view/boxm2_trajectory.h>
class bstm_cam_tableau:  public vgui_tableau, public vgui_drag_mixin
{
 public:

    //: Constructor - don't use this, use vgui_viewer3D_tableau_new.
    bstm_cam_tableau();
    virtual ~bstm_cam_tableau() {}

    //camera
    void set_camera(vpgl_perspective_camera<double> cam) {cam_ = cam; }

    void setup_gl_matrices();

    //tableau handle - moves camera around
    virtual bool handle( vgui_event const& e );
    bool mouse_up(int x, int y, vgui_button button, vgui_modifier modifier);
    bool mouse_drag(int x, int y, vgui_button button, vgui_modifier modifier);
    bool mouse_down(int x, int y, vgui_button button, vgui_modifier modifier);

    //event conditions
    vgui_event_condition c_mouse_rotate;
    vgui_event_condition c_mouse_translate;
    vgui_event_condition c_mouse_zoom;

 protected:

    //Current Cam, init cam and current stare point
    vpgl_perspective_camera<double> cam_;
    vpgl_perspective_camera<double> default_cam_;
    vgl_homg_point_3d<double> stare_point_;
    vgl_homg_point_3d<double> default_stare_point_;

    double scale_;

    //: trajectory for idle rendering
    bool render_trajectory_;
    boxm2_trajectory* trajectory_;
    boxm2_trajectory::iterator cam_iter_;

    //keep track of events
    vgui_event event;
    vgui_event last;

    //keep track of mouse dragging
    float beginx;
    float beginy;
    float prevx;
    float prevy;
};

//: declare smart pointer
typedef vgui_tableau_sptr_t<bstm_cam_tableau> bstm_cam_tableau_sptr;

//: Create a smart-pointer to a bstm_cam_tableau tableau.
struct bstm_cam_tableau_new : public bstm_cam_tableau_sptr
{
  // Constructor - create an empty bstm_cam_tableau.
  typedef bstm_cam_tableau_sptr base;
  bstm_cam_tableau_new() : base( new bstm_cam_tableau ) { }
};
#endif // bstm_cam_tableau_h_
