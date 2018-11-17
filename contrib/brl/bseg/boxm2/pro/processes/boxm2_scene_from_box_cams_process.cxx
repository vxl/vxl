// This is brl/bseg/boxm2/pro/processes/boxm2_scene_from_box_cams_process.cxx
//:
// \file
// \brief  A process for computing scenes from perspective cameras and box
//
// \author J. L. Mundy
// \date May 24, 2011

#include <bprb/bprb_func_process.h>
#include <boxm2/boxm2_scene.h>
#include <bpgl/algo/bpgl_camera_from_box.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/util/boxm2_cams_and_box_to_scene.h>
#include <bpgl/bpgl_camera_utils.h>
namespace boxm2_scene_from_box_cams_process_globals
{
  constexpr unsigned n_inputs_ = 8;
  constexpr unsigned n_outputs_ = 0;
}
bool boxm2_scene_from_box_cams_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_scene_from_box_cams_process_globals;

  //process takes 1 input, the scene
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string" ; //cam_dir
  input_types_[1] = "float"; //xmin
  input_types_[2] = "float"; //ymon
  input_types_[3] = "float"; //zmin
  input_types_[4] = "float"; //width
  input_types_[5] = "float"; //height
  input_types_[6] = "float"; //depth
  input_types_[7] = "vcl_string"; // model dir

  // process has 1 output:
  std::vector<std::string>  output_types_(n_outputs_);


  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_scene_from_box_cams_process(bprb_func_process& pro)
{
  using namespace boxm2_scene_from_box_cams_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  std::string camdir = pro.get_input<std::string>(0);
  auto xmin = pro.get_input<float>(1);
  auto ymin = pro.get_input<float>(2);
  auto zmin = pro.get_input<float>(3);
  auto width = pro.get_input<float>(4);
  auto height = pro.get_input<float>(5);
  auto depth = pro.get_input<float>(6);
  std::string modeldir= pro.get_input<std::string>(7);
  auto lvcs_origin_lat = pro.get_input<double>(8);
  auto lvcs_origin_lon = pro.get_input<double>(9);
  auto lvcs_origin_elev = pro.get_input<double>(10);

  // get the scene bounding box
  vgl_box_3d<double> box(vgl_point_3d<double>(xmin,ymin,zmin),
                         vgl_point_3d<double>(xmin+width,ymin+height,zmin+depth));
  std::vector<vpgl_perspective_camera<double>* > ptrcams = bpgl_camera_utils::cameras_from_directory(camdir);
  std::vector<vpgl_perspective_camera<double> > cams;
  cams.reserve(ptrcams.size());
for(auto & ptrcam : ptrcams)
      cams.push_back( * ptrcam );

  std::vector<std::string> appearance;
  appearance.emplace_back("boxm2_mog3_grey");
  appearance.emplace_back("boxm2_num_obs");

  std::string scene_dir =modeldir+ "/model";
  if (!vul_file::make_directory_path( scene_dir.c_str()))
    return false;
  boxm2_scene_sptr uscene = new boxm2_scene(scene_dir, box.min_point());
  uscene->set_appearances(appearance);
  uscene->save_scene();

  vpgl_lvcs lvcs(lvcs_origin_lat, lvcs_origin_lon, lvcs_origin_elev, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  uscene->set_lvcs(lvcs);

  //build the two scenes
  boxm2_util_cams_and_box_to_scene(cams, box, *uscene);
  uscene->set_xml_path(scene_dir+"/uscene.xml");
  uscene->save_scene();
  return true;
}
