/*
  fsm@robots.ox.ac.uk
*/
#include "vgui_test.h"

#include <vcl_cmath.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_matrix_state.h>

//--------------------------------------------------------------------------------

vgui_test_thingy2d::vgui_test_thingy2d() 
  : vgui_tableau() 
{
}

bool vgui_test_thingy2d::handle(const vgui_event &e) {
  if (e.type != vgui_DRAW)
    return false;
  
  glLineWidth(1);
  glBegin(GL_LINES);

  glColor3f(1,0,0); 
  glVertex2f(0,0); glVertex2f(1,0);

  glColor3f(0,1,0);
  glVertex2f(0,0); glVertex2f(0,1);

  glColor3f(0,0,1);
  glVertex2f(0,0); glVertex2f(1,1);
    
  glColor3f(0,1,1);
  glVertex2f(1,0); glVertex2f(1,1);

  glColor3f(1,0,1);
  glVertex2f(0,1); glVertex2f(1,1);

  glColor3f(1,1,0);
  glVertex2f(1,0); glVertex2f(0,1);

  glEnd(); // GL_LINES

  glColor3f(1,1,1);
  unsigned N=36;
  glBegin(GL_LINE_LOOP);
  for (unsigned i=0; i<N; ++i) {
    float v=2*3.14159*i/N;
    glVertex2f(vcl_cos(v), vcl_sin(v));
  }
  glEnd(); // GL_LINE_LOOP

  return true;
}

//--------------------------------------------------------------------------------

vgui_test_thingy3d::vgui_test_thingy3d()
  : vgui_tableau()
{
}

bool vgui_test_thingy3d::handle(const vgui_event &e) {
  if (e.type != vgui_DRAW)
    return false;

  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,1);
  //glDisable(GL_DITHER);
  glEnable(GL_DITHER);
  
  // the light source :
  glEnable(GL_LIGHT0);
  static GLfloat light_position[]={+1.32f,
				   +0.83f,
				   -1.50f,
				   0.0f}; // position of light source
  static GLfloat light_diffuse []={0,0,0,0};
  //static GLfloat light_specular[]={1,1,1,1};
  static GLfloat light_ambient []={.3f,.3f,.3f,.3f};
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_DIFFUSE , light_diffuse);
  //glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_AMBIENT , light_ambient);

  // material colours :
  GLfloat i=1.0; // intensity
  GLfloat a=1.0;
  static GLfloat mat_r[]={i , 0 , 0  ,a};
  static GLfloat mat_g[]={0 , i , 0  ,a};
  static GLfloat mat_b[]={0 , 0 , i  ,a};
  //static GLfloat mat_s[]={.1,.1,.1,.1};

  // the polygons :

  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_r);
  //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_s);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_r);
  glBegin(GL_TRIANGLES);
  glNormal3f(0,0,1);
  glVertex3f(0,0,0);
  glVertex3f(0,1,0);
  glVertex3f(1,0,0);
  glEnd();

  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_g);
  //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_s);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_g);
  glBegin(GL_TRIANGLES);
  glNormal3f(1,0,0);
  glVertex3f(0,0,0);
  glVertex3f(0,1,0);
  glVertex3f(0,0,1);
  glEnd();
  
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_b);
  //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_s);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_b);
  glBegin(GL_TRIANGLES);
  glNormal3f(0,1,0);
  glVertex3f(0,0,0);
  glVertex3f(0,0,1);
  glVertex3f(1,0,0);
  glEnd();

  glDisable(GL_LIGHTING);

  // lines
  glColor3f(1,1,1); // white
  glLineWidth(3);

  // boundary (white) of little triangle :
  {
    glBegin(GL_LINE_LOOP);
    glVertex3f(1,0,0);
    glVertex3f(0,1,0);
    glVertex3f(0,0,1);
    glEnd();
  }

  // exposed boundaries (white) of triangles
  {
    glBegin(GL_LINE_LOOP);
    glVertex3f(0.5, 0.5, 0.0);
    glVertex3f(0.5, 0.0, 0.5);
    glVertex3f(0.0, 0.5, 0.5);
    glEnd();
  }

  // axes :
  {
    glBegin(GL_LINES);
    glVertex3f(0,0,0); glVertex3f(1,0,0);
    glVertex3f(0,0,0); glVertex3f(0,1,0);
    glVertex3f(0,0,0); glVertex3f(0,0,1);
    glEnd();
  }

  return true;
}

//--------------------------------------------------------------------------------

vgui_test_credits::vgui_test_credits() : vgui_text() {
  vgui_text::add( 40,  50, "vgui - Visual Geometry User Interface");
  vgui_text::add( 25, 150, "written by : ");
  // well - myself and fred did do most of the work :)
  vgui_text::add(110, 180, "Philip Pritchett"); 
  vgui_text::add(110, 210, "Frederik Schaffalitzky"); 
  vgui_text::add(110, 240, "Andrew Fitzgibbon");
  vgui_text::add(110, 270, "Geoffrey Cross");
  vgui_text::add(110, 300, "Karen McGaul");
}

bool vgui_test_credits::handle(vgui_event const &e) {
  if (e.type != vgui_DRAW)
    return false;

  vgui_matrix_state PM;

  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, vp[2],
	  vp[3], 0,
	  -1,+1);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  return vgui_text::handle(e);
}

//--------------------------------------------------------------------------------
