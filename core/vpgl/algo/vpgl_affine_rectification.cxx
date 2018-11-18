#include <iostream>
#include <limits>
#include "vpgl_affine_rectification.h"
//:
// \file


#include <vnl/algo/vnl_svd.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/algo/vnl_lsqr.h>
#include <vnl/vnl_sparse_matrix_linear_system.h>
#include <vpgl/algo/vpgl_camera_compute.h>
#include <vgl/vgl_box_3d.h>

vpgl_affine_camera<double>* vpgl_affine_rectification::compute_affine_cam(const std::vector< vgl_point_2d<double> >& image_pts,
                                                                          const std::vector< vgl_point_3d<double> >& world_pts)
{
  vpgl_affine_camera<double> aff_camera;
  vpgl_affine_camera_compute::compute(image_pts, world_pts, aff_camera);
  vgl_box_3d<double> bbox;
  for (const auto & world_pt : world_pts)
    bbox.add(world_pt);

  // use the constructor with matrix to compute the camera direction properly
  auto* out_camera = new vpgl_affine_camera<double>(aff_camera.get_matrix());
  out_camera->set_viewing_distance(10.0*bbox.height());
  return out_camera;
}


//:Extract the fundamental matrix from affine cameras
bool vpgl_affine_rectification::compute_affine_f(const vpgl_affine_camera<double>* cam1,
                                                 const vpgl_affine_camera<double>* cam2,
                                                 vpgl_affine_fundamental_matrix<double>& FA)
{
  vnl_matrix_fixed<double, 3,4> M1 = cam1->get_matrix();
  vgl_homg_point_3d<double> C = cam1->camera_center();
  vnl_vector_fixed<double,4> C1; C1[0] = C.x(); C1[1] = C.y(); C1[2] = C.z(); C1[3] = C.w();

  vnl_matrix_fixed<double, 3,4> M2 = cam2->get_matrix();

  vnl_vector_fixed<double,3> e2 = M2*C1;

  vnl_matrix_fixed<double, 3,3> e2M;
  e2M[0][0] = 0; e2M[0][1] = -e2[2]; e2M[0][2] = e2[1];
  e2M[1][0] = e2[2]; e2M[1][1] = 0; e2M[1][2] = -e2[0];
  e2M[2][0] = -e2[1]; e2M[2][1] = e2[0]; e2M[2][2] = 0;

  // find pseudo inverse of the first camera
  vnl_svd<double> temp(M1*M1.transpose()); // use svd to find inverse of M1*M1.transpose()
  vnl_matrix_fixed<double, 4,3> M1inv = M1.transpose()*temp.inverse();

  vnl_matrix_fixed<double,3,3> FAM;
  FAM = e2M*M2*M1inv;
  FA.set_matrix(FAM);

  // check for zero elements
  /*if (std::abs(FA.get_matrix()[0][0]) > 10*std::numeric_limits<double>::epsilon() ||
      std::abs(FA.get_matrix()[0][1]) > 10*std::numeric_limits<double>::epsilon() ||
      std::abs(FA.get_matrix()[1][0]) > 10*std::numeric_limits<double>::epsilon() ||
      std::abs(FA.get_matrix()[1][1]) > 10*std::numeric_limits<double>::epsilon()) {
      std::cerr << "In vpgl_affine_rectification::compute_affine_f() -- the computed matrix is not an affine fundamental matrix! the input cameras may have not been affine cameras!\n";
      return false;
  }*/

  return true;
}

//: compute the rectification homographies using the affine fundamental matrix
//  image correspondences need to be passed to find homographies
//  (if cameras are known, one can use known points in 3d in the observed scene, project them using the cameras and pass the image points to this routine)
bool vpgl_affine_rectification::compute_rectification(const vpgl_affine_fundamental_matrix<double>& FA,
                                                      const std::vector<vnl_vector_fixed<double, 3> >& img_p1,
                                                      const std::vector<vnl_vector_fixed<double, 3> >& img_p2,
                                                      vnl_matrix_fixed<double, 3, 3>& H1,
                                                      vnl_matrix_fixed<double, 3, 3>& H2)
{
  vnl_matrix_fixed<double, 3, 3> FAM = FA.get_matrix();
  vnl_vector_fixed<double, 2> e1; e1[0] = -FAM[2][1]; e1[1] = FAM[2][0]; e1[2] = 0;
  vnl_vector_fixed<double, 2> e2; e2[0] = -FAM[1][2]; e2[1] = FAM[0][2]; e2[2] = 0;

  double e1l = e1.magnitude();
  double e2l = e2.magnitude();

  H1.set_identity();
  H2.set_identity();

  // rotation of the left image so that epipolar lines become parallel
  H1[0][0] = e1[0]/e1l; H1[0][1] = e1[1]/e1l;
  H1[1][0] = -e1[1]/e1l; H1[1][1] = e1[0]/e1l;

  // rotation of the right image so that epipolar lines become parallel
  H2[0][0] = e2[0]/e2l; H2[0][1] = e2[1]/e2l;
  H2[1][0] = -e2[1]/e2l; H2[1][1] = e2[0]/e2l;

  auto m = static_cast<unsigned int>(img_p1.size());
  if (m != img_p2.size()) {
    std::cerr << " In vpgl_affine_rectification::compute_rectification() -- img_p1 and img_p2 do not have equal size!\n";
    return false;
  }

  // find a scaling and offset for the Y axis
  vnl_sparse_matrix<double> A(m,2); vnl_vector<double> b(m);
  // setup rows of A and b
  for (unsigned i = 0; i < m; i++) {
    vnl_vector_fixed<double, 3> p1rot = H1*img_p1[i];
    vnl_vector_fixed<double, 3> p2rot = H2*img_p2[i];
    A(i,0) = p2rot[1]; A(i,1) = 1;
    b[i] = p1rot[1];
  }

  vnl_sparse_matrix_linear_system<double> ls(A,b);
  vnl_vector<double> scaling(2); scaling[0]=scaling[1]=0.0;
  vnl_lsqr lsqr(ls); lsqr.minimize(scaling);
  std::cout << "scaling: " << scaling << std::endl;

  H2[0][0] = scaling[0]*H2[0][0]; H2[0][1] = scaling[0]*H2[0][1];
  H2[1][0] = scaling[0]*H2[1][0]; H2[1][1] = scaling[0]*H2[1][1];

  H2[1][2] = scaling[1];

  // find scaling, skew and offset for X
  vnl_sparse_matrix<double> AA(m,3);
  for (unsigned i = 0; i < m; i++) {
    vnl_vector_fixed<double, 3> p1rot = H1*img_p1[i];
    vnl_vector_fixed<double, 3> p2rot = H2*img_p2[i];
    AA(i,0) = p2rot[0]; AA(i,1) = p2rot[1]; AA(i,2) = p2rot[2];
    b[i] = p1rot[0];
  }
  vnl_sparse_matrix_linear_system<double> ls2(AA,b);
  vnl_vector<double> shear(3); shear[0]=shear[1]=shear[2]=0.0;
  vnl_lsqr lsqr2(ls2); lsqr2.minimize(shear);
  std::cout << "shear: " << shear << std::endl;

  vnl_matrix_fixed<double, 3, 3> interm; interm.set_identity();
  interm[0][1] = -shear[1] / 2.0;
  H1 = interm*H1;  // affine_left = interm * affine_left;

  interm.set_identity();
  interm[0][0] = shear[0];
  interm[0][1] = shear[1] / 2.0;
  interm[0][2] = shear[2];
  H2 = interm*H2; // affine_right = interm * affine_right;

  return true;
}
