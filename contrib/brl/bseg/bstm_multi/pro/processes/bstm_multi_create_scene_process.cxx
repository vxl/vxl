//:
// \file bstm_multi_create_scene_process.cxx
// \brief  Processes for creating and saving bstm_multi scene files.
//
// \author Raphael Kargon
// \date 04 Aug 2017

#include <vcl_fstream.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vul/vul_file.h>

#include <bprb/bprb_func_process.h>
#include <bstm_multi/bstm_multi_block.h>
#include <bstm_multi/bstm_multi_typedefs.h>

namespace bstm_multi_create_scene_process_globals {
constexpr unsigned n_inputs_ = 6;
constexpr unsigned n_outputs_ = 1;
}

bool bstm_multi_create_scene_process_cons(bprb_func_process &pro) {
  using namespace bstm_multi_create_scene_process_globals;

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string"; // datapath
  input_types_[1] = "vcl_string"; // appearance model
  input_types_[2] = "vcl_string"; // occupancy model
  input_types_[3] = "float";      // origin x
  input_types_[4] = "float";      // origin y
  input_types_[5] = "float";      // origin z

  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "bstm_multi_scene_sptr";

  return pro.set_input_types(input_types_) &&
         pro.set_output_types(output_types_);
}

bool bstm_multi_create_scene_process(bprb_func_process &pro) {
  using namespace bstm_multi_create_scene_process_globals;

  if (pro.n_inputs() < n_inputs_) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_
             << vcl_endl;
    return false;
  }
  // get the inputs
  vcl_vector<vcl_string> appearance(1, "");
  unsigned i = 0;
  vcl_string datapath = pro.get_input<vcl_string>(i++);
  appearance[0] = pro.get_input<vcl_string>(i++); // Appearance Model String
  vcl_string opt_app = pro.get_input<vcl_string>(i++); // Occupancy Model String
  float origin_x = pro.get_input<float>(i++);
  float origin_y = pro.get_input<float>(i++);
  float origin_z = pro.get_input<float>(i++);

  if (!vul_file::make_directory_path(datapath.c_str()))
    return false;
  bstm_multi_scene_sptr scene = new bstm_multi_scene(
      datapath, vgl_point_3d<double>(origin_x, origin_y, origin_z), 2);
  scene->set_local_origin(vgl_point_3d<double>(origin_x, origin_y, origin_z));

  if (opt_app.length() > 0)
    appearance.push_back(opt_app);
  scene->set_appearances(appearance);

  i = 0; // store scene smart pointer
  pro.set_output_val<bstm_multi_scene_sptr>(i++, scene);
  return true;
}

namespace bstm_multi_write_scene_xml_process_globals {
constexpr unsigned n_inputs_ = 2;
constexpr unsigned n_outputs_ = 0;
}

bool bstm_multi_write_scene_xml_process_cons(bprb_func_process &pro) {
  using namespace bstm_multi_write_scene_xml_process_globals;

  // process takes 2 inputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bstm_multi_scene_sptr";
  input_types_[1] = "vcl_string";

  // process has 1 output
  vcl_vector<vcl_string> output_types_(n_outputs_);
  bool ok =
      pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  // default arguments - default filename is "scene"
  brdb_value_sptr filename = new brdb_value_t<vcl_string>("scene");
  pro.set_input(1, filename);
  return ok;
}

bool bstm_multi_write_scene_xml_process(bprb_func_process &pro) {
  using namespace bstm_multi_write_scene_xml_process_globals;

  if (pro.n_inputs() < n_inputs_) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_
             << vcl_endl;
    return false;
  }
  // get the inputs
  unsigned i = 0;
  bstm_multi_scene_sptr scene = pro.get_input<bstm_multi_scene_sptr>(i++);
  vcl_string filename = pro.get_input<vcl_string>(i++);

  // set xml path to reflect fname
  vcl_string xmlPath = scene->xml_path();
  vcl_string xmlDir = vul_file::dirname(xmlPath);
  scene->set_xml_path(xmlDir + "/" + filename + ".xml");

  // make file and x_write to file
  vcl_ofstream ofile(scene->xml_path().c_str());
  x_write(ofile, (*scene.ptr()), "scene");
  return true;
}
