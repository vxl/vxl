//: \file
// \brief Convert a 3D rotation between various representations e.g. quaternion and Euler angles


#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_arg.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <mbl/mbl_log.h>


int main(int argc, char* argv[])
{
  mbl_logger logger("mul.tools.convert_rotation_3d");
  mbl_logger::root().load_log_config_file();

  vul_arg_base::set_help_description(
    "DESCRIPTION:\n"
    "Convert a 3D rotation between various representations e.g. quaternion, 3x3 matrix and Euler angles.\n"
    "Essentially, this program provides a tool-level interface to vgl_rotation_3d<double>.\n"
    "Output is written to std out as a vector<double>.\n"
  );

  vul_arg<std::vector<double> > quat("-q", "Input quaternion: x, y, z, w.  NB The order of elements follows vnl_quaternion.");
  vul_arg<std::vector<double> > eul("-e", "Input euler angles: rx, ry, rz");
  vul_arg<std::vector<double> > mat("-m", "Input 3x3 rotation matrix: row0, row1, row2");
  vul_arg<std::string> out_rep("-out", "Output representation e.g. \"q\" (quaternion), \"m\" (matrix) or \"e\" (euler)");
  vul_arg_parse(argc, argv);

  // Log the input arguments
  if (quat.set()) MBL_LOG(INFO, logger, "quat: " << quat()[0] << ',' << quat()[1] << ',' << quat()[2] << ',' << quat()[3]);
  if (mat.set()) MBL_LOG(INFO, logger, "mat: " << mat()[0] << ',' << mat()[1] << ',' << mat()[2]);
  if (eul.set()) MBL_LOG(INFO, logger, "eul: " << eul()[0] << ',' << eul()[1] << ',' << eul()[2]);
  if (out_rep.set()) MBL_LOG(INFO, logger, "out_rep: " << out_rep());

  // Check consistency of parsed arguments
  // Only 1 rotation argument permitted
  unsigned nrot=0;
  if (quat.set()) nrot++;
  if (mat.set()) nrot++;
  if (eul.set()) nrot++;
  if (nrot>1)
  {
    std::cerr << "ERROR: Please provide input as a rotation matrix OR a quaternion OR euler angles.\n";
    return 1;
  }

  if ( quat.set() && quat().size() != 4 )
  {
    std::cerr << "ERROR: Invalid quaternion. Please provide a 4-vector.\n";
    return 1;
  }

  if ( mat.set() && mat().size() != 9 )
  {
    std::cerr << "ERROR: Invalid rotation matrix. Please provide a 9-vector.\n";
    return 1;
  }

  if ( eul.set() && eul().size() != 3 )
  {
    std::cerr << "ERROR: Invalid euler. Please provide a 3-vector.\n";
    return 1;
  }

  if (!out_rep.set())
  {
    std::cerr << "ERROR: output representation not specified\n";
    return 2;
  }

  // Create rotation class with provided input data
  vgl_rotation_3d<double> rot;
  if (eul.set())
  {
    // Euler angle input
    MBL_LOG(DEBUG, logger, "Using Euler angle input format");
    rot = vgl_rotation_3d<double>(eul()[0], eul()[1], eul()[2]);
  }
  else if (quat.set())
  {
    // Quaternion input
    MBL_LOG(DEBUG, logger, "Using quaternion input format");
    vnl_quaternion<double> q(quat()[0], quat()[1], quat()[2], quat()[3]);
    rot = vgl_rotation_3d<double>(q);
  }
  else if (mat.set())
  {
    // Matrix input
    MBL_LOG(DEBUG, logger, "Using matrix input format");
    vnl_matrix_fixed<double,3,3> m;
    m[0][0] = mat()[0]; m[0][1] = mat()[1]; m[0][2] = mat()[2]; // row0
    m[1][0] = mat()[3]; m[1][1] = mat()[4]; m[1][2] = mat()[5]; // row1
    m[2][0] = mat()[6]; m[2][1] = mat()[7]; m[2][2] = mat()[8]; // row2
    rot = vgl_rotation_3d<double>(m);
  }
  else
  {
    std::cerr << "ERROR: no input data specified\n";
    return 3;
  }

  // Perform requested conversion and write to std out
  if (out_rep()=="q")
  {
    vnl_quaternion<double> q = rot.as_quaternion();
    MBL_LOG(DEBUG, logger, "Converted to quaternion: "
            << q[0] << ' ' << q[1] << ' ' << q[2] << ' ' << q[3]);
    std::cout << q[0] << ' ' << q[1] << ' ' << q[2] << ' ' << q[3] << std::endl;
  }
  else if (out_rep()=="e")
  {
    vnl_vector_fixed<double,3> e = rot.as_euler_angles();
    MBL_LOG(DEBUG, logger, "Converted to euler: "
            << e[0] << ' ' << e[1] << ' ' << e[2]);
    std::cout << e[0] << ' ' << e[1] << ' ' << e[2] << std::endl;
  }
  else if (out_rep()=="m")
  {
    // Rotation matrix output
    vnl_matrix_fixed<double,3,3> m = rot.as_matrix();
    MBL_LOG(DEBUG, logger, "Converted to matrix: " << m);
    std::cout << m << std::endl;
  }
  else
  {
    std::cerr << "ERROR: invalid output representation\n";
    return 4;
  }

  return 0;
}
