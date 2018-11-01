// This is core/vgui/vgui_loader_tableau.h
#ifndef vgui_loader_tableau_h_
#define vgui_loader_tableau_h_
//:
// \file
// \brief  Tableau where the user can set the projection and modelview matrices.
// \author Geoffrey Cross, Oxford RRG
// \date   03 Nov 99
//
//  Contains classes  vgui_loader_tableau   vgui_loader_tableau_new
//
// \verbatim
//  Modifications
//   03-Nov-1999 Geoff Initial version.
//   04-Jan-2000 fsm. added set_ortho(), thereby obsoleting
//                           view_section and view_volume.
//   05-Jan-2000 fsm. added set_identity(), thereby obsoleting
//                           reset_matrix_state.
//   14-Aug-2002 K.Y.McGaul - Changed to and added Doxygen style comments.
//   01-OCT-2002 K.Y.McGaul - Moved vgui_load to vgui_loader_tableau.
// \endverbatim

#include <vnl/vnl_fwd.h>
#include <vgui/vgui_loader_tableau_sptr.h>
#include <vgui/vgui_wrapper_tableau.h>

//: Tableau where the user can set the projection and modelview matrices.
//
//  Class vgui_loader_tableau is a tableau which (optionally) loads given values
//  for the projection and modelview matrices before passing control to
//  its child. This is typically used to initialize GL before rendering a
//  scene.
class vgui_loader_tableau : public vgui_wrapper_tableau
{
 public:

  //: Constructor - don't use this, use vgui_loader_tableau_new.
  //  Takes the single child tableau as a parameter.
  vgui_loader_tableau(vgui_tableau_sptr const& child);

  //: Returns the type of this tableau ('vgui_loader_tableau').
  std::string type_name() const;

  //: Set the projection matrix to the given matrix.
  void set_projection(vnl_matrix_fixed<double,4,4> const &m);

  //: Set the modelview matrix to the given matrix.
  void set_modelview(vnl_matrix_fixed<double,4,4> const &m);

  //: Unset the projection matrix.
  void unset_projection();

  //: Unset the modelview matrix.
  void unset_modelview();

  //: Set projection and modalview matrices to the identity matrix.
  //  Calling this method is equivalent to calling set_projection(I) and
  //  set_modelview(I) with I an identity matrix.
  void set_identity();

  //: Set projection matrix to identity and modelview matrix using glOrtho().
  // These easy methods set the projection matrix to the identity and the
  // modelview matrix using glOrtho(). In each case, the coordinates given
  // are the desired coordinates of the viewport/render-volume corners.
  // Eg. to view an image which is w-by-h, one might use
  //   l.set_ortho(0,h, w,0);
  // unless one wants the image upside down.
  void set_ortho(float x1, float y1, // bottom, left
                 float x2, float y2);// top, right

  //: Set projection matrix to identity and modelview matrix using glOrtho().
  void set_ortho(float x1,float y1,float z1, // bottom, left hand, far corner.
                 float x2,float y2,float z2);// top, right hand, near corner.

  //: Default for a w-by-h image :
  void set_image(unsigned w, unsigned h) { set_ortho(0.f, float(h), float(w), 0.f); }

 protected:
  //: Destructor - called by vgui_loader_tableau_sptr.
  ~vgui_loader_tableau() { }

  bool handle( vgui_event const &e);

  bool projectionmatrixloaded;
  bool modelviewmatrixloaded;

  // note: for efficiency, these are pre-transformed, ie stored in GL format.
  double projectionmatrixt[16];
  double modelviewmatrixt[16];
};

//: Create a smart-pointer to a vgui_loader_tableau tableau.
struct vgui_loader_tableau_new : public vgui_loader_tableau_sptr
{
  typedef vgui_loader_tableau_sptr base;

  //: Constructor - takes the single child tableau as a parameter.
  vgui_loader_tableau_new(vgui_tableau_sptr const& child) : base(new vgui_loader_tableau(child)) { }
};

#endif // vgui_loader_tableau_h_
