// This is oxl/vgui/vgui_matrix_state.cxx

//:
// \file
// \author  fsm@robots.ox.ac.uk
// \brief   See vgui_matrix_state.h for a description of this file.


#include "vgui_matrix_state.h"
#include <vcl_cstdlib.h> // vcl_abort()
#include <vcl_iostream.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matlab_print.h>
#include <vgui/vgui_gl.h>

//: Construct a vgui_matrix_state, and save now.
vgui_matrix_state::vgui_matrix_state(bool save_now_restore_on_destroy) {
  restore_on_destroy = save_now_restore_on_destroy;
  if (save_now_restore_on_destroy)
    save();
}

vgui_matrix_state::~vgui_matrix_state() {
  if (restore_on_destroy)
    restore();
}

void vgui_matrix_state::save() {
  glGetDoublev(GL_PROJECTION_MATRIX,P);
  glGetDoublev(GL_MODELVIEW_MATRIX, M);
}

void vgui_matrix_state::restore() const {
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixd(P);

  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixd(M);
}


void vgui_matrix_state::print(vcl_ostream &s) {
  vnl_matlab_print_format_push(vnl_matlab_print_format_long);
  vnl_matlab_print(s, projection_matrix(), "GL_PROJECTION_MATRIX");
  vnl_matlab_print(s, modelview_matrix (), "GL_MODELVIEW_MATRIX" );
  vnl_matlab_print_format_pop();
  s << vcl_endl;
}


void vgui_matrix_state::identity_gl_matrices() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void vgui_matrix_state::clear_gl_matrices() {

  vnl_matrix<double> empty(4,4,0);

  // set projection matrix :
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixd(empty.data_block());

  // set modelview matrix :
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixd(empty.data_block());
}

bool vgui_matrix_state::gl_matrices_are_cleared() {
  vnl_matrix<double> empty(4,4,0);
  vnl_matrix<double> Pt(4,4);
  vnl_matrix<double> Mt(4,4);

  glGetDoublev(GL_PROJECTION_MATRIX,Pt.data_block());
  glGetDoublev(GL_MODELVIEW_MATRIX,Mt.data_block());

  if (Pt == empty && Mt == empty)
    return true;

  return false;
}


vnl_matrix<double> vgui_matrix_state::projection_matrix() {
  vnl_matrix<double> P(4,4);
  glGetDoublev(GL_PROJECTION_MATRIX,P.data_block());
  P.inplace_transpose();

  return P;
}

vnl_matrix<double> vgui_matrix_state::modelview_matrix() {

  vnl_matrix<double> M(4,4);
  glGetDoublev(GL_MODELVIEW_MATRIX,M.data_block());
  M.inplace_transpose();

  return M;
}

vnl_matrix<double> vgui_matrix_state::total_transformation() {
  return projection_matrix()*modelview_matrix();
}

//: this premultiplies the given matrix by M
void vgui_matrix_state::premultiply(vnl_matrix<double> const &M,GLenum matrix) {
  M.assert_size(4,4);

  // get current (transposed) projection matrix :
  vnl_matrix<double> P(4,4);
  if      (matrix==GL_PROJECTION || matrix==GL_PROJECTION_MATRIX)
    glGetDoublev(GL_PROJECTION_MATRIX,P.data_block());
  else if (matrix==GL_MODELVIEW  || matrix==GL_MODELVIEW_MATRIX )
    glGetDoublev(GL_MODELVIEW_MATRIX,P.data_block());
  else
    vcl_abort();
  P.inplace_transpose();

  // set the matrix :
  if      (matrix==GL_PROJECTION || matrix==GL_PROJECTION_MATRIX)
    glMatrixMode(GL_PROJECTION);
  else if (matrix==GL_MODELVIEW  || matrix==GL_MODELVIEW_MATRIX )
    glMatrixMode(GL_MODELVIEW);
  glLoadMatrixd( (M*P).transpose().data_block() );
}


//: This postmultiplies the given matrix by M
void vgui_matrix_state::postmultiply(const vnl_matrix<double> &M,GLenum matrix) {
  M.assert_size(4,4);

  // set matrix mode :
  if      (matrix==GL_PROJECTION || matrix==GL_PROJECTION_MATRIX)
    glMatrixMode(GL_PROJECTION);
  else if (matrix==GL_MODELVIEW  || matrix==GL_MODELVIEW_MATRIX )
    glMatrixMode(GL_MODELVIEW);

  glMultMatrixd( M.transpose().data_block() );
}
