// This is oxl/vgui/vgui_load.h
#ifndef vgui_load_h_
#define vgui_load_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif

// .NAME vgui_load
// .INCLUDE vgui/vgui_load.h
// .FILE vgui_load.cxx
//
// .SECTION Description
// Class vgui_load is a tableau which (optionally) loads given values
// for the projection and modelview matrices before passing control to
// its child. This is typically used to initialize GL before rendering a
// scene.
//
// .SECTION Author
//   Geoffrey Cross, Oxford RRG
//   Created: 03 Nov 99
// .SECTION Modifications:
// 03 Nov 1999 Geoff Initial version.
// 04 Jan 2000 fsm@robots. added set_ortho(), thereby obsoleting view_section
//             and view_volume.
// 05 Jan 2000 fsm@robots. added set_identity(), thereby obsoleting
//             reset_matrix_state.

template <class T> class vnl_matrix;
#include <vgui/vgui_load_sptr.h>
#include <vgui/vgui_wrapper_tableau.h>

class vgui_load : public vgui_wrapper_tableau
{
 public:

  vgui_load(vgui_tableau_sptr const& child);

  vcl_string type_name() const;

  void set_projection(vnl_matrix<double> const &m);
  void set_modelview(vnl_matrix<double> const &m);

  void unset_projection();
  void unset_modelview();

  // Calling this method is equivalent to calling set_projection(I) and
  // set_modelview(I) with I an identity matrix.
  void set_identity();

  // These easy methods set the projection matrix to the identity and the
  // modelview matrix using glOrtho(). In each case, the coordinates given
  // are the desired coordinates of the viewport/render-volume corners.
  // Eg. to view an image which is w-by-h, one might use
  //   l.set_ortho(0,h, w,0);
  // unless one wants the image upside down.
  void set_ortho(float x1, float y1, // bottom, left
                 float x2, float y2);// top, right

  void set_ortho(float x1,float y1,float z1, // bottom, left hand, far corner.
                 float x2,float y2,float z2);// top, right hand, near corner.

  // default for a w-by-h image :
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

struct vgui_load_new : public vgui_load_sptr
{
  typedef vgui_load_sptr base;
  vgui_load_new(vgui_tableau_sptr const& child) : base(new vgui_load(child)) { }
};

#endif // vgui_load_h_
