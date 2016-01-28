// This is brl/bseg/bstm/pro/processes/bstm_bundle_to_scene_process.cxx
#include <bprb/bprb_func_process.h>
// :
// \file
// \brief  A process for creating a scene from a bundler file
//
// \author Andy Miller
// \date Sep 16, 2011
#include <vcl_fstream.h>
#include <vcl_cstdio.h> // for sprintf()
#include <bstm/bstm_scene.h>
#include <boxm2/util/boxm2_convert_nvm.h>
#include <bstm/util/bstm_cams_and_box_to_scene.h>
#include <vgl/vgl_box_3d.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>

namespace bstm_bundle_to_scene_process_globals
{
const unsigned n_inputs_ = 7;
const unsigned n_outputs_ = 1;
}

bool bstm_bundle_to_scene_process_cons(bprb_func_process& pro)
{
  using namespace bstm_bundle_to_scene_process_globals;

  // process takes 5 inputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string"; // bundler or nvm file
  input_types_[1] = "vcl_string"; // image dir path (all images)
  input_types_[2] = "vcl_string"; // appearance model
  input_types_[3] = "vcl_string"; // num_obs model
  input_types_[4] = "bool";       // to align the model with z-plane ( good for aerial models )
  input_types_[5] = "unsigned";   // number of time steps
  input_types_[6] = "vcl_string"; // optional arg - output dir to save cams/imgs

  // process has 2 outputs
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "bstm_scene_sptr";  // update scene

  // set input and output types
  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
  return good;
}

bool bstm_bundle_to_scene_process(bprb_func_process& pro)
{
  using namespace bstm_bundle_to_scene_process_globals;
  typedef vpgl_perspective_camera<double> CamType;
  if( pro.n_inputs() < n_inputs_ )
    {
    vcl_cout << pro.name() << ": The number of inputs should be " << n_inputs_ << vcl_endl;
    return false;
    }

  // ----------------------------------------------------------------------------
  // get the inputs
  // ----------------------------------------------------------------------------
  unsigned               i = 0;
  vcl_string             bundler_out = pro.get_input<vcl_string>(i++); // bundler out
  vcl_string             in_img_dir  = pro.get_input<vcl_string>(i++); // input png/tiff images
  vcl_vector<vcl_string> appearance(2, "");
  appearance[0]          = pro.get_input<vcl_string>(i++); // Appearance Model String
  appearance[1]          = pro.get_input<vcl_string>(i++); // Occupancy Model String
  bool       axis_align        = pro.get_input<bool>(i++);
  unsigned   time_steps   = pro.get_input<unsigned>(i++);
  vcl_string out_dir     = pro.get_input<vcl_string>(i++); // output dir for imgs/files

  // ----------------------------------------------------------------------------
  // run bundle to scene
  // ----------------------------------------------------------------------------
  vcl_map<vcl_string, CamType *>     cams;
  vgl_box_3d<double>                 bbox;
  double                             resolution;
  vcl_vector<vgl_point_3d<double>  > pts3d;
  if( vul_file::extension(bundler_out) == ".nvm" )
    {
    boxm2_util_convert_nvm(bundler_out, in_img_dir, cams, pts3d, bbox, resolution, axis_align);
    }

  // create vector of camera objects
  vcl_vector<vpgl_perspective_camera<double> >                     cs;
  vcl_map<vcl_string, vpgl_perspective_camera<double> *>::iterator iter;
  for( iter = cams.begin(); iter != cams.end(); ++iter )
    {
    cs.push_back(*iter->second);
    }

  // ----------------------------------------------------------------------------
  // run cams and bounding box to scene
  // ----------------------------------------------------------------------------
  // create update scene
  vcl_string scene_dir = out_dir + "/bstm/";
  if( !vul_file::make_directory_path( scene_dir.c_str() ) )
    {
    return false;
    }
  bstm_scene_sptr uscene = new bstm_scene(scene_dir, bbox.min_point() );
  bstm_util_cams_and_box_to_scene(cs, bbox, *uscene, time_steps);

  uscene->set_local_origin(vgl_point_3d<double>(0.0, 0.0, 0.0) );
  uscene->set_appearances(appearance);
  uscene->save_scene();

  // ----------------------------------------------------------------------------
  // if output directory is non empty, create directory and save imgs, cams dirs
  // ----------------------------------------------------------------------------
#if 0
  if( out_dir != "" )
    {
    vcl_string krt_dir = out_dir + "/cams_krt";
    vcl_string img_dir = out_dir + "/imgs";
    if( !vul_file::make_directory_path( out_dir.c_str() ) ||
        !vul_file::make_directory_path( krt_dir.c_str() ) ||
        !vul_file::make_directory_path( img_dir.c_str() ) )
      {
      vcl_cout << "bstm_bundle_to_scene_process: cannot write images/cams to disk" << vcl_endl;
      }
    else
      {
      vcl_cout << "    Writing cameras and images to disk" << vcl_endl;

      // write cams to disk
      vcl_map<vcl_string, CamType *>::iterator iter;
      for( iter = cams.begin(); iter != cams.end(); ++iter )
        {
        // image basename
        vcl_string full_img_name = iter->first;
        vcl_string img_name      = vul_file::basename(full_img_name);
        vcl_string stripped_name = vul_file::strip_extension(img_name);

        // good camera
        CamType cam      = *iter->second;

        // save cam file
        char filename[1024];
        vcl_sprintf(filename, "%s/%s_cam.txt", krt_dir.c_str(), stripped_name.c_str() );
        vcl_ofstream ofile(filename);
        double       u1, v1;
        cam.project(0, 0, 0, u1, v1);
        if( ofile )
          {
          ofile << cam.get_calibration().get_matrix() << '\n'
                << cam.get_rotation().as_matrix() << '\n'
                << cam.get_translation().x() << ' '
                << cam.get_translation().y() << ' '
                << cam.get_translation().z() << '\n';
          }
        ofile.close();

        // save image

        vcl_string        outImgName;
        vcl_string        inImgGlob = in_img_dir + "/" + stripped_name + ".*JPG";
        vul_file_iterator moveImage(inImgGlob.c_str() );
        if( moveImage )
          {
          outImgName = vcl_string(moveImage() );
          vcl_cout << "    Writing camera and image for image " << outImgName << vcl_endl;
          vil_image_view_base_sptr img = vil_load(outImgName.c_str() );
          vcl_string               img_path = img_dir + "/" + vul_file::basename(outImgName);
          vil_save( *img, img_path.c_str() );
          }
        else
          {
          vcl_cout << "Cannot move image " << full_img_name << " ! breaking !" << vcl_endl;
          }
        }
      }
    }
#endif

  // set output
  i = 0;  // store scene smart pointer
  pro.set_output_val<bstm_scene_sptr>(i++, uscene);

  vcl_cout << "Done." << vcl_endl;
  return true;
}
