// This is oxl/vgui/vgui_load.h
#ifndef vgui_load_h_
#define vgui_load_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Geoffrey Cross, Oxford RRG
// \date   03 Nov 99
// \brief  Tableau where the user can set the projection and modelview matrices.
//
//  Contains classes:  vgui_load   vgui_load_new
//
// \verbatim
//  Modifications:
//    03-Nov-1999 Geoff Initial version.
//    04-Jan-2000 fsm@robots. added set_ortho(), thereby obsoleting 
//                            view_section and view_volume.
//    05-Jan-2000 fsm@robots. added set_identity(), thereby obsoleting
//                            reset_matrix_state.
//    14-Aug-2002 K.Y.McGaul - Changed to and added Doxygen style comments.
// \endverbatim

template <class T> class vnl_matrix;
#include <vgui/vgui_load_sptr.h>
#include <vgui/vgui_wrapper_tableau.h>

//: Tableau where the user can set the projection and modelview matrices.
//
//  Class vgui_load is a tableau which (optionally) loads given values
//  for the projection and modelview matrices before passing control to
//  its child. This is typically used to initialize GL before rendering a
//  scene.
class vgui_load : public vgui_wrapper_tableau
{
 public:

  //: Constructor - don't use this, use vgui_load_new.
  vgui_load(vgui_tableau_sptr const& child);

  //: Returns the type of this tableau ('vgui_load').
  vcl_string type_name() const;

  //: Set the projection matrix to the given matrix.
  void set_projection(vnl_matrix<double> const &m);

  //: Set the modelview matrix to the given matrix.
  void set_modelview(vnl_matrix<double> const &m);

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
  void set_image(unsigned w, unsigned h) { set_ortho(0, h, w, 0); }

 protected:
  ~vgui_load() { }

  bool handle( vgui_event const &e);

  bool projectionmatrixloaded;
  bool modelviewmatrixloaded;

  // note: for efficiency, these are pre-transformed, ie stored in GL format.
  //vnl_matrix<double> projectionmatrixt;
  //vnl_matrix<double> modelviewmatrixt;
  double projectionmatrixt[16];
  double modelviewmatrixt[16];
};

//: Create a smart-pointer to a vgui_load tableau.
struct vgui_load_new : public vgui_load_sptr
{
  typedef vgui_load_sptr base;
  vgui_load_new(vgui_tableau_sptr const& child) : base(new vgui_load(child)) { }
};

#endif // vgui_load_h_
