// This is oxl/vgui/vrml/vgui_vrml_tableau.h
#ifndef vgui_vrml_tableau_h_
#define vgui_vrml_tableau_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   17 Sep 99
// \brief  Tableau to display VRML.
// 
// \verbatim
//  Modifications:
//    17-SEP-1999 P.Pritchett - Initial version.
//    13-SEP-1999 K.Y.McGaul - Changed to Doxygen style comments.
// \endverbatim

#include <vgui/vgui_tableau.h>

#include <vgui/vgui_tableau_sptr.h>

class vgui_vrml_tableau;
typedef vgui_tableau_sptr_t<vgui_vrml_tableau> vgui_vrml_tableau_sptr;

class QvVrmlFile;
class vgui_vrml_draw_visitor;

//: Tableau to display VRML.
class vgui_vrml_tableau : public vgui_tableau
{
 public:
  vgui_vrml_tableau(const char* file, bool scale);
  ~vgui_vrml_tableau();

  // vgui_tableau methods/data
  bool handle(const vgui_event &);
  void invalidate_vrml();

  vcl_string file_name() const;
  vcl_string pretty_name() const;
  vcl_string type_name() const;

  QvVrmlFile* vrml;
  vgui_vrml_draw_visitor* drawer;
  int setup_dl;

  void set_rescale( bool flag) { rescale_model= flag; }

 private:

  bool rescale_model;
};

//: Creates a smart-pointer to a vgui_vrml_tableau.
struct vgui_vrml_tableau_new : public vgui_vrml_tableau_sptr
{
  typedef vgui_vrml_tableau_sptr base;
  vgui_vrml_tableau_new(char const* file, bool scale = true) : base(new vgui_vrml_tableau(file, scale)) { }
};


#endif // vgui_vrml_tableau_h_
