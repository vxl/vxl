// This is brl/bseg/boxm2/pro/processes/boxm2_create_scene_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for creating a scene
//
// \author Vishal Jain
// \date Mar 15, 2011

#include <vcl_fstream.h>
#include <vul/vul_file.h>
#include <boxm2/boxm2_scene.h>

namespace boxm2_create_scene_process_globals
{
  const unsigned n_inputs_ = 10;
  const unsigned n_outputs_ = 1;
}

bool boxm2_create_scene_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_create_scene_process_globals;

  //process takes 10 inputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";
  input_types_[1] = "vcl_string";
  input_types_[2] = "vcl_string";
  input_types_[3] = "float"; // origin x
  input_types_[4] = "float"; // origin y
  input_types_[5] = "float"; // origin z
  input_types_[6] = "float"; // lon
  input_types_[7] = "float"; // lat
  input_types_[8] = "float"; // elev
  input_types_[9] = "int";   // number of illumination bins in the scene

  // process has 1 output
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "boxm2_scene_sptr";

  // ill bins might not be set
  brdb_value_sptr idx = new brdb_value_t<int>(0);
  brdb_value_sptr idx2 = new brdb_value_t<float>(0);
  pro.set_input(9, idx);
  pro.set_input(8, idx2);
  pro.set_input(7, idx2);
  pro.set_input(6, idx2);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_create_scene_process(bprb_func_process& pro)
{
  using namespace boxm2_create_scene_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  vcl_vector<vcl_string> appearance(2,"");
  unsigned i = 0;
  vcl_string datapath = pro.get_input<vcl_string>(i++);
  appearance[0]       = pro.get_input<vcl_string>(i++); //Appearance Model String
  appearance[1]       = pro.get_input<vcl_string>(i++); //Occupancy Model String
  float origin_x      = pro.get_input<float>(i++);
  float origin_y      = pro.get_input<float>(i++);
  float origin_z      = pro.get_input<float>(i++);
  float lon           = pro.get_input<float>(i++);
  float lat           = pro.get_input<float>(i++);
  float elev          = pro.get_input<float>(i++);
  int num_bins        = pro.get_input<int>(i++);

  if (!vul_file::make_directory_path(datapath.c_str()))
    return false;
  boxm2_scene_sptr scene =new boxm2_scene(datapath,vgl_point_3d<double>(origin_x,origin_y,origin_z));
  scene->set_local_origin(vgl_point_3d<double>(origin_x,origin_y,origin_z));
  scene->set_appearances(appearance);
#if 0
  vpgl_lvcs lv = scene->lvcs();
  lv.set_origin((double)lon, (double)lat, (double)elev);
#endif
  vpgl_lvcs lv(lat, lon, elev, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  scene->set_lvcs(lv);
  scene->set_num_illumination_bins(num_bins);
  i=0;  // store scene smart pointer
  pro.set_output_val<boxm2_scene_sptr>(i++, scene);
  return true;
}


//: A process that takes two (lat,lon,elev) positions and creates a scene with a given voxel size and corresponding block structure.
//  lvcs is used to figure out the local origins of the blocks
namespace boxm2_create_scene_and_blocks_process_globals
{
  const unsigned n_inputs_ = 17;
  const unsigned n_outputs_ = 1;
}

bool boxm2_create_scene_and_blocks_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_create_scene_and_blocks_process_globals;
  //process takes 13 inputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string"; // scene dir (with no slash at the end)
  input_types_[1] = "vcl_string"; //Appearance Model String
  input_types_[2] = "vcl_string"; //Occupancy Model String
  input_types_[3] = "float"; // lon1  // point1: coords of lower left corner of the scene bounding box
  input_types_[4] = "float"; // lat1
  input_types_[5] = "float"; // elev1
  input_types_[6] = "float"; // lon2  // coords of upper right corner of the scene bounding box
  input_types_[7] = "float"; // lat2
  input_types_[8] = "float"; // elev2
  input_types_[9] = "float"; // origin_lon, origin of lvcs may be different than lower left corner
  input_types_[10] = "float"; // origin_lat
  input_types_[11] = "float"; // origin elev
  input_types_[12] = "float"; // voxel size in meters
  input_types_[13] = "float"; // block length in meters, set according to memory requirement of GPU
  input_types_[14] = "float"; // block length in z in meters, set according to memory requirement of GPU
  input_types_[15] = "int";   // number of illumination bins in the scene
  input_types_[16] = "vcl_string";   // name of the local coordinate system for lvcs, it could be wgs84 for a local plane with point1 as origin at the middle or utm plane of the point1 with point1 as the origin

  // process has 1 output
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "boxm2_scene_sptr";

  // ill bins might not be set
  brdb_value_sptr idx = new brdb_value_t<int>(0);
  pro.set_input(10, idx);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_create_scene_and_blocks_process(bprb_func_process& pro)
{
  using namespace boxm2_create_scene_and_blocks_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  vcl_vector<vcl_string> appearance(2,"");
  unsigned i = 0;
  vcl_string datapath = pro.get_input<vcl_string>(i++);
  appearance[0]       = pro.get_input<vcl_string>(i++); //Appearance Model String
  appearance[1]       = pro.get_input<vcl_string>(i++); //Occupancy Model String
  float lon1          = pro.get_input<float>(i++);
  float lat1          = pro.get_input<float>(i++);
  float elev1         = pro.get_input<float>(i++);
  float lon2          = pro.get_input<float>(i++);
  float lat2          = pro.get_input<float>(i++);
  float elev2         = pro.get_input<float>(i++);
  float origin_lon    = pro.get_input<float>(i++);
  float origin_lat    = pro.get_input<float>(i++);
  float origin_elev   = pro.get_input<float>(i++);
  float voxel_size    = pro.get_input<float>(i++);
  float block_len     = pro.get_input<float>(i++);  // blocks have equal length on x and y direction
  float block_lenz     = pro.get_input<float>(i++);
  int num_bins        = pro.get_input<int>(i++);
  vcl_string cs_name  = pro.get_input<vcl_string>(i++);

  unsigned init_level = 1;
  unsigned max_level = 4;
  float max_data_mb = 1000.0;
  float p_init = 0.001;

  if (!vul_file::make_directory_path(datapath.c_str()))
    return false;

  vpgl_lvcs::cs_names cs_id;
  if (cs_name == "wgs84")
    cs_id = vpgl_lvcs::wgs84;
  else if (cs_name == "utm")
    cs_id = vpgl_lvcs::utm;
  else if (cs_name == "nad27n")
    cs_id = vpgl_lvcs::nad27n;
  else if (cs_name == "wgs72")
    cs_id = vpgl_lvcs::wgs72;
  else
    cs_id = vpgl_lvcs::NumNames; // this should never happen ... (avoids compiler warning)

  vpgl_lvcs lv(origin_lat, origin_lon, origin_elev, cs_id, vpgl_lvcs::DEG, vpgl_lvcs::METERS);

  double local_origin_x = 0.0, local_origin_y = 0.0, local_origin_z = 0.0; // this is local origin
  double lx, ly, lz;
  lv.global_to_local(lon1, lat1, elev1, vpgl_lvcs::wgs84, local_origin_x, local_origin_y, local_origin_z);
  vcl_cout << "local coords:\nlat1,lon1,elev1: " << lx << ' ' << ly << ' ' << lz << vcl_endl;
  lv.global_to_local(lon2, lat2, elev2, vpgl_lvcs::wgs84, lx, ly, lz);
  vcl_cout << "lat2,lon2,elev2: " << lx << ' ' << ly << ' ' << lz << vcl_endl;
  lx -= local_origin_x;  // width
  ly -= local_origin_y;  // depth
  lz -= local_origin_z;  // height
  if (lx <= 0 || ly <= 0 || lz <= 0) {
    vcl_cout << "error: negative width or height! select the upper right corner to the North-East of lower left corner of the scene!\n";
  }
  vcl_cout << "local origin of the scene: " << local_origin_x << ", " << local_origin_y << ", " << local_origin_z << " width: " << lx << " depth: " << ly << " height: " << lz << vcl_endl;

  boxm2_scene_sptr scene =new boxm2_scene(datapath,vgl_point_3d<double>(local_origin_x,local_origin_y,local_origin_z));
  scene->set_local_origin(vgl_point_3d<double>(local_origin_x,local_origin_y,local_origin_z));
  scene->set_appearances(appearance);

  scene->set_lvcs(lv);
  scene->set_num_illumination_bins(num_bins);

  // calculate number of voxels and block and subblock sizes
  float sb_length = 8*voxel_size;

  int num_xy = (int)vcl_ceil(block_len/sb_length);
  int num_z = (int)vcl_ceil(block_lenz/sb_length);
  int n_x = (int)vcl_ceil(lx / (num_xy*sb_length));
  int n_y = (int)vcl_ceil(ly / (num_xy*sb_length));
  int n_z = (int)vcl_ceil(lz / (num_z*sb_length));

  vcl_cout << "sb_length: " << sb_length << " block_len_xy: " << block_len << " num_xy: " << num_xy << '\n'
           << "block_len z: " << block_lenz << " num_z: " << num_z << '\n'
           << "num of blocks in x: " << n_x << " y: " << n_y << " n_z: " << n_z << '\n'
           << "input scene length x: " << lx << " blocked x: " << n_x*num_xy*sb_length << '\n'
           << "input scene length y: " << ly << " blocked y: " << n_y*num_xy*sb_length << '\n'
           << "input scene length z: " << lz << " blocked z: " << n_z*num_z*sb_length << vcl_endl;

  for (int i = 0; i < n_x; ++i)
    for (int j = 0; j < n_y; ++j)
      for (int k = 0; k < n_z; ++k) {
        boxm2_block_id id(i,j,k);
        vcl_map<boxm2_block_id, boxm2_block_metadata>& blks=scene->blocks();

        if (blks.find(id)!=blks.end())
        {
          vcl_cout<<"Problems in adding block: (" << i << ',' << j << ',' << k << ") block already exists"<<vcl_endl;
          return false;
        }
        double local_z = k*num_z*sb_length + local_origin_z;
        double local_y = j*num_xy*sb_length + local_origin_y;
        double local_x = i*num_xy*sb_length + local_origin_x;

        boxm2_block_metadata mdata(id,vgl_point_3d<double>(local_x,local_y,local_z),
                                   vgl_vector_3d<double>(sb_length,sb_length,sb_length),
                                   vgl_vector_3d<unsigned>(num_xy,num_xy,num_z),
                                   init_level,max_level,max_data_mb,p_init);

        blks[id]=mdata;
      }

  i=0;  // store scene smart pointer
  pro.set_output_val<boxm2_scene_sptr>(i++, scene);
  return true;
}


//: A process that takes two (lat,lon,elev) positions and creates a scene with a given voxel size and corresponding block structure.
//  lvcs is used to figure out the local origins of the blocks
namespace boxm2_distribute_scene_blocks_process_globals
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 0;
}

bool boxm2_distribute_scene_blocks_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_distribute_scene_blocks_process_globals;

  //process takes 13 inputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr"; // big scene
  input_types_[1] = "double"; // dimension of the smaller scenes (in units of the local coordinate system, e.g. if UTM in meters)
  input_types_[2] = "vcl_string"; // output folder to write the smaller scene xml files
  input_types_[3] = "vcl_string"; // xml name prefix

  // process has 1 output
  vcl_vector<vcl_string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_distribute_scene_blocks_process(bprb_func_process& pro)
{
  using namespace boxm2_distribute_scene_blocks_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  vcl_vector<vcl_string> appearance(2,"");
  unsigned i = 0;

  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  double scene_dim = pro.get_input<double>(i++);
  vcl_string output_path = pro.get_input<vcl_string>(i++);
  vcl_string name_prefix = pro.get_input<vcl_string>(i++);

  vpgl_lvcs lv = scene->lvcs();

  vcl_vector<boxm2_scene_sptr> small_scenes;

  vcl_map<boxm2_block_id, boxm2_block_metadata>& blks=scene->blocks();
  vcl_cout << "number of blocks in the scene: " << blks.size() << vcl_endl;
  vgl_box_3d<double> bb = scene->bounding_box();
  for (double orig_x = bb.min_point().x(); orig_x <= bb.max_point().x(); orig_x += scene_dim)
    for (double orig_y = bb.min_point().y(); orig_y <= bb.max_point().y(); orig_y += scene_dim) {
      boxm2_scene_sptr small_scene = new boxm2_scene(scene->data_path(), scene->local_origin());
      small_scene->set_appearances(scene->appearances());
      small_scene->set_lvcs(lv);
      small_scene->set_num_illumination_bins(scene->num_illumination_bins());
      vcl_map<boxm2_block_id, boxm2_block_metadata>& small_scene_blks=small_scene->blocks();

      vgl_box_2d<double> small_scene_box;
      small_scene_box.add(vgl_point_2d<double>(orig_x, orig_y));
      small_scene_box.add(vgl_point_2d<double>(orig_x + scene_dim - scene_dim/1000, orig_y + scene_dim - scene_dim/1000));
      // find all the blocks in the scene with (orig_x, orig_y) <-> (orig_x + scene_dim, orig_y + scene_dim)
      for (vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator iter = blks.begin(); iter != blks.end(); iter++) {
        boxm2_block_metadata md = iter->second;
        vgl_point_2d<double> lo(iter->second.local_origin_.x(), iter->second.local_origin_.y());
        if (small_scene_box.contains(lo))
          small_scene_blks[iter->first] = iter->second;
      }

      small_scenes.push_back(small_scene);
    }

  vcl_cout << output_path + name_prefix + ".xml\n"
           << " number of small scenes: " << small_scenes.size() << vcl_endl;

  // write each scene
  for (unsigned i = 0; i < small_scenes.size(); i++) {
    vcl_stringstream ss; ss << i;
    vcl_string filename = name_prefix + ss.str();
    vcl_string filename_full = output_path + name_prefix + ss.str() + ".xml";

    small_scenes[i]->set_xml_path(filename_full);

    //make file and x_write to file
    vcl_ofstream ofile(filename_full.c_str());
    x_write(ofile,(*small_scenes[i].ptr()), "scene");
  }

  return true;
}
