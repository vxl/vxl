// This is core/vgui/vgui_matrix_state.cxx

//:
// \file
// \author  fsm
// \brief   See vgui_matrix_state.h for a description of this file.


#include <cstdlib>
#include <iostream>
#include "vgui_matrix_state.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_matlab_print.h>
#include <vgui/vgui_gl.h>

//: Construct a vgui_matrix_state, and save now.
vgui_matrix_state::vgui_matrix_state(bool save_now_restore_on_destroy)
{
  restore_on_destroy = save_now_restore_on_destroy;
  if (save_now_restore_on_destroy)
    save();
}

vgui_matrix_state::~vgui_matrix_state()
{
  if (restore_on_destroy)
    restore();
}

void vgui_matrix_state::save()
{
  glGetDoublev(GL_PROJECTION_MATRIX,P);
  glGetDoublev(GL_MODELVIEW_MATRIX, M);
}

void vgui_matrix_state::restore() const
{
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixd(P);

  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixd(M);
}


void vgui_matrix_state::print(std::ostream &s)
{
  vnl_matlab_print_format_push(vnl_matlab_print_format_long);
  vnl_matlab_print(s, projection_matrix(), "GL_PROJECTION_MATRIX");
  vnl_matlab_print(s, modelview_matrix (), "GL_MODELVIEW_MATRIX" );
  vnl_matlab_print_format_pop();
  s << std::endl;
}


void vgui_matrix_state::identity_gl_matrices()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void vgui_matrix_state::clear_gl_matrices()
{
  vnl_matrix_fixed<double,4,4> empty(0.0);

  // set projection matrix :
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixd(empty.data_block());

  // set modelview matrix :
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixd(empty.data_block());
}

bool vgui_matrix_state::gl_matrices_are_cleared()
{
  vnl_matrix_fixed<double,4,4> empty(0.0);
  vnl_matrix_fixed<double,4,4> Pt;
  vnl_matrix_fixed<double,4,4> Mt;

  glGetDoublev(GL_PROJECTION_MATRIX,Pt.data_block());
  glGetDoublev(GL_MODELVIEW_MATRIX,Mt.data_block());

  return Pt == empty && Mt == empty;
}


vnl_matrix_fixed<double,4,4> vgui_matrix_state::projection_matrix()
{
  vnl_matrix_fixed<double,4,4> P;
  glGetDoublev(GL_PROJECTION_MATRIX,P.data_block());
  P.inplace_transpose();

  return P;
}

vnl_matrix_fixed<double,4,4> vgui_matrix_state::modelview_matrix()
{
  vnl_matrix_fixed<double,4,4> M;
  glGetDoublev(GL_MODELVIEW_MATRIX,M.data_block());
  M.inplace_transpose();

  return M;
}

vnl_matrix_fixed<double,4,4> vgui_matrix_state::total_transformation()
{
  return projection_matrix()*modelview_matrix();
}

//: this premultiplies the given matrix by M
void vgui_matrix_state::premultiply(vnl_matrix_fixed<double,4,4> const& M,GLenum matrix)
{
  // get current (transposed) projection matrix :
  vnl_matrix_fixed<double,4,4> P;
  if      (matrix==GL_PROJECTION || matrix==GL_PROJECTION_MATRIX)
    glGetDoublev(GL_PROJECTION_MATRIX,P.data_block());
  else if (matrix==GL_MODELVIEW  || matrix==GL_MODELVIEW_MATRIX )
    glGetDoublev(GL_MODELVIEW_MATRIX,P.data_block());
  else
    std::abort();
  P.inplace_transpose();

  // set the matrix :
  if      (matrix==GL_PROJECTION || matrix==GL_PROJECTION_MATRIX)
    glMatrixMode(GL_PROJECTION);
  else if (matrix==GL_MODELVIEW  || matrix==GL_MODELVIEW_MATRIX )
    glMatrixMode(GL_MODELVIEW);
  glLoadMatrixd( (M*P).transpose().data_block() );
}


//: This postmultiplies the given matrix by M
void vgui_matrix_state::postmultiply(vnl_matrix_fixed<double,4,4> const& M,GLenum matrix)
{
  // set matrix mode :
  if      (matrix==GL_PROJECTION || matrix==GL_PROJECTION_MATRIX)
    glMatrixMode(GL_PROJECTION);
  else if (matrix==GL_MODELVIEW  || matrix==GL_MODELVIEW_MATRIX )
    glMatrixMode(GL_MODELVIEW);

  glMultMatrixd( M.transpose().data_block() );
}
