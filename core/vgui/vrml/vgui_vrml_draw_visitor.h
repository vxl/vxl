// This is oxl/vgui/vrml/vgui_vrml_draw_visitor.h
#ifndef vgui_vrml_draw_visitor_h_
#define vgui_vrml_draw_visitor_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//
// .NAME vgui_vrml_draw_visitor
// .LIBRARY vgui-vrml
// .HEADER vxl Package
// .INCLUDE vgui/vrml/vgui_vrml_draw_visitor.h
// .FILE vgui_vrml_draw_visitor.cxx
//
// .SECTION Author
//              Philip C. Pritchett, 15 Oct 99
//              Robotics Research Group, University of Oxford
//
// .SECTION Modifications
//   990109 AWF Initial version.
//
//-----------------------------------------------------------------------------

#include <Qv/QvVisitor.h>

//these may be typedefs in the glu header (linux),
//so they can't be forward declared like this.
//cf vcl_istream, vcl_ostream
//class GLUquadricObj;
#include <vgui/vgui_gl.h>
#include <vgui/vgui_glu.h>

class vgui_vrml_draw_visitor : public QvVisitor
{
 public:

  // Constructors/Destructors--------------------------------------------------

  vgui_vrml_draw_visitor();
 ~vgui_vrml_draw_visitor();

  bool Visit(QvSeparator* node);
  bool Visit(QvTransformSeparator* node);
  bool Visit(QvCube* node);
  bool Visit(QvSphere* node);
  bool Visit(QvCylinder* node);
  bool Visit(QvCone* node);
  bool Visit(QvPointSet* ps);
  bool Visit(QvIndexedLineSet* node);
  bool Visit(QvIndexedFaceSet* node);
  bool Visit(QvAsciiText* node);
  bool Visit(QvTransform* node);
  bool Visit(QvScale* node);
  bool Visit(QvRotation* node);
  bool Visit(QvTranslation* node);
  bool Visit(QvMaterial* node);
  bool Visit(QvTexture2* node);
  bool Visit(QvShapeHints* node);


  enum mode_type {wireframe, textured, shaded};

  mode_type get_gl_mode() {
    return gl_mode;
  }

  void set_gl_mode(mode_type gl_new) {
    if (gl_new == textured) remake_texture = true;
    gl_mode = gl_new;
  }

 protected:
  GLUquadricObj *quadric;
  mode_type gl_mode;
  bool twosided;
  bool remake_texture;
};

#endif // vgui_vrml_draw_visitor_h_
