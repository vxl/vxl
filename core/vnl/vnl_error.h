#ifndef vnl_error_h_
#define vnl_error_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

// FIXME.

#define vnl_vector_index_error     vnl_error_vector_index
#define vnl_vector_dimension_error vnl_error_vector_dimension
#define vnl_vector_va_arg_error    vnl_error_vector_va_arg

//
#define vnl_matrix_row_index_error vnl_error_matrix_row_index
#define vnl_matrix_col_index_error vnl_error_matrix_col_index
#define vnl_matrix_dimension_error vnl_error_matrix_dimension
#define vnl_matrix_nonsquare_error vnl_error_matrix_nonsquare
#define vnl_matrix_va_arg_error    vnl_error_matrix_va_arg

//--------------------------------------------------------------------------------

//
extern void vnl_vector_index_error (const char* fcn, int index);
extern void vnl_vector_dimension_error (const char* fcn, int l1, int l2);
extern void vnl_vector_va_arg_error (int n);

//
extern void vnl_matrix_row_index_error (char const* fcn, int r);
extern void vnl_matrix_col_index_error (char const* fcn, int c);
extern void vnl_matrix_dimension_error (char const* fcn, int r1, int c1, int r2, int c2);
extern void vnl_matrix_nonsquare_error (char const* fcn);
extern void vnl_matrix_va_arg_error (int n);

#endif
