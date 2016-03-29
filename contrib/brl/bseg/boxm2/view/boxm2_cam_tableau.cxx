#include "boxm2_cam_tableau.h"
//:
// \file
#include <vgui/vgui_gl.h>
#include <vgui/vgui_glu.h>
#include <vgui/internals/trackball.h>
#include <vgui/vgui.h>
#include <vgui/vgui_event.h>
#include <vgl/vgl_distance.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/algo/vgl_h_matrix_3d.h>


boxm2_cam_tableau::boxm2_cam_tableau() :
      c_mouse_rotate(vgui_LEFT),
      c_mouse_translate(vgui_LEFT, vgui_CTRL),
      c_mouse_zoom(vgui_LEFT, vgui_SHIFT)
{
  stare_point_ = vgl_homg_point_3d<double>(0,0,0);
  default_stare_point_= vgl_homg_point_3d<double>(0,0,0);
  vgl_point_3d<double> def(2,2,2);
  cam_.set_camera_center(def);
  default_cam_.set_camera_center(def);
  default_cam_.look_at(default_stare_point_);
}

//: sets up viewport and GL Modes
void boxm2_cam_tableau::setup_gl_matrices()
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
bool boxm2_cam_tableau::handle(vgui_event const &e)
{
  //cam tableau just handles key presses and mouse manipulation
  if (e.type == vgui_KEY_PRESS)
  {
    if (e.key == vgui_key('r')) {
        std::cout<<"resetting to initial camera view"<<std::endl;
        stare_point_ = default_stare_point_;//vgl_homg_point_3d<double>(0,0,0);
        cam_.set_camera_center(default_cam_.get_camera_center());
        cam_.set_rotation(default_cam_.get_rotation());
    }
    else if (e.key == vgui_key('x')) {
        std::cout<<"looking down X axis at the origin"<<std::endl;
        stare_point_ = default_stare_point_;//vgl_homg_point_3d<double>(0,0,0);
        cam_.set_camera_center(vgl_point_3d<double>(stare_point_.x(),default_cam_.camera_center().y(),stare_point_.z()));
        cam_.look_at(stare_point_);
    }
    else if (e.key == vgui_key('y')) {
        std::cout<<"looking down Y axis at the origin"<<std::endl;
        stare_point_ = default_stare_point_;//vgl_homg_point_3d<double>(0,0,0);
        cam_.set_camera_center(vgl_point_3d<double>(default_cam_.camera_center().x(),stare_point_.y(),stare_point_.z()));
        cam_.look_at(stare_point_);
    }
    else if (e.key == vgui_key('z')) {
        std::cout<<"looking down Z axis at the origin"<<std::endl;
        stare_point_ = default_stare_point_;//vgl_homg_point_3d<double>(0,0,0);
        cam_.set_camera_center(vgl_point_3d<double>(stare_point_.x(),stare_point_.y(),default_cam_.camera_center().z()));
        cam_.look_at(stare_point_);
    }
    this->post_redraw();
  }
  else if (e.type == vgui_RESHAPE)
  {
      this->setup_gl_matrices();
      this->post_redraw();
  }
  if (e.type == vgui_KEY_PRESS && e.key == vgui_key('t')) {
    std::cout<<"rendering trajectory..."<<std::endl;
    render_trajectory_ = true;
    this->post_idle_request();
  }

  //Handles Idle events - should render trajectory
  else if (e.type == vgui_IDLE)
  {
    if(render_trajectory_) {
      vpgl_camera_double_sptr& camSptr = *cam_iter_;
      vpgl_perspective_camera<double>* camPtr = (vpgl_perspective_camera<double>*) camSptr.ptr();
      cam_ = *camPtr;

      //increment cam iter
      ++cam_iter_;
      if(cam_iter_ == trajectory_->end())
        cam_iter_ = trajectory_->begin();

      //rerender
      this->post_redraw();
      return true;
    }
    else {
      return false;
    }
  }
  //see if drag should handle it
  event = e;
  if (vgui_drag_mixin::handle(e))
  {
      render_trajectory_ = false;
      return true;
  }
  if (vgui_tableau::handle(e))
    return true;

  return false;
}


//: called on all mouse down events on tableau
bool boxm2_cam_tableau::mouse_down(int x, int y, vgui_button /*button*/, vgui_modifier /*modifier*/)
{
  if (c_mouse_rotate(event) || c_mouse_translate(event) || c_mouse_zoom(event)) {
    beginx = x;
    beginy = y;
    last = event;
    return true;
  }
  return false;
}

//: called on mouse movement while mousedown is true
bool boxm2_cam_tableau::mouse_drag(int x, int y, vgui_button button, vgui_modifier modifier)
{
  //std::cout<<"Cam @ "<<cam_.get_camera_center()<<std::endl;
  // SPINNING
  if (c_mouse_rotate(button, modifier)) {

    //get viewport height and width
    GLdouble vp[4];
    glGetDoublev(GL_VIEWPORT, vp); // ok
    float width  = (float)vp[2];
    float height = (float)vp[3];

    // get mouse deltas
    double dx = (beginx - x) / width;
    double dy = (beginy - y) / height;

    //cam location in spherical coordinates (this should be relative to the stare point
    vgl_point_3d<double> cam_center = cam_.get_camera_center();

    vgl_point_3d<double> origin(stare_point_);

    vgl_h_matrix_3d<double> T0;
    T0.set_identity();
    T0.set_translation(-origin.x(), -origin.y(), -origin.z());
    vnl_vector_fixed<double,3> cam_x(cam_.get_rotation().as_matrix().get_row(0));
    vnl_vector_fixed<double,3> cam_y(cam_.get_rotation().as_matrix().get_row(1));
    //vnl_vector_fixed<double,3> cam_z(cam_.get_rotation().as_matrix().get_row(2));
    // we want to rotate around the camera x and y axes
    static const double angle_scale = 0.2;
    vgl_rotation_3d<double> Rx(cam_x * -dy * angle_scale);
    vgl_rotation_3d<double> Ry(cam_y * -dx * angle_scale);

    // now compose all the transformations to get total movement of camera center
    vgl_h_matrix_3d<double> composed = T0.get_inverse() * Rx.as_h_matrix_3d() * Ry.as_h_matrix_3d() * T0;

    vgl_point_3d<double> new_center = composed * vgl_homg_point_3d<double>(cam_center);
    cam_.set_camera_center(new_center);
    cam_.look_at(stare_point_, vgl_vector_3d<double>(-cam_y[0], -cam_y[1], -cam_y[2]));

    this->post_redraw();
    return true;
  }

  // ZOOMING
  if (c_mouse_zoom(button, modifier)) {

    //get viewport size
    GLdouble vp[4];
    glGetDoublev(GL_VIEWPORT, vp); // ok
    double width = vp[2];
    double height = vp[3];

    //dy determines distance change in principle axis
    double dy = (beginy - y) / height;

        double scale = dy/2.0f;
    cam_.set_camera_center(cam_.get_camera_center() + scale_*scale*cam_.principal_axis());

    this->post_redraw();
    return true;
  }

  // TRANSLATION
  if (c_mouse_translate(button, modifier)) {
    //std::cout<<"Translating: begin ("<<beginx<<','<<beginy<<") -> ("<<x<<','<<y<<')'<<std::endl;

    //get viewport height, and mouse dx, dy
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp); // ok
    double width = (double)vp[2];
    double height = (double)vp[3];
    double dx = (beginx - x) / width;
    double dy = (beginy - y) / height;
    double scale = .5 * std::sqrt(std::pow(dx,2.0) + std::pow(dy,2.0));


    //get cam space points
    vgl_point_3d<double> p0(beginx/width, beginy/height, 0.0);
    vgl_point_3d<double> p1(x/width, y/height, 0.0);

    //transform points to world space
    vgl_point_3d<double> cam_center = cam_.get_camera_center();
    vgl_point_3d<double> wp0 = cam_.get_rotation().inverse()*p0;
    vgl_point_3d<double> wp1 = cam_.get_rotation().inverse()*p1;
    wp0 += cam_center - vgl_point_3d<double>(0,0,0);
    wp1 += cam_center - vgl_point_3d<double>(0,0,0);
    vgl_vector_3d<double> worldVec = scale * normalized(wp0-wp1);

    //std::cout<<"  new cam center: "<<cam_center + worldVec<<std::endl;
    stare_point_ += worldVec;
    cam_.set_camera_center(cam_center + worldVec);

    this->post_redraw();
    return true;
  }

  return false;
}

//:
// \todo i don't think this really does anything.
bool boxm2_cam_tableau::mouse_up(int x, int y, vgui_button button, vgui_modifier modifier)
{
  // SPINNING
  if (c_mouse_rotate(button, modifier))
  {
    GLdouble vp[4];
    glGetDoublev(GL_VIEWPORT, vp); // ok
    double width = vp[2];
    double height = vp[3];

    double wscale = 2.0 / width;
    double hscale = 2.0 / height;
    float delta_r[4];
    trackball(delta_r,
              static_cast<float>(wscale*beginx - 1), static_cast<float>(hscale*beginy - 1),
              static_cast<float>(wscale*x - 1), static_cast<float>(hscale*y - 1));
  }
  return false;
}
