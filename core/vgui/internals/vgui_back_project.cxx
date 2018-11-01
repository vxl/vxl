// This is core/vgui/internals/vgui_back_project.cxx
//:
// \file
// \author fsm
// \brief  See vgui_back_project.h for a description of this file.

#include "vgui_back_project.h"
#include <vgui/vgui_gl.h>
#include "vgui_transpose_4x4.h"
#include "vgui_multiply_4x4.h"

static
void fast_null(double const P0[4],
               double const P1[4],
               double const P2[4],
               double f[4])
{
  f[0] = -P0[1]*P1[2]*P2[3]+P0[1]*P1[3]*P2[2]+P1[1]*P0[2]*P2[3]-P1[1]*P0[3]*P2[2]-P2[1]*P0[2]*P1[3]+P2[1]*P0[3]*P1[2];
  f[1] =  P0[0]*P1[2]*P2[3]-P0[0]*P1[3]*P2[2]-P1[0]*P0[2]*P2[3]+P1[0]*P0[3]*P2[2]+P2[0]*P0[2]*P1[3]-P2[0]*P0[3]*P1[2];
  f[2] = -P0[0]*P1[1]*P2[3]+P0[0]*P1[3]*P2[1]+P1[0]*P0[1]*P2[3]-P1[0]*P0[3]*P2[1]-P2[0]*P0[1]*P1[3]+P2[0]*P0[3]*P1[1];
  f[3] =  P0[0]*P1[1]*P2[2]-P0[0]*P1[2]*P2[1]-P1[0]*P0[1]*P2[2]+P1[0]*P0[2]*P2[1]+P2[0]*P0[1]*P1[2]-P2[0]*P0[2]*P1[1];
}

bool vgui_back_project_d3d4d4d4d4_d4(double const x[3],
                                     double const P0[4],
                                     double const P1[4],
                                     double const P2[4],
                                     double const pi[4],
                                     double X[4])
{
  // X = ( (x[0] P1.P2 + x[1] P2.P0 + x[2] P0.P1).pi )*
  //             f0           f1           f2
  double f0[4]; fast_null(P1, P2, pi,  f0);
  double f1[4]; fast_null(P2, P0, pi,  f1);
  double f2[4]; fast_null(P0, P1, pi,  f2);
  for (unsigned i=0; i<4; ++i)
    X[i] = x[0]*f0[i] + x[1]*f1[i] + x[2]*f2[i];
  return true;
}

bool vgui_back_project_d3d4_d4(double const x[3], double const pi[4], double X[4])
{
  double P[4][4]; glGetDoublev(GL_PROJECTION_MATRIX, &P[0][0]); vgui_transpose_4x4(P);
  double M[4][4]; glGetDoublev(GL_MODELVIEW_MATRIX,  &M[0][0]); vgui_transpose_4x4(M);
  double T[4][4]; vgui_multiply_4x4(P, M, T);
  return vgui_back_project_d3d4d4d4d4_d4(x, T[0], T[1], T[3], pi, X);
}

bool vgui_back_project_iid4_d4(int vx, int vy, double const pi[4], double X[4])
{
  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  double x[3] = { 2*(vx-vp[0])/double(vp[2])-1,
                  2*(vy-vp[1])/double(vp[3])-1,
                  1 };
  return vgui_back_project_d3d4_d4(x, pi, X);
}

bool vgui_back_project_ii_dd(int vx, int vy, double *x, double *y)
{
  double const pi[4] = { 0, 0, 1, 0 };
  double X[4];
  if (!vgui_back_project_iid4_d4(vx, vy, pi, X))
    return false;
  if (X[3] == 0)
    return false;
  if (x) *x = X[0]/X[3];
  if (y) *y = X[1]/X[3];
  return true;
}
