#include "vpgl_camera_homographies.h"
//:
// \file
#include <vgl/vgl_vector_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>

vgl_h_matrix_2d<double> vpgl_camera_homographies::
homography_to_camera(vpgl_proj_camera<double> const& cam,
                     vgl_plane_3d<double> const& plane)
{
  // get the translation that moves the plane to intersect with the origin
  vgl_vector_3d<double> n = plane.normal();
  double mag = n.length();
  double trans = plane.d()/(mag*mag);//translate the plane to the origin
  // find the rotation needed to align the normal with the positive z axis
  vgl_vector_3d<double> z(0, 0, 1.0);
  vgl_rotation_3d<double> R(n, z);
  vgl_vector_3d<double> t = R*(trans*n);
  vgl_h_matrix_3d<double> Tr = R.as_h_matrix_3d();
  Tr.set_translation(t.x(), t.y(), t.z());
  //note the composition is [R][t], i.e. first translate then rotate
  // invert to transform the camera
  vgl_h_matrix_3d<double> Trinv = Tr.get_inverse();

  // postmultipy the camera by the inverse
  vpgl_proj_camera<double> tcam = postmultiply(cam, Trinv);
  // extract the homography (columns 0, 1, and 3)
  vnl_matrix_fixed<double, 3, 4> p = tcam.get_matrix();
  vnl_matrix_fixed<double, 3, 3> h;
  vnl_vector_fixed<double, 3> h0, h1, h2; //columns of h
  h0 = p.get_column(0);  h1 = p.get_column(1); h2 = p.get_column(3);
  h.set_column(0, h0);   h.set_column(1, h1); h.set_column(2, h2);
  vgl_h_matrix_2d<double> H(h);
  return H;
}

//: create a plane projective transformation from the camera image plane to the specified plane
vgl_h_matrix_2d<double> vpgl_camera_homographies::
homography_to_camera(vpgl_perspective_camera<double> const& cam,
                     vgl_plane_3d<double> const& plane)
{
  vpgl_proj_camera<double> const& pcam =
    static_cast<vpgl_proj_camera<double> const&>(cam);
  return vpgl_camera_homographies::homography_to_camera(pcam, plane);
}

//: create a plane projective transformation from the camera image plane to the specified plane
vgl_h_matrix_2d<double> vpgl_camera_homographies::
homography_from_camera(vpgl_proj_camera<double> const& cam,
                       vgl_plane_3d<double> const& plane)
{
  vgl_h_matrix_2d<double> H =
    vpgl_camera_homographies::homography_to_camera(cam, plane);
  return H.get_inverse();
}

//: create a plane projective transformation from the camera image plane to the specified plane
vgl_h_matrix_2d<double> vpgl_camera_homographies::
homography_from_camera(vpgl_perspective_camera<double> const& cam,
                       vgl_plane_3d<double> const& plane)
{
  vgl_h_matrix_2d<double> H =
    vpgl_camera_homographies::homography_to_camera(cam, plane);
  return H.get_inverse();
}
