// This is brl/bpro/core/vpgl_pro/processes/vpgl_compute_persp_cam_pa_covariance_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
//   Compute the 3x3 covariance matrix for the principle axis direction
//   assuming that the axis direction is normally distributed in a cone
//   around the given perspective camera, the extent of the cone is std dev of rotation angle theta
//   applies error propagation to the geometric equation for perturbing the axis, see
//   \a vpgl_perturb_persp_cam_orient_process
//

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>

//: initialization
bool vpgl_compute_persp_cam_pa_covariance_process_cons(bprb_func_process& pro)
{
  //input[0]: the camera
  //input[1]: the std dev of theta
  //input[2]: output file name to write 3x3 covariance matrix
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vpgl_camera_double_sptr");
  input_types.push_back("float");  // passed to the process in degrees, will be converted to radians
  input_types.push_back("vcl_string");
  return pro.set_input_types(input_types);
}

//: Execute the process
bool vpgl_compute_persp_cam_pa_covariance_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 3) {
    vcl_cout << "vpgl_compute_persp_cam_pa_covariance_process: The number of inputs should be 3" << vcl_endl;
    return false;
  }

  // get the inputs
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(0);
  double std_dev = pro.get_input<float>(1)/180.0*vnl_math::pi;
  vcl_string filename = pro.get_input<vcl_string>(2);
  vpgl_perspective_camera<double> *cam = dynamic_cast<vpgl_perspective_camera<double>*>(camera.as_pointer());

  if (!cam) {
    vcl_cerr << "error: could not convert camera input to a vpgl_perspective_camera\n";
    return false;
  }
  vnl_matrix_fixed<double, 3, 3> R = cam->get_rotation().as_matrix();
  double r1 = R[0][0], r4 = R[1][0], r7 = R[2][0];
  double var = std_dev*std_dev;
  vnl_matrix_fixed<double, 3, 3> cov;
  cov[0][0] = r1*r1*var; cov[0][1] = r1*r4*var; cov[0][2] = r1*r7*var;
  cov[1][0] = cov[0][1]; cov[1][1] = r4*r4*var; cov[1][2] = r4*r7*var;
  cov[2][0] = cov[0][2]; cov[2][1] = cov[1][2]; cov[2][2] = r7*r7*var;

  // write matrices to the text file.
  vcl_ofstream ofs(filename.c_str());
  if (!ofs.is_open()) {
    vcl_cerr << "Failed to open file " << filename << '\n';
    return false;
  }
  ofs << cov;
  ofs.close();

  return true;
}

