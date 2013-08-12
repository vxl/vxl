// This is brl/bseg/boxm2/pro/processes/boxm2_scene_from_nvm_txt_process.cxx
//:
// \file
// \brief  A process for creating a scene from VisualSFM's pmvs output.
//
// \author Ali Osman Ulusoy
// \date August 12, 2013

#include <bprb/bprb_func_process.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/util/boxm2_convert_nvm_txt.h>
#include <boxm2/util/boxm2_cams_and_box_to_scene.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vcl_cstdio.h> // for sprintf()



namespace boxm2_scene_from_nvm_txt_process_globals
{
  const unsigned n_inputs_ = 9;
  const unsigned n_outputs_ = 0;
}
bool boxm2_scene_from_nvm_txt_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_scene_from_nvm_txt_process_globals;

  //process takes 1 input, the scene
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string" ; //nvm txt file
  input_types_[1] = "vcl_string" ; //input img file
  input_types_[2] = "float"; //xmin
  input_types_[3] = "float"; //ymon
  input_types_[4] = "float"; //zmin
  input_types_[5] = "float"; //width
  input_types_[6] = "float"; //height
  input_types_[7] = "float"; //depth
  input_types_[8] = "vcl_string"; // model dir

  // process has 1 output:
  vcl_vector<vcl_string>  output_types_(n_outputs_);


  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_scene_from_nvm_txt_process(bprb_func_process& pro)
{
  using namespace boxm2_scene_from_nvm_txt_process_globals;
  typedef vpgl_perspective_camera<double> CamType;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  vcl_string nvm_filename  = pro.get_input<vcl_string>(0);
  vcl_string input_img_folder  = pro.get_input<vcl_string>(1);
  float x_center         = pro.get_input<float>(2);
  float y_center        = pro.get_input<float>(3);
  float z_center         = pro.get_input<float>(4);
  float width        = pro.get_input<float>(5);
  float height       = pro.get_input<float>(6);
  float depth        = pro.get_input<float>(7);
  vcl_string modeldir= pro.get_input<vcl_string>(8);

  // get the scene bounding box
  vgl_box_3d<double> box(vgl_point_3d<double>(x_center,y_center,z_center), width,height,depth, vgl_box_3d<double>::centre );


  //loadup the nvm file
  vcl_map<vcl_string, CamType*> cams;
  vcl_map<vcl_string, vcl_string> img_name_mapping;

  if (vul_file::extension(nvm_filename) == ".txt")
    boxm2_util_convert_nvm_txt(nvm_filename, input_img_folder, cams,img_name_mapping);

  vcl_vector<vcl_string> appearance;
  appearance.push_back("boxm2_mog3_grey");
  appearance.push_back("boxm2_num_obs");

  vcl_string scene_dir = modeldir;
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
  vcl_vector<vpgl_perspective_camera<double> > cs;
  for (vcl_map<vcl_string, vpgl_perspective_camera<double>* >::iterator iter=cams.begin(); iter!=cams.end(); ++iter)
    cs.push_back(* iter->second);

  boxm2_util_cams_and_box_to_scene(cs, box, *uscene, *rscene);
  uscene->set_xml_path(scene_dir+"/uscene.xml");
  uscene->save_scene();
  rscene->set_xml_path(scene_dir+"/rscene.xml");
  rscene->save_scene();

  if (modeldir != "")
  {

    vcl_string krt_dir = modeldir + "/cams_krt";
    vcl_string img_dir = modeldir + "/imgs";
    if (! vul_file::make_directory_path( modeldir.c_str() ) ||
        ! vul_file::make_directory_path( krt_dir.c_str() ) ||
        ! vul_file::make_directory_path( img_dir.c_str() ) ) {
      vcl_cout<<"boxm2_bundle_to_scene_process: cannot write images/cams to disk"<<vcl_endl;
    }
    else {
      vcl_cout<<"    Writing cameras and images to disk"<<vcl_endl;

      //write cams to disk
      vcl_map<vcl_string, CamType*>::iterator iter;
      for (iter = cams.begin(); iter != cams.end(); ++iter)
      {
        //image basename
        vcl_string full_img_name = iter->first;
        vcl_string img_name      = vul_file::basename(full_img_name);
        vcl_string stripped_name = vul_file::strip_extension(img_name);

        //good camera
        CamType    cam      = *iter->second;

        //save cam file
        char filename[1024];
        vcl_sprintf(filename,"%s/%s_cam.txt", krt_dir.c_str(), stripped_name.c_str());
        vcl_ofstream ofile(filename);
        double u1,v1;
        cam.project(0,0,0,u1,v1);
        if (ofile)
        {
          ofile<<cam.get_calibration().get_matrix()<<'\n'
               <<cam.get_rotation().as_matrix()<<'\n'
               <<cam.get_translation().x()<<' '
               <<cam.get_translation().y()<<' '
               <<cam.get_translation().z()<<'\n';
        }
        ofile.close();

        //save image
        vcl_string outImgName;
        vcl_string inImgGlob = input_img_folder + "/" + img_name_mapping[full_img_name];
        vul_file_iterator moveImage(inImgGlob.c_str());
        if (moveImage) {
          outImgName = vcl_string(moveImage());
          vcl_cout<<"    Writing camera and image for image "<<outImgName<<vcl_endl;
          vil_image_view_base_sptr img = vil_load(outImgName.c_str());
          vcl_string img_path = img_dir + "/" + img_name;
          vil_save( *img, img_path.c_str() );
        }
        else {
          vcl_cout<<"Cannot move image "<<full_img_name<<" ! breaking !"<<vcl_endl;
        }
      }
    }
  }

  return true;
}
