#include "vpgl_camera_homographies.h"
//:
// \file
#include <vgl/vgl_vector_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
static vgl_h_matrix_3d<double> plane_trans(vgl_plane_3d<double> const& plane,
                                           vgl_point_3d<double> const& ref_pt)
{
  // get the translation that moves the plane to intersect with the origin
  // note that calling plane.normal() pre-normalizes the vector so
  // scale factor is lost, so form normal directly
  vgl_vector_3d<double> n(plane.a(), plane.b(), plane.c());
  double mag = n.length();
  n/=mag;
  double trans = plane.d()/mag;//translate the plane to the origin
  // find the rotation needed to align the normal with the positive z axis
  vgl_vector_3d<double> z(0, 0, 1.0);
  vgl_rotation_3d<double> R(n, z);
  vgl_vector_3d<double> t = R*(trans*n);
  vgl_h_matrix_3d<double> Tr = R.as_h_matrix_3d();
  Tr.set_translation(t.x(), t.y(), t.z());
  //note the composition is [R][t], i.e. first translate then rotate
  //correct the transformation if the reference point is inverse transformed
  //to negative z
  vgl_h_matrix_3d<double> Trinv = Tr.get_inverse();
  vgl_homg_point_3d<double> hp(ref_pt);
  vgl_homg_point_3d<double> thp = Trinv(hp);
  vgl_point_3d<double> tp(thp);
  //This flip is here to standardize the reference point
  //(typically a camera center)
  //to be in the positive z half space of the x-y plane
  if (tp.z()<0) {
    vnl_matrix_fixed<double,3, 3> m(0.0);
    m[0][0] = -1.0; m[1][1] = 1.0; m[2][2]=-1.0;
    vgl_h_matrix_3d<double> Trflip;//180 degree rotation about the y axis
    Trflip.set_identity();
    Trflip.set_rotation_matrix(m);
    Tr = Trflip*Tr;
  }
  return Tr;
}

vgl_h_matrix_2d<double> vpgl_camera_homographies::
homography_to_camera(vpgl_proj_camera<double> const& cam,
                     vgl_plane_3d<double> const& plane)
{
  vgl_homg_point_3d<double> hc = cam.camera_center();
  vgl_point_3d<double> cp(hc);
  vgl_h_matrix_3d<double> Tr = plane_trans(plane,cp);
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
  auto const& pcam =
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

vpgl_perspective_camera<double> vpgl_camera_homographies::
transform_camera_to_plane(vpgl_perspective_camera<double> const& cam,
                          vgl_plane_3d<double> const& plane)
{
  vgl_homg_point_3d<double> hc = cam.camera_center();
  vgl_point_3d<double> cp(hc);
  vgl_h_matrix_3d<double> Tr = plane_trans(plane,cp);
  vgl_h_matrix_3d<double> Trinv = Tr.get_inverse();
  // postmultipy the camera by the inverse
  vpgl_perspective_camera<double> tcam =
    vpgl_perspective_camera<double>::postmultiply(cam, Trinv);
  return tcam;
}

vpgl_proj_camera<double> vpgl_camera_homographies::
transform_camera_to_plane(vpgl_proj_camera<double> const& cam,
                          vgl_plane_3d<double> const& plane)
{
  vgl_homg_point_3d<double> hc = cam.camera_center();
  vgl_point_3d<double> cp(hc);
  vgl_h_matrix_3d<double> Tr = plane_trans(plane,cp);
  vgl_h_matrix_3d<double> Trinv = Tr.get_inverse();
  // postmultipy the camera by the inverse
  vpgl_proj_camera<double> tcam = postmultiply(cam, Trinv);
  return tcam;
}

std::vector<vgl_point_3d<double> > vpgl_camera_homographies::
transform_points_to_plane(vgl_plane_3d<double> const& plane,
                          vgl_point_3d<double> const& ref_point,
                          std::vector<vgl_point_3d<double> > const& pts )
{
  std::vector<vgl_point_3d<double> > tr_pts;
  vgl_h_matrix_3d<double> Tr = plane_trans(plane, ref_point);
  for (const auto & pt : pts)
  {
    vgl_homg_point_3d<double> hp(pt);
    vgl_homg_point_3d<double> tr_hp = Tr(hp);
    vgl_point_3d<double> trp(tr_hp);
    tr_pts.push_back(trp);
  }
  return tr_pts;
}
