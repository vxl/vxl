// This is oxl/vgui/vgui_matrix_state.h
#ifndef vgui_matrix_state_h_
#define vgui_matrix_state_h_
//:
// \file
// \author  fsm@robots.ox.ac.uk
// \brief   Stores and retrieves the current projection and modelview matrices.
//
//  Contains classes:  vgui_matrix_state
//
// \verbatim
//  Modifications
//    AWF Renamed store, and made it save and restore on 
//        construction/desctruction.
//    FSM Renamed method names for consistency with projection_inspector.
// \endverbatim

#include <vcl_iosfwd.h>
#include <vgui/vgui_gl.h>
template <class T> class vnl_matrix;

//: Stores and retrieves the current projection and modelview matrices.
//
// vgui_matrix_state stores the current OpenGL projection and modelview
// matrices when save() is called. The matrices are restored to these values
// on the next call to restore(). Normally, the constructor calls save() and
// the destructor calls restore(), but this can be overridden.
//
// vgui_matrix_state also has various static convenience methods for GL matrix
// handling.
class vgui_matrix_state {
public:
  vgui_matrix_state(bool save_now_restore_on_destroy = true);
  ~vgui_matrix_state();

  void save();
  void restore() const;
  void print(vcl_ostream& );
  
  // set
  static void identity_gl_matrices(); // set both matrices to the identity.
  static void clear_gl_matrices();    // set both matrices to zero.
  static void zero_out_gl_matrices() { clear_gl_matrices(); }

  // query
  static bool gl_matrices_are_cleared();
  static vnl_matrix<double> projection_matrix();
  static vnl_matrix<double> modelview_matrix();
  static vnl_matrix<double> total_transformation();

  // Projection matrices
  static void premultiply(vnl_matrix<double> const &,GLenum );
  static void premultiply_projection(vnl_matrix<double> const &M) { premultiply(M,GL_PROJECTION); }
  static void premultiply_modelview (vnl_matrix<double> const &M) { premultiply(M,GL_MODELVIEW); }

  static void postmultiply(const vnl_matrix<double> &M,GLenum matrix);
  static void postmultiply_projection(vnl_matrix<double> const &M) { postmultiply(M,GL_PROJECTION); }
  static void postmultiply_modelview (vnl_matrix<double> const &M) { postmultiply(M,GL_MODELVIEW); }

private:
  // NB : matrices stored in column (fortran) order.
  double P[16]; // projection
  double M[16]; // modelview
  bool restore_on_destroy;
};

#endif // vgui_matrix_state_h_
