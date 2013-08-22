// This is brl/bseg/boxm2/pro/processes/boxm2_scene_kml_process.cxx
//:
// \file
// \brief  A process for writing scene blocks into a kml file
//
// \author Ozge C. Ozcanli
// \date July 28, 2012

#include <bprb/bprb_func_process.h>

#include <boxm2/boxm2_scene.h>
#include <bkml/bkml_write.h>
#include <vpgl/vpgl_lvcs.h>
#include <vgl/vgl_box_3d.h>

namespace boxm2_scene_kml_process_globals
{
  const unsigned n_inputs_ = 2;
  const unsigned n_outputs_ = 0;
}
bool boxm2_scene_kml_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_scene_kml_process_globals;
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "vcl_string";

  vcl_vector<vcl_string>  output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_scene_kml_process(bprb_func_process& pro)
{
  using namespace boxm2_scene_kml_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  
  //get the inputs
  boxm2_scene_sptr   scene = pro.get_input<boxm2_scene_sptr>(0);
  vpgl_lvcs lvcs = scene->lvcs();

  vcl_string kml_name = pro.get_input<vcl_string>(1);
  vcl_ofstream ofs(kml_name.c_str());
  bkml_write::open_document(ofs);
  vcl_map<boxm2_block_id, boxm2_block_metadata> blks = scene->blocks();
  
  for (vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator iter = blks.begin(); iter != blks.end(); iter++) {
    
    vgl_box_3d<double> box = iter->second.bbox();

    double lon, lat, elev;
    lvcs.local_to_global(box.min_x(), box.min_y(), box.min_z(), vpgl_lvcs::wgs84, lon, lat, elev);
    vnl_double_2 ll; ll[0] = lat; ll[1] = lon;
    
    lvcs.local_to_global(box.max_x(), box.min_y(), box.min_z(), vpgl_lvcs::wgs84, lon, lat, elev);
    vnl_double_2 lr; lr[0] = lat; lr[1] = lon;
    
    lvcs.local_to_global(box.max_x(), box.max_y(), box.min_z(), vpgl_lvcs::wgs84, lon, lat, elev);
    vnl_double_2 ur; ur[0] = lat; ur[1] = lon;
    
    lvcs.local_to_global(box.min_x(), box.max_y(), box.min_z(), vpgl_lvcs::wgs84, lon, lat, elev);
    vnl_double_2 ul; ul[0] = lat; ul[1] = lon;
    
    vcl_stringstream box_id;
    box_id << iter->first.to_string() << ", max_level: " << iter->second.max_level_ 
           << ", dim: " 
           << iter->second.sub_block_dim_.x() << "x" <<  iter->second.sub_block_dim_.y() << "x" <<  iter->second.sub_block_dim_.z();
    vcl_string desc = scene->data_path() + " block footprint";
    bkml_write::write_box(ofs, box_id.str(), desc, ul, ur, ll, lr);
  }

  bkml_write::close_document(ofs);
  ofs.close();
  return true;
}
