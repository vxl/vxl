// This is oxl/vgui/vgui_load.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "vgui_load.h"
#include <vcl_iostream.h>
#include <vnl/vnl_matrix.h>
#include <vgui/vgui_gl.h>

vcl_string vgui_load::type_name() const {
  return "vgui_load";
}

// Default ctor
vgui_load::vgui_load( vgui_tableau_sptr const&child)
  : vgui_wrapper_tableau( child),
    projectionmatrixloaded( false),
    modelviewmatrixloaded( false)
    //projectionmatrixt(4,4),
    //modelviewmatrixt(4,4)
{
}

void vgui_load::set_projection( vnl_matrix<double> const &m)
{
  projectionmatrixloaded= true;
  m.transpose().copy_out(projectionmatrixt);
}

void vgui_load::unset_projection()
{
  projectionmatrixloaded= false;
}

void vgui_load::set_modelview( vnl_matrix<double> const &m)
{
  modelviewmatrixloaded= true;
  m.transpose().copy_out(modelviewmatrixt);
}

void vgui_load::unset_modelview()
{
  modelviewmatrixloaded= false;
}

bool vgui_load::handle( vgui_event const &e)
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

#if 0
  if (child) {
    vcl_cerr << "child = " << child << vcl_endl;

    bool (vgui_tableau::*method)(vgui_event const &);
    method = &vgui_tableau::handle;
    vcl_cerr << "method = " << method << vcl_endl;

    return (child->*method)(e);
  }
  else
    return false;
#else
  return child && child->handle(e);
#endif
}

//--------------------------------------------------------------------------------

void vgui_load::set_identity()
{
  vnl_matrix<double> I(4,4);
  I.set_identity();
  set_projection(I);
  set_modelview(I);
}

void vgui_load::set_ortho(float x1,float y1,float z1, float x2,float y2,float z2)
{
  if (x1==x2 || y1==y2 || z1==z2)
    vcl_cerr << __FILE__ " warning in set_ortho() : volume has no extent\n";

  vnl_matrix<double> M(4,4);

  M.set_identity();
  set_projection(M);

  M(0,0) = 2/(x2-x1); M(0,3) = (x1+x2)/(x1-x2);
  M(1,1) = 2/(y2-y1); M(1,3) = (y1+y2)/(y1-y2);
  M(2,2) = 2/(z2-z1); M(2,3) = (z1+z2)/(z1-z2);
  set_modelview(M);
}

void vgui_load::set_ortho(float x1, float y1, float x2, float y2)
{
  set_ortho(x1,y1, -1, x2,y2, +1);
}
