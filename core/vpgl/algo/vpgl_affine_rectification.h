// This is core/vpgl/algo/vpgl_affine_rectification.h
#ifndef vpgl_affine_rectification_h_
#define vpgl_affine_rectification_h_
//:
// \file
// \brief Methods for computing an affine fundamental matrix, FA using a pair of affine cameras and using FA to find homographies for rectification
// \author Ozge C. Ozcanli
// \date Nov 27, 2013

#include <vpgl/vpgl_affine_camera.h>
#include <vpgl/vpgl_affine_fundamental_matrix.h>
#include <vpgl/vpgl_proj_camera.h>

class vpgl_affine_rectification
{
 public:
  static vpgl_affine_camera<double>* compute_affine_cam(const std::vector< vgl_point_2d<double> >& image_pts,
                                                        const std::vector< vgl_point_3d<double> >& world_pts);

  //:extract the fundamental matrix from a pair of affine cameras
  static bool compute_affine_f(const vpgl_affine_camera<double>* cam1,
                               const vpgl_affine_camera<double>* cam2,
                               vpgl_affine_fundamental_matrix<double>& FA);

  //: compute the rectification homographies using the affine fundamental matrix
  //  an image correspondence needs to be passed to find homographies
  //  (if cameras are known, one can use a known point in 3d in the scene, project it using the cameras and pass the output image points to this routine)
  static bool compute_rectification(const vpgl_affine_fundamental_matrix<double>& FA,
                                    const std::vector<vnl_vector_fixed<double, 3> >& img_p1,
                                    const std::vector<vnl_vector_fixed<double, 3> >& img_p2,
                                    vnl_matrix_fixed<double, 3, 3>& H1,
                                    vnl_matrix_fixed<double, 3, 3>& H2);

  vpgl_affine_rectification() = delete;
  ~vpgl_affine_rectification() = delete;
};

#endif // vpgl_affine_rectification_h_
