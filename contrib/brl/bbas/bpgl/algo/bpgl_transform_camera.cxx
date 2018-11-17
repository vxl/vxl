# include <bpgl/algo/bpgl_transform_camera.h>

// A similarity or Euclidean transformation for perspective camera
vpgl_perspective_camera<double>
bpgl_transform_camera::transform_perspective_camera(vpgl_perspective_camera<double> const& cam,
                                                    vgl_rotation_3d<double> const& Rs,
                                                    vnl_vector_fixed<double, 3> const& ts,
                                                    const double scale)
{
  vnl_matrix_fixed<double,3,3> Rms = Rs.as_matrix();
  //Get input camera components
  //note, the homogeneous calibration matrix is unaffected by the scale
  const vpgl_calibration_matrix<double>& K = cam.get_calibration();
  vnl_matrix_fixed<double, 3, 3> R = cam.get_rotation().as_matrix();
  vgl_vector_3d<double> tv = cam.get_translation();
  vnl_vector_fixed<double, 3> t(tv.x(), tv.y(), tv.z());
  //compose rotations
  vnl_matrix_fixed<double, 3, 3> Rt = R*Rms;
  vgl_rotation_3d<double> Rtr(Rt);
  //compute new translation
  vnl_vector_fixed<double, 3> tt = (1.0/scale)*t + R*ts;
  vgl_vector_3d<double> ttg(tt[0], tt[1], tt[2]);
  //construct transformed camera
  vpgl_perspective_camera<double> camt(K, Rtr, ttg);
  return camt;
}
