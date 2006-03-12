// This is gel/mrc/vpgl/algo/vpgl_list.h
#ifndef vpgl_list_h_
#define vpgl_list_h_
//:
// \file
// \brief Write lists of cameras, homographies, fundamental matrices in a standard format
//
// Template classes for writing lists of cameras, homographies, fundamental
// matrices, etc in a standard format.  This will eventually be replaced by XML
// read/writers.
// \author Thomas Pollard
// \date 03/05/05
//
// \verbatim
//
// \endverbatim

#include <vcl_iosfwd.h>
#include <vcl_cassert.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_fundamental_matrix.h>
#include <vgl/algo/vgl_h_matrix_3d.h>


template <class T>
bool vpgl_read_list(
  vcl_vector<T>& list,
  vcl_string file );

template <class T>
bool vpgl_write_list(
  const vcl_vector<T>& list,
  vcl_string file );


// vpgl_proj_camera<double>
template bool vpgl_read_list(
  vcl_vector< vpgl_proj_camera<double> >&, vcl_string file );
template bool vpgl_write_list(
  const vcl_vector< vpgl_proj_camera<double> >&, vcl_string file );

// vgl_h_matrix_2d<double>
template bool vpgl_read_list(
  vcl_vector< vgl_h_matrix_2d<double> >&, vcl_string file );
template bool vpgl_write_list(
  const vcl_vector< vgl_h_matrix_2d<double> >&, vcl_string file );

#if 0
// vpgl_fundamental_matrix<double> doesn't have io yet
template bool vpgl_read_list(
  vcl_vector< vpgl_fundamental_matrix<double> >&, vcl_string file );
template bool vpgl_write_list(
  const vcl_vector< vpgl_fundamental_matrix<double> >&, vcl_string file );
#endif // 0

#endif // vpgl_list_h_
