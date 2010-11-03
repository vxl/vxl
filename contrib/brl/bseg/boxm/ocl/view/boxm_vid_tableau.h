// This is brl/bseg/boxm/ocl/view/boxm_vid_tableau.h
#ifndef boxm_vid_tableau_h_
#define boxm_vid_tableau_h_
//:
// \file
// \brief A tableau with a single camera that can be manipulated in the style of a video game
// \author Andrew Miller
// \date July 14, 2010
#include "boxm_vid_tableau_sptr.h"
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_drag_mixin.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vgui/vgui_event_condition.h>

//: Controls
//  Use the mouse to determine the look vector.  If it is a certain distance 
//  from center, it sill rotate your look vector. 
//
//  Keyboard controls:
//  A (and left arrow): pan left
//  D (and right arrow): pan right
//  W (and up arrow): move forward
//  S (and back arrow): move backward

class boxm_vid_tableau: public vgui_tableau, public vgui_drag_mixin
{
 public:

    //: Constructor - don't use this, use vgui_viewer3D_tableau_new.
    boxm_vid_tableau();
    virtual ~boxm_vid_tableau() {}

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
    
    //camera manipulations
    void pan_camera(double dx, double dy);
    void zoom_camera(double dz); 
    bool rotate_camera();

    //Current Cam, init cam and current stare point
    vpgl_perspective_camera<double> cam_;
    vpgl_perspective_camera<double> default_cam_;
    vgl_homg_point_3d<double> stare_point_;

    //keep track of events
    vgui_event event;
    vgui_event last;

    //keep track of mouse dragging
    float beginx;
    float beginy;
    float prevx;
    float prevy;
};

//: Create a smart-pointer to a boxm_vid_tableau tableau.
struct boxm_vid_tableau_new : public boxm_vid_tableau_sptr
{
  // Constructor - create an empty boxm_vid_tableau.
  typedef boxm_vid_tableau_sptr base;
  boxm_vid_tableau_new() : base( new boxm_vid_tableau ) { }
};
#endif // boxm_vid_tableau_h_
