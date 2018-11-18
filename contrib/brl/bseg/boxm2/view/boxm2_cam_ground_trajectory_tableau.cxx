#include "boxm2_cam_ground_trajectory_tableau.h"
//:
// \file
#include <vgui/vgui_gl.h>
#include <vgui/vgui_glu.h>
#include <vgui/internals/trackball.h>
#include <vgui/vgui.h>
#include <vgui/vgui_event.h>
#include <vgl/vgl_distance.h>


boxm2_cam_ground_trajectory_tableau::boxm2_cam_ground_trajectory_tableau() :
      c_mouse_rotate_left(vgui_CURSOR_LEFT),
      c_mouse_rotate_right(vgui_CURSOR_RIGHT),
      c_mouse_rotate_up(vgui_CHAR_w),
      c_mouse_rotate_down(vgui_CHAR_s),
      c_mouse_translate_fwd(vgui_CURSOR_UP),
      c_mouse_translate_bwd(vgui_CURSOR_DOWN),
      c_mouse_zoom_in(vgui_CHAR_a),
      c_mouse_zoom_out(vgui_CHAR_d)
{
  stare_point_ = vgl_homg_point_3d<double>(0,0,0);
  default_stare_point_= vgl_homg_point_3d<double>(0,0,0);
  vgl_point_3d<double> def(2,2,2);
  cam_.set_camera_center(def);
  default_cam_.set_camera_center(def);
}

//: sets up viewport and GL Modes
void boxm2_cam_ground_trajectory_tableau::setup_gl_matrices()
{
  GLint vp[4]; // x,y,w,h
  glGetIntegerv(GL_VIEWPORT, vp);
  glViewport(0, 0, vp[2], vp[3]);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, 1.0, 0.0, 1.0, 0.0, 1.0);
}


//: Handles tableau key and drag events
bool boxm2_cam_ground_trajectory_tableau::handle(vgui_event const &e)
{
  //cam tableau just handles key presses and mouse manipulation
  if (e.type == vgui_KEY_PRESS)
  {
    if (e.key == vgui_key('r')) {
        std::cout<<"resetting to initial camera view"<<std::endl;
        stare_point_ = default_stare_point_;//vgl_homg_point_3d<double>(0,0,0);
        cam_ = default_cam_;
        this->post_redraw();
        return true;
    }
    if (e.key == vgui_CURSOR_LEFT || e.key == vgui_CURSOR_RIGHT || e.key == vgui_CHAR_w || e.key == vgui_CHAR_s)
    {

        double dx = 0.0;
        double dy = 0.0;
        // get mouse deltas
        if(e.key == vgui_CURSOR_LEFT)
            dx = 1.0;
        if(e.key == vgui_CURSOR_RIGHT)
            dx = -1.0;
        if(e.key == vgui_CHAR_w)
            dy = -1.0;
        if(e.key == vgui_CHAR_s)
            dy = 1.0;
        //cam location in spherical coordinates (this should be relative to the stare point
        vgl_rotation_3d<double> R = cam_.get_rotation();
        vnl_matrix<double> rm = R.inverse().as_matrix();
        vnl_vector<double> yaxis = rm*vnl_vector_fixed<double,3>(0.0,1.0,0.0);
        vnl_vector<double> xaxis = rm*vnl_vector_fixed<double,3>(1.0,0.0,0.0);
        //update theta by a function of dy
        double angleScale = .1;
        vnl_quaternion<double> qx(yaxis,-dx*angleScale);
        vnl_quaternion<double> qy(xaxis,-dy*angleScale);
        vgl_rotation_3d<double> dRx( qx ) ;
        vgl_rotation_3d<double> dRy( qy ) ;
        cam_.set_rotation(R*dRy.inverse()*dRx.inverse());
        this->post_redraw();
        return true;
    }
    if(e.key == vgui_CURSOR_UP || e.key == vgui_CURSOR_DOWN)
    {
         if(e.key == vgui_CURSOR_UP && waypoint_iter!=waypoints_.end())
            waypoint_iter++;
        if(e.key == vgui_CURSOR_DOWN && waypoint_iter!=waypoints_.begin())
            waypoint_iter--;

        if(waypoint_iter== waypoints_.end())
            waypoint_iter--;

        cam_.set_camera_center(*waypoint_iter);
        this->post_redraw();
        return true;
    }
    if(e.key == vgui_CHAR_a || e.key == vgui_CHAR_d)
    {
        double scale = 1.1;
        vpgl_calibration_matrix<double> K=  cam_.get_calibration();
        double f = K.focal_length();
        double defaultf =default_cam_.get_calibration().focal_length();
        if ( e.key == vgui_CHAR_a && f >= defaultf/2 )
            K.set_focal_length(f/scale);
        else if (e.key == vgui_CHAR_d && f <= 2*defaultf )
            K.set_focal_length(f*scale);
        cam_.set_calibration(K);
        this->post_redraw();
        return true;
    }
    this->post_redraw();
  }
  else if (e.type == vgui_RESHAPE)
  {
      this->post_redraw();
  }

  if (vgui_tableau::handle(e))
    return true;

  return false;
}
