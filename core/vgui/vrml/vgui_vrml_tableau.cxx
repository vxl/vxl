//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation
#endif
//
// .NAME vgui_vrml_tableau
// Author: Philip C. Pritchett, RRG, University of Oxford
// Created: 17 Sep 99
//
//-----------------------------------------------------------------------------

#include "vgui_vrml_tableau.h"

#include <Qv/QvVrmlFile.h>
#include <Qv/QvVisitor.h>

#include <vcl_iostream.h>

#include <vnl/vnl_math.h>

#include <vbl/vbl_file.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_glu.h>
#include <vgui/vgui_event.h>

#include "vgui_vrml_texture_map.h"
#include "vgui_vrml_draw_visitor.h"

static bool debug= false;


vgui_vrml_tableau::vgui_vrml_tableau(const char* filename, bool scale)
  : rescale_model( true)
{
  vrml = new QvVrmlFile(filename);
  drawer = new vgui_vrml_draw_visitor;
  vgui_vrml_texture_map::vrml_dirname = vbl_file::dirname(vrml->get_filename());


  if (scale) vrml->compute_centroid_radius();

  setup_dl = GL_INVALID_VALUE;
}


vgui_vrml_tableau::~vgui_vrml_tableau()
{
  delete vrml;
  delete drawer;
}

void vgui_vrml_tableau::invalidate_vrml()
{
  setup_dl = GL_INVALID_VALUE;
  post_redraw();
}

vcl_string vgui_vrml_tableau::type_name() const {
  return "vgui_vrml_tableau";
}

vcl_string vgui_vrml_tableau::pretty_name() const {
  vcl_string fn = vrml?(vrml->get_filename()):"null";
  return type_name() + "[" + fn + "]";
}

vcl_string vgui_vrml_tableau::file_name() const {
  if (vrml)
    return (vrml->get_filename());

  return type_name();
}


bool vgui_vrml_tableau::handle(const vgui_event &e)
{
  if (debug) vcl_cerr << "vgui_vrml_tableau::draw" << vcl_endl;

  if (!glIsEnabled(GL_LIGHTING))
    glColor3f(1,1,1);

  int mode = 1;
  if (mode == 0 /*e.user == &vgui_3D::wireframe*/) {
    if (debug) vcl_cerr << "vgui_vrml_tableau wireframe" << vcl_endl;
    if (drawer->get_gl_mode() != vgui_vrml_draw_visitor::wireframe) {
      drawer->set_gl_mode(vgui_vrml_draw_visitor::wireframe);
      setup_dl = GL_INVALID_VALUE;
    }
  }
  else if (mode == 1 /*e.user == &vgui_3D::textured*/) {
    if (debug) vcl_cerr << "vgui_vrml_tableau textured" << vcl_endl;
    if (drawer->get_gl_mode() != vgui_vrml_draw_visitor::textured) {
      drawer->set_gl_mode(vgui_vrml_draw_visitor::textured);
      setup_dl = GL_INVALID_VALUE;
    }
  }


  if (e.type != vgui_DRAW)
    return false;

  //
  double scale = 10 / vrml->radius;

  if( rescale_model)
    {
      glScalef(scale, scale, scale);
      glTranslatef(-vrml->centroid[0], -vrml->centroid[1], -vrml->centroid[2]);
    }

  //extern void projective_skew(float*);
  //projective_skew(token.trans);

  if (setup_dl == GL_INVALID_VALUE) {
    if (debug) vcl_cerr << "vgui_vrml_tableau  generating display list" << vcl_endl;

    setup_dl = glGenLists(1);
    glNewList(setup_dl, GL_COMPILE_AND_EXECUTE);

    vrml->traverse(drawer);

    glEndList();
   }
  else {
    if (debug) vcl_cerr << "vgui_vrml_tableau  using display list" << vcl_endl;
    glCallList(setup_dl);
  }

  return true;
}
