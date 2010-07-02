// Main to run opencl implentation of onlineupdate
#include <bocl/bocl_cl.h>
#include <boxm/ocl/boxm_ocl_utils.h>
#include <boxm/ocl/boxm_render_single_block_manager.h>

#include <boxm/boxm_scene.h>
#include <boxm/algo/boxm_init_scene.h>
#include <boxm/boxm_scene_parser.h>
#include <boxm/boxm_apm_traits.h>
#include <boxm/boxm_block.h>
#include <boct/boct_tree.h>


#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>

#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_timer.h>






int main(int argc,  char** argv)
{
  vcl_cout<<"RENDER "<<vcl_endl;
  vul_arg<vcl_string> cam_file("-cam", "camera filename", "");
  vul_arg<vcl_string> cam_dir("-camdir", "camera directory", "");
  vul_arg<vcl_string> scene_file("-scene", "scene filename", "");
  vul_arg<vcl_string> img("-img", "image filename", "");
  vul_arg<vcl_string> img_dir("-imgdir", "image directory", "");
  vul_arg<vcl_string> workdir("-workdir", "work directory", "");
  vul_arg<unsigned> ni("-ni", "Image width", 640);
  vul_arg<unsigned> nj("-nj", "Image height", 480);
  vul_arg_parse(argc, argv);
  boxm_render_single_block_manager* ray_mgr= boxm_render_single_block_manager::instance();
  int bundle_dim=8;
  ray_mgr->set_bundle_ni(bundle_dim);
  ray_mgr->set_bundle_nj(bundle_dim);

  vul_file::is_directory(workdir().c_str());

  vcl_string treefile =workdir()+"/treefile.bin";
  vcl_string treedatafile =workdir()+"/treedatafile.bin";

  
  vul_file_iterator file_it(vcl_string(cam_dir()+"/*.txt").c_str());
  //vul_file_iterator img_file_it(vcl_string(img_dir()+"/*.txt").c_str());

  int count=0;
  while(file_it)
  {
  // load the camera;
  vcl_ifstream ifs(file_it());
  vpgl_perspective_camera<double>* pcam =new vpgl_perspective_camera<double>;
  if (!ifs.is_open()) {
    vcl_cerr << "Failed to open file " << cam_file() << vcl_endl;
    return -1;
  }
  else {
    ifs >> *pcam;
  }

  vil_image_view<float> expimg(ni(),nj(),1);

  // load the scene;
  boxm_scene_parser parser;
  boxm_scene_base_sptr scene_ptr=new boxm_scene_base();
  scene_ptr->load_scene(scene_file(), parser);

  if (scene_ptr->appearence_model() == BOXM_APM_MOG_GREY) {
    typedef boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > tree_type;
    boxm_scene<tree_type>* scene = new boxm_scene<tree_type>();
    scene->load_scene(parser);
    scene_ptr = scene;
    boxm_init_scene<BOXM_APM_MOG_GREY>(*scene);
    ray_mgr->init_render(treefile,treedatafile,scene->origin(),scene->block_dim(), pcam, expimg,scene->max_level()-1);
  }
  else {
      return -1;
  }

  ray_mgr->run_scene();
  char filename[100];
  vcl_sprintf(filename,"f:/apl/test/img%05d.tif",count);
  ray_mgr->save_output_image(vcl_string(filename));

  ++file_it;++count;
  }
  return 0;
}

