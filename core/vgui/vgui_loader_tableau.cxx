// This is core/vgui/vgui_loader_tableau.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Geoffrey Cross, Oxford RRG
// \date   03 Nov 99
// \brief  See vgui_loader_tableau.h for a description of this file.

#include "vgui_loader_tableau.h"
#include <vcl_iostream.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgui/vgui_gl.h>

vcl_string vgui_loader_tableau::type_name() const
{
  return "vgui_loader_tableau";
}

// Default ctor
vgui_loader_tableau::vgui_loader_tableau( vgui_tableau_sptr const&child)
  : vgui_wrapper_tableau( child),
    projectionmatrixloaded( false),
    modelviewmatrixloaded( false)
    //projectionmatrixt(4,4),
    //modelviewmatrixt(4,4)
{
}

void vgui_loader_tableau::set_projection( vnl_matrix_fixed<double,4,4> const &m)
{
  projectionmatrixloaded= true;
  m.transpose().copy_out(projectionmatrixt);
}

void vgui_loader_tableau::unset_projection()
{
  projectionmatrixloaded= false;
}

void vgui_loader_tableau::set_modelview( vnl_matrix_fixed<double,4,4> const &m)
{
  modelviewmatrixloaded= true;
  m.transpose().copy_out(modelviewmatrixt);
}

void vgui_loader_tableau::unset_modelview()
{
  modelviewmatrixloaded= false;
}

bool vgui_loader_tableau::handle( vgui_event const &e)
{
  if (projectionmatrixloaded)
  {
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd( projectionmatrixt );
  }

  if (modelviewmatrixloaded)
  {
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd( modelviewmatrixt );
  }

  return child && child->handle(e);
}

//--------------------------------------------------------------------------------

void vgui_loader_tableau::set_identity()
{
  vnl_matrix_fixed<double,4,4> I; I.set_identity();
  set_projection(I);
  set_modelview(I);
}

void vgui_loader_tableau::set_ortho(float x1,float y1,float z1, float x2,float y2,float z2)
{
  if (x1==x2 || y1==y2 || z1==z2)
    vcl_cerr << __FILE__ " warning in set_ortho() : volume has no extent\n";

  vnl_matrix_fixed<double,4,4> M; M.set_identity();
  set_projection(M);

  M(0,0) = 2/(x2-x1); M(0,3) = (x1+x2)/(x1-x2);
  M(1,1) = 2/(y2-y1); M(1,3) = (y1+y2)/(y1-y2);
  M(2,2) = 2/(z2-z1); M(2,3) = (z1+z2)/(z1-z2);
  set_modelview(M);
}

void vgui_loader_tableau::set_ortho(float x1, float y1, float x2, float y2)
{
  set_ortho(x1,y1, -1, x2,y2, +1);
}
