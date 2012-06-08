// This is brl/bpro/core/vpgl_pro/processes/vpgl_load_geo_camera_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <vpgl/vpgl_lvcs_sptr.h>

//: initialization
bool vpgl_load_geo_camera_process_cons(bprb_func_process& pro)
{
  //this process takes one input: local rational camera filename
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string");
  input_types.push_back("vpgl_lvcs_sptr");
  input_types.push_back("int");  // UTM zone, pass 0 if not UTM
  input_types.push_back("unsigned");  // UTM hemisphere, pass 0 for north, 1 for south
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  vcl_vector<vcl_string> output_types;
  output_types.push_back("vpgl_camera_double_sptr");  //camera output
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;
}

//: Execute the process
bool vpgl_load_geo_camera_process(bprb_func_process& pro)
{
  if (pro.n_inputs()!= 4) {
    vcl_cout << "vpgl_load_geo_camera_process: The input number should be 1" << vcl_endl;
    return false;
  }

  // get the inputs
  vcl_string tfw_filename = pro.get_input<vcl_string>(0);
  vpgl_lvcs_sptr lvcs = pro.get_input<vpgl_lvcs_sptr>(1);
  int utm_zone = pro.get_input<int>(2);
  unsigned northing = pro.get_input<unsigned>(3);

  vcl_ifstream ifs(tfw_filename.c_str());
  vnl_matrix<double> trans_matrix(4,4,0.0);
  ifs >> trans_matrix[0][0];
  ifs >> trans_matrix[0][1];
  ifs >> trans_matrix[1][0];
  ifs >> trans_matrix[1][1];
  ifs >> trans_matrix[0][3];
  ifs >> trans_matrix[1][3];
  trans_matrix[3][3] = 1.0;
  ifs.close();

  vcl_cout << "trans matrix: " << trans_matrix << vcl_endl;

  vpgl_geo_camera *cam = new vpgl_geo_camera(trans_matrix, lvcs);
  if (utm_zone != 0)
    cam->set_utm(utm_zone, northing);
  cam->set_scale_format(true);

  pro.set_output_val<vpgl_camera_double_sptr>(0, cam);
  return true;
}
