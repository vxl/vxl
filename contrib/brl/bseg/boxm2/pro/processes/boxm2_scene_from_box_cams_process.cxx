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
  const unsigned n_inputs_ = 8;
  const unsigned n_outputs_ = 0;
}
bool boxm2_scene_from_box_cams_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_scene_from_box_cams_process_globals;

  //process takes 1 input, the scene
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string" ; //cam_dir
  input_types_[1] = "float"; //xmin
  input_types_[2] = "float"; //ymon
  input_types_[3] = "float"; //zmin
  input_types_[4] = "float"; //width
  input_types_[5] = "float"; //height
  input_types_[6] = "float"; //depth
  input_types_[7] = "vcl_string"; // model dir

  // process has 1 output:
  vcl_vector<vcl_string>  output_types_(n_outputs_);


  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_scene_from_box_cams_process(bprb_func_process& pro)
{
  using namespace boxm2_scene_from_box_cams_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  vcl_string camdir  = pro.get_input<vcl_string>(0);
  float xmin         = pro.get_input<float>(1);
  float ymin         = pro.get_input<float>(2);
  float zmin         = pro.get_input<float>(3);
  float width        = pro.get_input<float>(4);
  float height       = pro.get_input<float>(5);
  float depth        = pro.get_input<float>(6);
  vcl_string modeldir= pro.get_input<vcl_string>(7);
 
  // get the scene bounding box
  vgl_box_3d<double> box(vgl_point_3d<double>(xmin,ymin,zmin),
                         vgl_point_3d<double>(xmin+width,ymin+height,zmin+depth));
  vcl_vector<vpgl_perspective_camera<double>* > ptrcams = bpgl_camera_utils::cameras_from_directory(camdir);
  vcl_vector<vpgl_perspective_camera<double> > cams;
  for(unsigned int i = 0 ; i < ptrcams.size(); i++)
      cams.push_back( * (ptrcams[i]) );

  vcl_vector<vcl_string> appearance;
  appearance.push_back("boxm2_mog3_grey");
  appearance.push_back("boxm2_num_obs");

  vcl_string scene_dir =modeldir+ "/model";
  if (!vul_file::make_directory_path( scene_dir.c_str()))
    return false;
  boxm2_scene_sptr uscene = new boxm2_scene(scene_dir, box.min_point());
  uscene->set_appearances(appearance);
  uscene->save_scene();

  //create render scene
  boxm2_scene_sptr rscene = new boxm2_scene(scene_dir, box.min_point());
  rscene->set_appearances(appearance);
  rscene->save_scene();

  //build the two scenes
  boxm2_util_cams_and_box_to_scene(cams, box, *uscene, *rscene);
  uscene->set_xml_path(scene_dir+"/uscene.xml");
  uscene->save_scene();
  rscene->set_xml_path(scene_dir+"/rscene.xml");
  rscene->save_scene();
  return true;
}
