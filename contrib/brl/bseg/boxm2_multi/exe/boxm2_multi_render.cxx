//:
// \file
// \brief  First attempt at multi gpu render
// \author Andy Miller
// \date 13-Oct-2011
#include <iostream>
#include <algorithm>
#include <boxm2_multi_cache.h>
#include <algo/boxm2_multi_render.h>

#include <vcl_where_root_dir.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//executable args
#include <vul/vul_arg.h>
#include <vul/vul_timer.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_save.h>

#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/ocl/boxm2_opencl_cache1.h>

#include <bocl/bocl_manager.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>

void test_render_expected_images(const boxm2_scene_sptr& scene,
                                 const bocl_device_sptr& device,
                                 boxm2_opencl_cache1* cache,
                                 std::vector<vpgl_camera_double_sptr>& cams,
                                 unsigned ni, unsigned nj)
{
  //register data types and process functions
  DECLARE_FUNC_CONS(boxm2_ocl_render_expected_color_process);
  DECLARE_FUNC_CONS(boxm2_ocl_render_expected_image_process);
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_render_expected_color_process, "boxm2OclRenderExpectedColorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_render_expected_image_process, "boxm2OclRenderExpectedImageProcess");
  REGISTER_DATATYPE(boxm2_opencl_cache1_sptr);
  REGISTER_DATATYPE(boxm2_scene_sptr);
  REGISTER_DATATYPE(vil_image_view_base_sptr);
  REGISTER_DATATYPE(float);

  //////////////////////////////////////////////////////////////////////////////
  // Has to be done on each render
  //setup image size
#ifdef DEBUG
  int num_renders = 10;
#endif
  for (unsigned int i=0; i<cams.size(); ++i)
  {
    //set up brdb_value_sptr arguments... (for generic passing)
    brdb_value_sptr brdb_device = new brdb_value_t<bocl_device_sptr>(device);
    brdb_value_sptr brdb_scene = new brdb_value_t<boxm2_scene_sptr>(scene);
    brdb_value_sptr brdb_opencl_cache = new brdb_value_t<boxm2_opencl_cache1_sptr>(cache);
    brdb_value_sptr brdb_cam = new brdb_value_t<vpgl_camera_double_sptr>(cams[i]);
    brdb_value_sptr brdb_ni = new brdb_value_t<unsigned>(ni);
    brdb_value_sptr brdb_nj = new brdb_value_t<unsigned>(nj);

    //if scene has RGB data type, use color render process
    bool good = true;
    if (scene->has_data_type(boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix()) ) {
      good = bprb_batch_process_manager::instance()->init_process("boxm2OclRenderExpectedColorProcess");
    }
    else {
      good = bprb_batch_process_manager::instance()->init_process("boxm2OclRenderExpectedImageProcess");
    }
    if ( !good ) {
      std::cout << "ERROR: couldn't start color render process: " << __FILE__ << __LINE__ << std::endl;
    }

    //set process args
    good = good && bprb_batch_process_manager::instance()->set_input(0, brdb_device) // device
                && bprb_batch_process_manager::instance()->set_input(1, brdb_scene)  //  scene
                && bprb_batch_process_manager::instance()->set_input(2, brdb_opencl_cache)
                && bprb_batch_process_manager::instance()->set_input(3, brdb_cam)    // camera
                && bprb_batch_process_manager::instance()->set_input(4, brdb_ni)     // ni for rendered image
                && bprb_batch_process_manager::instance()->set_input(5, brdb_nj)     // nj for rendered image
                && bprb_batch_process_manager::instance()->run_process();
    if ( !good ) {
      std::cout << "ERROR: couldn't set process args: " << __FILE__ << __LINE__ << std::endl;
    }

    //grab vil_image_view_base_sptr from process
    unsigned int out_img = 0;
    good = good && bprb_batch_process_manager::instance()->commit_output(0, out_img);
    if ( !good ) {
      std::cout << "ERROR: couldn't commit output: " << __FILE__ << __LINE__ << std::endl;
    }

    brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, out_img);
    brdb_selection_sptr S = DATABASE->select("vil_image_view_base_sptr_data", std::move(Q));
    if (S->size()!=1) {
      std::cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
               << " no selections\n";
    }
    brdb_value_sptr value;
    if (!S->get_value(std::string("value"), value)) {
      std::cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
               << " didn't get value\n";
    }
    vil_image_view_base_sptr out_img_sptr =value->val<vil_image_view_base_sptr>();
    std::stringstream s; s<<"out_single"<<i<<".png";
    vil_save(*out_img_sptr.ptr(),s.str().c_str());
  }
#ifdef DEBUG
  std::cout<<"Mean render time: "<<mean_time/num_renders<<std::endl;
#endif
}

int main(int argc,  char** argv)
{
  //init vgui (should choose/determine toolkit)
  vul_arg<std::string> scene_file("-scene", "scene filename", "");
  vul_arg<unsigned>   ni("-ni", "Width of output image", 1280);
  vul_arg<unsigned>   nj("-nj", "Height of output image", 720);
  vul_arg<unsigned>   numFrames("-num", "Maximum number of frames to render", 10);
  vul_arg<unsigned>   numGPU("-numGPU", "Number of GPUs to use", 2);
  vul_arg_parse(argc, argv);

  //create scene
  boxm2_scene_sptr scene = new boxm2_scene(scene_file());

  //make bocl manager (handles a lot of OpenCL stuff)
  bocl_manager_child &mgr = bocl_manager_child::instance();

  //create cpu cache (lru), and create opencl_cache on the device
  boxm2_lru_cache1::create(scene);

  if (numGPU() > mgr.gpus_.size()) {
    std::cout<<"-numGPU ("<<numGPU()<<") is too big, only "<<mgr.gpus_.size()<<" available"<<std::endl;
  }
  //make a multicache
  std::vector<bocl_device_sptr> gpus;
  for (unsigned int i=0; i<numGPU(); ++i)
    gpus.push_back(mgr.gpus_[i]);
  boxm2_multi_cache mcache(scene, gpus);
  std::cout<<"Multi Cache:\n"<<mcache.to_string()<<std::endl;

  //generate cameras
  int num_renders = (int) numFrames();
  std::vector<vpgl_camera_double_sptr> cams;
  for (int i=0; i<num_renders; ++i) {
    double currInc    = 45.0;
    double currRadius = scene->bounding_box().height();
    double currAz     = i*30.0f;
    vpgl_perspective_camera<double>* pcam;
    pcam = boxm2_util::construct_camera(currInc, currAz, currRadius, ni(), nj(), scene->bounding_box(), false);
    vpgl_camera_double_sptr cam = new vpgl_perspective_camera<double>(*pcam);
    cams.push_back(cam);
  }

  //render each cam
  float mean_time=0.0f, gpu_total=0.0f;
  boxm2_multi_render renderer;
  for (unsigned int i=0; i<cams.size(); ++i) {
    vil_image_view<float> out(ni(),nj());
    vul_timer rtimer; rtimer.mark();
    float gpu_time = renderer.render(mcache, out, cams[i]);
    auto rtime = (float) rtimer.all();
    std::cout<<"Render "<<i<<" time: "<<rtime<<std::endl;
    std::stringstream s; s<<"e:/data/3dModeling/apt/out_"<<i<<".tiff";
    vil_save(out, s.str().c_str());
    mean_time += rtime;
    gpu_total += gpu_time;
  }

  std::cout<<"-----------------------------------------\n"
          <<" RENDERING STATS:\n"
          <<"   Mean render time: "<<mean_time/cams.size()<<'\n'
          <<"   Mean GPU time: "<<gpu_total/cams.size()<<'\n'
          <<"-----------------------------------------"<<std::endl;

  //test scene render on one gpu
  //boxm2_opencl_cache1* opencl_cache = new boxm2_opencl_cache1(scene, mgr.gpus_[0]);
  //test_render_expected_images(scene, mgr.gpus_[0], opencl_cache, cams, ni(), nj());

  return 0;
}
