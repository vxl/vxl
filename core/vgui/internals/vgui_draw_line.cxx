// This is core/vgui/internals/vgui_draw_line.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm

#include "vgui_draw_line.h"
#include <vgui/vgui_gl.h>
#include <vgui/internals/vgui_multiply_4x4.h>
#include <vgui/internals/vgui_transpose_4x4.h>

//--------------------------------------------------------------------------------

#define dot4(a, b) ((a)[0]*(b)[0] + (a)[1]*(b)[1] + (a)[2]*(b)[2] + (a)[3]*(b)[3])
#define l_c4(a, x, b, y, ax_plus_by) { for (int i=0; i<4; ++i) (ax_plus_by)[i] = (a)*(x)[i] + (b)*(y)[i]; }

bool vgui_draw_line(double const T[4][4], double const X[4], double const Y[4])
{
  double T0_X = dot4(T[0], X);
  double T1_X = dot4(T[1], X);
  double T3_X = dot4(T[3], X);

  double T0_Y = dot4(T[0], Y);
  double T1_Y = dot4(T[1], Y);
  double T3_Y = dot4(T[3], Y);

  double tmp[4];
  glBegin(GL_LINE_STRIP);

  // line x_dev = -w_dev :
  {
    double u = T0_X+T3_X;
    double v = T0_Y+T3_Y;
    double y_dev = (v*T1_X - u*T1_Y);
    double w_dev = (v*T3_X - u*T3_Y);
    if (!w_dev) { }
    else if ((w_dev>0) ? (-w_dev<=y_dev && y_dev<=w_dev) : (-w_dev>=y_dev && y_dev>=w_dev)) {
      l_c4(v, X, -u, Y, tmp);
      glVertex4dv(tmp);
    }
  }

  // line x_dev = +w_dev :
  {
    double u = T0_X-T3_X;
    double v = T0_Y-T3_Y;
    double y_dev = (v*T1_X - u*T1_Y);
    double w_dev = (v*T3_X - u*T3_Y);
    if (!w_dev) { }
    else if ((w_dev>0) ? (-w_dev<=y_dev && y_dev<=w_dev) : (-w_dev>=y_dev && y_dev>=w_dev)) {
      l_c4(v, X, -u, Y, tmp);
      glVertex4dv(tmp);
    }
  }

  // line y_dev = -w_dev :
  {
    double u = T1_X+T3_X;
    double v = T1_Y+T3_Y;
    double x_dev = (v*T0_X - u*T0_Y);
    double w_dev = (v*T3_X - u*T3_Y);
    if (!w_dev) { }
    else if ((w_dev>0) ? (-w_dev<=x_dev && x_dev<=w_dev) : (-w_dev>=x_dev && x_dev>=w_dev)) {
      l_c4(v, X, -u, Y, tmp);
      glVertex4dv(tmp);
    }
  }

  // line y_dev = +w_dev :
  {
    double u = T1_X-T3_X;
    double v = T1_Y-T3_Y;
    double x_dev = (v*T0_X - u*T0_Y);
    double w_dev = (v*T3_X - u*T3_Y);
    if (!w_dev) { }
    else if ((w_dev>0) ? (-w_dev<=x_dev && x_dev<=w_dev) : (-w_dev>=x_dev && x_dev>=w_dev)) {
      l_c4(v, X, -u, Y, tmp);
      glVertex4dv(tmp);
    }
  }

  glEnd();
  return true;
}

bool vgui_draw_line(double const P[4][4], double const M[4][4], double const X[4], double const Y[4])
{
  double T[4][4];
  vgui_multiply_4x4(P, M, T);
  return vgui_draw_line(T, X, Y);
}

bool vgui_draw_line(double const X[4], double const Y[4])
{
  double P[4][4]; glGetDoublev(GL_PROJECTION_MATRIX, &P[0][0]); vgui_transpose_4x4(P);
  double M[4][4]; glGetDoublev(GL_MODELVIEW_MATRIX,  &M[0][0]); vgui_transpose_4x4(M);

  double T[4][4];
  vgui_multiply_4x4(P, M, T);
  return vgui_draw_line(T, X, Y);
}

//--------------------------------------------------------------------------------

#define dot3(a, b) ((a)[0]*(b)[0] + (a)[1]*(b)[1] + (a)[2]*(b)[2])
#define cross3(a, b, c) { \
  (c)[0] = (a)[1]*(b)[2] - (a)[2]*(b)[1]; \
  (c)[1] = (a)[2]*(b)[0] - (a)[0]*(b)[2]; \
  (c)[2] = (a)[0]*(b)[1] - (a)[1]*(b)[0]; \
}
#define add3(a, b, c) { for (int i=0; i<3; ++i) (c)[i] = (a)[i] + (b)[i]; }
#define sub3(a, b, c) { for (int i=0; i<3; ++i) (c)[i] = (a)[i] - (b)[i]; }
#define trace(str) /*{ vcl_cerr << str << vcl_endl; }*/

bool vgui_draw_line(double const T[4][4], double a, double b, double c)
{
  double l[3] = { a, b, c };

  double M[3][3] = { // M = T([0 1 3], [0 1 3]);
    { T[0][0], T[0][1], T[0][3] },
    { T[1][0], T[1][1], T[1][3] },
    { T[3][0], T[3][1], T[3][3] }
  };

  double M0_l[3]; cross3(M[0], l, M0_l);
  double M1_l[3]; cross3(M[1], l, M1_l);
  double M2_l[3]; cross3(M[2], l, M2_l);

  double tmp[3];
  glBegin(GL_LINE_STRIP);

  // line x_dev = -w_dev :
  {
    add3(M0_l, M2_l, tmp);
    double y_dev = dot3(M[1], tmp);
    double w_dev = dot3(M[2], tmp);
    if (w_dev>0) {
      if (-w_dev<=y_dev && y_dev<=w_dev) {
        trace("x=-1"); glVertex4d( tmp[0],  tmp[1],  0,  tmp[2]); }
    }
    else if (w_dev<0) {
      if (-w_dev>=y_dev && y_dev>=w_dev) {
        trace("x=-1"); glVertex4f(-tmp[0], -tmp[1],  0, -tmp[2]); }
    }
    else {
    }
  }

  // line x_dev = +w_dev :
  {
    sub3(M0_l, M2_l, tmp);
    double y_dev = dot3(M[1], tmp);
    double w_dev = dot3(M[2], tmp);
    if (w_dev>0) {
      if (-w_dev<=y_dev && y_dev<=w_dev) {
        trace("x=+1"); glVertex4d( tmp[0],  tmp[1],  0,  tmp[2]); }
    }
    else if (w_dev<0) {
      if (-w_dev>=y_dev && y_dev>=w_dev) {
        trace("x=+1"); glVertex4f(-tmp[0], -tmp[1],  0, -tmp[2]); }
    }
    else {
    }
  }

  // line y_dev = -w_dev :
  {
    add3(M1_l, M2_l, tmp);
    double x_dev = dot3(M[0], tmp);
    double w_dev = dot3(M[2], tmp);
    if (w_dev>0) {
      if (-w_dev<=x_dev && x_dev<=w_dev) {
        trace("y=-1"); glVertex4d( tmp[0],  tmp[1],  0,  tmp[2]); }
    }
    else if (w_dev<0) {
      if (-w_dev>=x_dev && x_dev>=w_dev) {
        trace("y=-1"); glVertex4f(-tmp[0], -tmp[1],  0, -tmp[2]); }
    }
    else {
    }
  }

  // line y_dev = +w_dev :
  {
    sub3(M1_l, M2_l, tmp);
    double x_dev = dot3(M[0], tmp);
    double w_dev = dot3(M[2], tmp);
    if (w_dev>0) {
      if (-w_dev<=x_dev && x_dev<=w_dev) {
        trace("y=+1"); glVertex4d( tmp[0],  tmp[1],  0,  tmp[2]); }
    }
    else if (w_dev<0) {
      if (-w_dev>=x_dev && x_dev>=w_dev) {
        trace("y=+1"); glVertex4f(-tmp[0], -tmp[1],  0, -tmp[2]); }
    }
    else {
    }
  }

  trace("");
  glEnd();

  return true;
}

bool vgui_draw_line(double const P[4][4], double const M[4][4], double a, double b, double c)
{
  double T[4][4];
  vgui_multiply_4x4(P, M, T);
  return vgui_draw_line(T, a, b, c);
}

bool vgui_draw_line(double a, double b, double c)
{
  double P[4][4]; glGetDoublev(GL_PROJECTION_MATRIX, &P[0][0]); vgui_transpose_4x4(P);
  double M[4][4]; glGetDoublev(GL_MODELVIEW_MATRIX,  &M[0][0]); vgui_transpose_4x4(M);
  double T[4][4]; vgui_multiply_4x4(P, M, T);
  return vgui_draw_line(T, a, b, c);
}
