//This is brl/bbas/bvrml/pro/processes/bvrml_write_processes.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
//
#include <bprb/bprb_parameters.h>

#include <vcl_string.h>
#ifdef DEBUG
#include <vcl_iostream.h>
#endif

#include <brdb/brdb_value.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_sphere_3d.h>
#include <bvrml/bvrml_write.h>

#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>

//: sets input and output types
bool bvrml_write_box_process_cons(bprb_func_process& pro)
{
  //inputs
  vcl_vector<vcl_string> input_types_(12);
  input_types_[0] = "vcl_string";
  input_types_[1] = "double";
  input_types_[2] = "double";
  input_types_[3] = "double";
  input_types_[4] = "double";
  input_types_[5] = "double";
  input_types_[6] = "double";
  input_types_[7] = "bool";  // 0 if solid box, 1 if wire box
  input_types_[8] = "float";
  input_types_[9] = "float";
  input_types_[10] = "float";
  input_types_[11] = "float";

  //output
  vcl_vector<vcl_string> output_types_(0);

  bool good = pro.set_input_types(input_types_) &&
              pro.set_output_types(output_types_);

  brdb_value_sptr idxw = new brdb_value_t<bool>(1);
  pro.set_input(7, idxw);
  brdb_value_sptr idxr = new brdb_value_t<float>(1.0f);
  pro.set_input(8, idxr);
  brdb_value_sptr idxg = new brdb_value_t<float>(1.0f);
  pro.set_input(9, idxg);
  brdb_value_sptr idxb = new brdb_value_t<float>(1.0f);
  pro.set_input(10, idxb);
  brdb_value_sptr idxt = new brdb_value_t<float>(0.0f);
  pro.set_input(11, idxt);

  return good;
}

bool bvrml_write_box_process(bprb_func_process& pro)
{
  // check number of inputs
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << ": Invalid inputs" << vcl_endl;
    return false;
  }
    //get the inputs
  vcl_string fname = pro.get_input<vcl_string>(0);
  double xmin = pro.get_input<double>(1);
  double ymin = pro.get_input<double>(2);
  double zmin = pro.get_input<double>(3);
  double xmax = pro.get_input<double>(4);
  double ymax = pro.get_input<double>(5);
  double zmax = pro.get_input<double>(6);
  bool wire = pro.get_input<bool>(7);
  float r = pro.get_input<float>(8);
  float g = pro.get_input<float>(9);
  float b = pro.get_input<float>(10);
  float transparency = pro.get_input<float>(11);

  vcl_ofstream ofs(fname.c_str(), vcl_ios::app);
  vgl_box_3d<double> box(xmin, ymin, zmin, xmax, ymax, zmax);
  if (wire)
    bvrml_write::write_vrml_wireframe_box(ofs, box, r, g, b, transparency);
  else
    bvrml_write::write_vrml_box(ofs, box, r, g, b, transparency);
  ofs.close();

  return true;
}


//: sets input and output types
bool bvrml_write_perspective_cam_process_cons(bprb_func_process& pro)
{
  //inputs
  vcl_vector<vcl_string> input_types_(7);
  input_types_[0] = "vcl_string";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "float";  // radius of the camera center sphere
  input_types_[3] = "float";  // length of principle axis
  input_types_[4] = "float";  // red in [0,1]
  input_types_[5] = "float";  // green
  input_types_[6] = "float";  // blue

  //output
  vcl_vector<vcl_string> output_types_(0);

  bool good = pro.set_input_types(input_types_) &&
              pro.set_output_types(output_types_);
  return good;
}

bool bvrml_write_perspective_cam_process(bprb_func_process& pro)
{
  // check number of inputs
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << ": Invalid inputs" << vcl_endl;
    return false;
  }
    //get the inputs
  vcl_string fname = pro.get_input<vcl_string>(0);
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(1);
  float rad = pro.get_input<float>(2);
  float axis_len = pro.get_input<float>(3);
  float red = pro.get_input<float>(4);
  float green = pro.get_input<float>(5);
  float blue = pro.get_input<float>(6);

  vpgl_perspective_camera<double> *cam = dynamic_cast<vpgl_perspective_camera<double>*>(camera.as_pointer());
  if (!cam) {
    vcl_cerr << "error: could not convert camera input to a vpgl_perspective_camera\n";
    return false;
  }

  vcl_ofstream ofs(fname.c_str(), vcl_ios::app);

  //: get cam center
  vgl_point_3d<double> cent = cam->get_camera_center();
  vcl_cout << "cent: " << cent << vcl_endl;
  vgl_vector_3d<double> axis = cam->principal_axis();

  vgl_sphere_3d<float> sp((float)cent.x(), (float)cent.y(), (float)cent.z(), rad);
  bvrml_write::write_vrml_sphere(ofs, sp, 1.0, 0.0, 0.0, 0.0f);
  bvrml_write::write_vrml_line(ofs, cent, axis, axis_len, red, green, blue);
  ofs.close();

  return true;
}


//: sets input and output types
bool bvrml_write_origin_and_axes_process_cons(bprb_func_process& pro)
{
  //inputs
  vcl_vector<vcl_string> input_types_(2);
  input_types_[0] = "vcl_string";
  input_types_[1] = "float";  // length of the axis lines


  //output
  vcl_vector<vcl_string> output_types_(0);

  bool good = pro.set_input_types(input_types_) &&
              pro.set_output_types(output_types_);
  return good;
}

bool bvrml_write_origin_and_axes_process(bprb_func_process& pro)
{
  // check number of inputs
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << ": Invalid inputs" << vcl_endl;
    return false;
  }
    //get the inputs
  vcl_string fname = pro.get_input<vcl_string>(0);
  float len = pro.get_input<float>(1);

  vcl_ofstream ofs(fname.c_str(), vcl_ios::app);

  //: get cam center
  vgl_point_3d<double> cent(0.0, 0.0, 0.0);
  vgl_vector_3d<double> axis_x(1.0, 0.0, 0.0);
  vgl_vector_3d<double> axis_y(0.0, 1.0, 0.0);
  vgl_vector_3d<double> axis_z(0.0, 0.0, 1.0);
  vgl_sphere_3d<float> sp((float)cent.x(), (float)cent.y(), (float)cent.z(), len/10);
  bvrml_write::write_vrml_sphere(ofs, sp, 1.0f, 0.0f, 0.0f, 0.0f);
  bvrml_write::write_vrml_line(ofs, cent, axis_x, len, 1.0f, 0.0f, 0.0f);
  bvrml_write::write_vrml_line(ofs, cent, axis_y, len, 0.0f, 1.0f, 0.0f);
  bvrml_write::write_vrml_line(ofs, cent, axis_z, len, 0.0f, 0.0f, 1.0f);
  ofs.close();

  return true;
}

//: sets input and output types
bool bvrml_write_point_process_cons(bprb_func_process& pro)
{
  //inputs
  vcl_vector<vcl_string> input_types_(8);
  input_types_[0] = "vcl_string";
  input_types_[1] = "float";  // x
  input_types_[2] = "float";  // y
  input_types_[3] = "float";  // z
  input_types_[4] = "float";  // radius of the sphere
  input_types_[5] = "float";  // red in [0,1]
  input_types_[6] = "float";  // green
  input_types_[7] = "float";  // blue

  //output
  vcl_vector<vcl_string> output_types_(0);

  bool good = pro.set_input_types(input_types_) &&
              pro.set_output_types(output_types_);
  return good;
}

bool bvrml_write_point_process(bprb_func_process& pro)
{
  // check number of inputs
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << ": Invalid inputs" << vcl_endl;
    return false;
  }

  //get the inputs
  vcl_string fname = pro.get_input<vcl_string>(0);
  float x = pro.get_input<float>(1);
  float y = pro.get_input<float>(2);
  float z = pro.get_input<float>(3);
  float rad = pro.get_input<float>(4);
  float red = pro.get_input<float>(5);
  float green = pro.get_input<float>(6);
  float blue = pro.get_input<float>(7);

  vcl_ofstream ofs(fname.c_str(), vcl_ios::app);

  //: get cam center
  vgl_point_3d<double> cent(x,y,z);
  vgl_sphere_3d<float> sp((float)cent.x(), (float)cent.y(), (float)cent.z(), rad);
  bvrml_write::write_vrml_sphere(ofs, sp, red, green, blue, 0.0f);
  ofs.close();

  return true;
}

//: sets input and output types
bool bvrml_write_line_process_cons(bprb_func_process& pro)
{
  //inputs
  vcl_vector<vcl_string> input_types_(10);
  input_types_[0] = "vcl_string";
  input_types_[1] = "float";  // x1
  input_types_[2] = "float";  // y1
  input_types_[3] = "float";  // z1
  input_types_[4] = "float";  // x2
  input_types_[5] = "float";  // y2
  input_types_[6] = "float";  // z2
  input_types_[7] = "float";  // red in [0,1]
  input_types_[8] = "float";  // green
  input_types_[9] = "float";  // blue

  //output
  vcl_vector<vcl_string> output_types_(0);

  bool good = pro.set_input_types(input_types_) &&
              pro.set_output_types(output_types_);
  return good;
}

bool bvrml_write_line_process(bprb_func_process& pro)
{
  // check number of inputs
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << ": Invalid inputs" << vcl_endl;
    return false;
  }

  //get the inputs
  vcl_string fname = pro.get_input<vcl_string>(0);
  float x1 = pro.get_input<float>(1);
  float y1 = pro.get_input<float>(2);
  float z1 = pro.get_input<float>(3);
  float x2 = pro.get_input<float>(4);
  float y2 = pro.get_input<float>(5);
  float z2 = pro.get_input<float>(6);
  float red = pro.get_input<float>(7);
  float green = pro.get_input<float>(8);
  float blue = pro.get_input<float>(9);

  vcl_ofstream ofs(fname.c_str(), vcl_ios::app);

  vgl_point_3d<double> p1(x1, y1, z1);
  vgl_point_3d<double> p2(x2, y2, z2);
  vgl_line_segment_3d<double> line(p1, p2);
  bvrml_write::write_vrml_line_segment(ofs, line, red, green, blue, 0.0f);
  ofs.close();

  return true;
}

