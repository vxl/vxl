// This is oxl/vgui/vgui_viewer3D.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   14-SEP-1999
// \brief  See vgui_viewer3D.h for a description of this file
//
// \verbatim
//  Modifications:
//    14-SEP-1999 P.Pritchett - Initial version.
// \endverbatim

#include "vgui_viewer3D.h"

#include <vcl_cmath.h>
#include <vcl_algorithm.h>

#include <vbl/vbl_bool_ostream.h>
#include <vul/vul_sprintf.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_glu.h>
#include <vgui/internals/trackball.h>
#include <vgui/vgui.h>
//#include <vgui/vgui_3D.h>
#include <vgui/vgui_event.h>

static bool debug=false;

struct vgui_viewer3D_spin {
  vgui_viewer3D *viewer;
  float delta_r[4];
  double delay;
};

const void * const vgui_viewer3D::SPIN_EVENT="x";

//: Constructor - don't use this, use vgui_viewer3D_new.
vgui_viewer3D::vgui_viewer3D(vgui_tableau_sptr const& s) :
  vgui_wrapper_tableau(s),
  c_mouse_rotate(vgui_LEFT),
  c_mouse_translate(vgui_RIGHT),
  c_mouse_zoom(vgui_MIDDLE),
  c_lock_dolly(vgui_key('D')),
  c_lock_zoom(vgui_key('Z')),
  c_lighting(vgui_key('l')),
  c_shading(vgui_key('s')),
  c_spinning(vgui_key('p')),
  c_render_mode(vgui_key('r')),
  c_niceness(vgui_key('n')),
  c_headlight(vgui_key('h')),
  c_save_home(vgui_key('>')),
  c_restore_home(vgui_key('<')),
  lock_dolly(false),
  lock_zoom(true),
  spin_data(0)
{
  spinning = false;
  allow_spinning = true;

  trackball(token.quat , 0.0, 0.0, 0.0, 0.0);
  token.scale = 1.0;

  token.trans[0] = 0;
  token.trans[1] = 0;
  token.trans[2] = -10;

  home = token;

  gl_mode = textured;
  lighting = true;
  smooth_shading = true;
  high_quality = true;
  headlight = true;
}

vgui_viewer3D::~vgui_viewer3D()
{
}

vcl_string vgui_viewer3D::type_name() const {return "vgui_viewer3D";}

static void draw_headlight() {
  //glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
  GLfloat light0_pos[4]   = {  0.0, 0.0, 1.0, 0.0 };
  //GLfloat light0_dir[4]   = {  0.0, 0.0, -1.0, 0.0 };
  GLfloat light0_diff[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
  GLfloat light0_amb[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
  GLfloat light0_spec[4] = { 0.1f, 0.1f, 0.1f, 1.0f };

  glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
  //glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light0_dir);
  //glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 1080.0);
  glLightfv(GL_LIGHT0, GL_AMBIENT,  light0_amb);
  glLightfv(GL_LIGHT0, GL_DIFFUSE,  light0_diff);
  glLightfv(GL_LIGHT0, GL_SPECULAR,  light0_spec);

  //glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.1);
  //glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0);
  //glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0);

  glEnable(GL_LIGHT0);
}


void vgui_viewer3D::setup_gl_matrices() {
  GLdouble vp[4];
  glGetDoublev(GL_VIEWPORT, vp); // ok
  double width = vp[2];
  double height = vp[3];

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(token.fov, width / height, 1, 1000);
  //glOrtho(-10,10,-10,10,-20000,10000);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glTranslatef(0,0,-10);      // object position

  glTranslatef(token.trans[0], token.trans[1], token.trans[2]);
  glScalef(token.scale, token.scale, token.scale);
  GLfloat m[4][4];
  build_rotmatrix(m,token.quat); // rotation
  glMultMatrixf(&m[0][0]);

  if (headlight)
    draw_headlight();
  else
    glDisable(GL_LIGHT0);
}


void vgui_viewer3D::draw_before_child()
{
  if (debug) vcl_cerr << "vgui_viewer3D::draw_before_child\n";

  // Setup OpenGL for 3D
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_TEXTURE_2D);
  //glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
  //glEnable(GL_COLOR_MATERIAL);

  glEnable(GL_NORMALIZE);

  glDepthFunc(GL_LESS);
  glEnable(GL_DEPTH_TEST);
  glDepthRange(0.0, 1.0);

  if (lighting)
    glEnable(GL_LIGHTING);
  else
    glDisable(GL_LIGHTING);


  if (smooth_shading)
    glShadeModel(GL_SMOOTH);
  else
    glShadeModel(GL_FLAT);

  if (high_quality) {
    glEnable(GL_DITHER);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  }
  else {
    glDisable(GL_DITHER);
    glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
    glHint(GL_POINT_SMOOTH_HINT, GL_FASTEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  }

  //glClear(GL_DEPTH_BUFFER_BIT);
  setup_gl_matrices();
}


bool vgui_viewer3D::handle(const vgui_event& e)
{
  if (this->spinning) {
    if (c_mouse_rotate(e) ||
        c_mouse_translate(e) ||
        c_mouse_zoom(e))
      this->spinning = false;
  }

  if (this->allow_spinning && this->spinning && event.user == &vgui_viewer3D::SPIN_EVENT) {

    vgui_viewer3D_spin const* spin_data = (vgui_viewer3D_spin const*)event.data;

    if (spin_data->viewer == this) {

      //vcl_cerr << "spinning\n";
      vcl_cerr << "-";
      if (debug) {
        vcl_cerr << vul_sprintf("spin_data->delta_r %f %f %f %f",
                            spin_data->delta_r[0],
                            spin_data->delta_r[1],
                            spin_data->delta_r[2],
                            spin_data->delta_r[3]) << vcl_endl;
      }

      add_quats(spin_data->delta_r, lastpos.quat, this->token.quat);

      // lastpos.quat = this->token.quat; // SGI CC can't do this.
      for (unsigned i=0; i<4; ++i) lastpos.quat[i] = this->token.quat[i];

      this->post_redraw();

      //Fl::add_idle(spin_callback,spin_data);
    }
  }

  event = e;

  if (vgui_drag_mixin::handle(e))
    return true;

  if (vgui_tableau::handle(e))
    return true;


  if (e.type == vgui_DRAW) {
    if (debug) vcl_cerr << "vgui_viewer3D vgui_DRAW\n";
    draw_before_child();
#if 0
    vgui_event eprime = e;

    if (gl_mode == textured) {
      if (debug) vcl_cerr << "vgui_viewer3D textured\n";
      eprime.user = (void*) &vgui_3D::textured;
    }
    else if (gl_mode == wireframe) {
      if (debug) vcl_cerr << "vgui_viewer3D wireframe\n";
      eprime.user = (void*) &vgui_3D::wireframe;
    }

    child->handle(eprime);
#else
    child && child->handle(e);
#endif
    return true;
  }
  else {
    setup_gl_matrices();

    bool used = child->handle(e);
    return used;
  }
}

inline bool gotmods(vgui_event const& event)
{
  return event.modifier_is_down(vgui_SHIFT);
}

bool vgui_viewer3D::mouse_down(int x, int y, vgui_button button, vgui_modifier /*modifier*/)
{
  if (c_mouse_rotate(event) || c_mouse_translate(event) || c_mouse_zoom(event)) {
    beginx = x;
    beginy = y;
    lastpos = this->token;
    last = event;
    this->spinning = false;
    return true;
  }

  return false;
}

bool vgui_viewer3D::mouse_drag(int x, int y, vgui_button button, vgui_modifier modifier) {

  // SPINNING
  if (c_mouse_rotate(button, modifier)) {
    if (debug) vcl_cerr << "vgui_trackball_handler::left \n";

    GLdouble vp[4];
    glGetDoublev(GL_VIEWPORT, vp); // ok
    double width = vp[2];
    double height = vp[3];

    double wscale = 2.0 / width;
    double hscale = 2.0 / height;
    float delta_r[4];
    trackball(delta_r,
              wscale*beginx - 1, hscale*beginy - 1,
              wscale*x - 1, hscale*y - 1);
    add_quats(delta_r, lastpos.quat, this->token.quat);

    prevx = x;
    prevx = y;

    this->post_redraw();
    return true;
  }

  // ZOOMING
  if (c_mouse_zoom(button, modifier)) {
    if (debug) vcl_cerr << "vgui_trackball_handler::middle\n";

    GLdouble vp[4];
    glGetDoublev(GL_VIEWPORT, vp); // ok
    double width = vp[2];
    double height = vp[3];


    double dx = (beginx - x) / width;
    double dy = (beginy - y) / height;

    double scalefactor = vcl_pow(5, dy);
    if (!lock_dolly)
      this->token.scale = lastpos.scale * scalefactor;

    double zoomfactor = vcl_pow(5,dx);
    if (!lock_zoom) {
      this->token.fov = lastpos.fov * zoomfactor;
      vgui::out << "viewer3D : fov " << this->token.fov << vcl_endl;
    }
    this->post_redraw();
    return true;
  }

  // TRANSLATION
  if (c_mouse_translate(button, modifier)) {
    if (debug) vcl_cerr << "vgui_trackball_handle::right\n";

  vcl_cerr << "handle! "<< button << "\n";

    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp); // ok
    double width = (double)vp[2];
    double height = (double)vp[3];


    double dx = (beginx - x) / width;
    double dy = (beginy - y) / height;

    this->token.trans[0] = lastpos.trans[0] - dx * 20;
    this->token.trans[1] = lastpos.trans[1] - dy * 20;

    this->post_redraw();
    return true;
  }
  return false;
}

bool vgui_viewer3D::mouse_up(int x, int y, vgui_button button, vgui_modifier modifier) {

  // SPINNING
  if (this->allow_spinning && c_mouse_rotate(button, modifier)) {
    if (debug) vcl_cerr << "vgui_trackball_handler::left \n";

    GLdouble vp[4];
    glGetDoublev(GL_VIEWPORT, vp); // ok
    double width = vp[2];
    double height = vp[3];

    double wscale = 2.0 / width;
    double hscale = 2.0 / height;
    float delta_r[4];
    trackball(delta_r,
              wscale*beginx - 1, hscale*beginy - 1,
              wscale*x - 1, hscale*y - 1);

    if (beginx != x && beginy != y) {

      this->spinning = true;
      double delay = event.secs_since(last);

      delete spin_data;
      spin_data = new vgui_viewer3D_spin;
      spin_data->viewer = this;
      spin_data->delay = delay;
      for (int i=0; i<4; ++i)
        spin_data->delta_r[i] = delta_r[i];


      // Fl::add_timeout(delay,spin_callback,(void*)spin_data);

      return true;
    }
  }
  return false;
}

bool vgui_viewer3D::help() {
  vcl_cerr << vcl_endl;
  // awfawf fixme
  vcl_cerr << "-- vgui_viewer3D ---------\n";
  vcl_cerr << "|     mouse              |\n";
  vcl_cerr << "| shift+left        zoom |\n";
  vcl_cerr << "| shift+middle    rotate |\n";
  vcl_cerr << "| shift+right  translate |\n";
  vcl_cerr << "|                        |\n";
  vcl_cerr << "|     keys               |\n";
  vcl_cerr << "| `D'         lock dolly |\n";
  vcl_cerr << "| `Z'          lock zoom |\n";
  vcl_cerr << "| `l'           lighting |\n";
  vcl_cerr << "| `s'            shading |\n";
  vcl_cerr << "| `p'           spinning |\n";
  vcl_cerr << "| `n'           niceness |\n";
  vcl_cerr << "| `h'          headlight |\n";
  vcl_cerr << "| `>'          save home |\n";
  vcl_cerr << "| `<'       restore home |\n";
  vcl_cerr << "--------------------------\n";
  vcl_cerr << vcl_endl;

  return false;
}

bool vgui_viewer3D::key_press(int, int, vgui_key key, vgui_modifier modifier) {
  if (c_lock_dolly(key, modifier)) {
    lock_dolly = !lock_dolly;
    vgui::out << "viewer3D : dolly lock " << vbl_bool_ostream::on_off(lock_dolly) << vcl_endl;
    return true;
  }

  if (c_lock_zoom(key, modifier)) {
    lock_zoom = !lock_zoom;
    vgui::out << "viewer3D : zoom lock " << vbl_bool_ostream::on_off(lock_zoom) << vcl_endl;
    return true;
  }

  if (c_lighting(key, modifier)) {
    this->lighting = !this->lighting;
    vgui::out << "viewer3D : lighting " << vbl_bool_ostream::on_off(this->lighting) << vcl_endl;
    this->post_redraw();
    return true;
  }

  if (c_shading(key, modifier)) {
    this->smooth_shading = !this->smooth_shading;
    vgui::out << "viewer3D : smooth shading " << vbl_bool_ostream::on_off(this->smooth_shading) << vcl_endl;
    this->post_redraw();
    return true;
  }

  if (c_spinning(key, modifier)) {
    this->allow_spinning = !this->allow_spinning;
    vgui::out << "viewer3D : allow spinning " << vbl_bool_ostream::on_off(this->allow_spinning) << vcl_endl;
    return true;
  }

  if (c_render_mode(key, modifier)) {
    if (this->gl_mode == vgui_viewer3D::wireframe) {
      vgui::out << "viewer3D : textured rendering\n";
      this->gl_mode = vgui_viewer3D::textured;
    }
    else if (this->gl_mode == vgui_viewer3D::textured) {
      vgui::out << "viewer3D : wireframe rendering\n";
      this->gl_mode = vgui_viewer3D::wireframe;
    }
    this->post_redraw();
    return true;
  }

  if (c_niceness(key, modifier)) {
    this->high_quality = !this->high_quality;
    vgui::out << "viewer3D : " << vbl_bool_ostream::high_low(this->high_quality) << " quality \n";
    this->post_redraw();
    return true;
  }

  if (c_headlight(key, modifier)) {
    this->headlight = !this->headlight;
    vgui::out << "viewer3D : headlight " << vbl_bool_ostream::on_off(this->headlight) << vcl_endl;
    this->post_redraw();
    return true;
  }

  if (c_save_home(key, modifier)) {
    this->home = this->token;
    vgui::out << "viewer3D : saving position to 'home'\n";
    return true;
  }

  if (c_restore_home(key, modifier)) {
    vgui::out << "viewer3D : restoring position 'home'\n";
    this->token = this->home;
    this->post_redraw();
    return true;
  }

  return false;
}
