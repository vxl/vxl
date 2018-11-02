// This is core/vgui/vgui_matrix_state.h
#ifndef vgui_matrix_state_h_
#define vgui_matrix_state_h_
//:
// \file
// \author  fsm
// \brief   Stores and retrieves the current projection and modelview matrices.
//
//  Contains class vgui_matrix_state
//
// \verbatim
//  Modifications
//   AWF Renamed store, and made it save and restore on
//       construction/destruction.
//   FSM Renamed method names for consistency with projection_inspector.
// \endverbatim

#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgui/vgui_gl.h>
#include <vnl/vnl_fwd.h>

//: Stores and retrieves the current projection and modelview matrices.
//
// vgui_matrix_state stores the current OpenGL projection and modelview
// matrices when save() is called. The matrices are restored to these values
// on the next call to restore(). Normally, the constructor calls save() and
// the destructor calls restore(), but this can be overridden.
//
// vgui_matrix_state also has various static convenience methods for GL matrix
// handling.
class vgui_matrix_state
{
  // NB : matrices stored in column (fortran) order.
  double P[16]; // projection
  double M[16]; // modelview
  bool restore_on_destroy;

 public:
  vgui_matrix_state(bool save_now_restore_on_destroy = true);
  ~vgui_matrix_state();

  void save();
  void restore() const;
  void print(std::ostream& );

  // set
  static void identity_gl_matrices(); // set both matrices to the identity.
  static void clear_gl_matrices();    // set both matrices to zero.
  static void zero_out_gl_matrices() { clear_gl_matrices(); }

  // query
  static bool gl_matrices_are_cleared();
  static vnl_matrix_fixed<double,4,4> projection_matrix();
  static vnl_matrix_fixed<double,4,4> modelview_matrix();
  static vnl_matrix_fixed<double,4,4> total_transformation();

  // Projection matrices
  static void premultiply(vnl_matrix_fixed<double,4,4> const &,GLenum );
  static void premultiply_projection(vnl_matrix_fixed<double,4,4> const &M) { premultiply(M,GL_PROJECTION); }
  static void premultiply_modelview (vnl_matrix_fixed<double,4,4> const &M) { premultiply(M,GL_MODELVIEW); }

  static void postmultiply(const vnl_matrix_fixed<double,4,4> &M,GLenum matrix);
  static void postmultiply_projection(vnl_matrix_fixed<double,4,4> const &M) { postmultiply(M,GL_PROJECTION); }
  static void postmultiply_modelview (vnl_matrix_fixed<double,4,4> const &M) { postmultiply(M,GL_MODELVIEW); }
};

#endif // vgui_matrix_state_h_
