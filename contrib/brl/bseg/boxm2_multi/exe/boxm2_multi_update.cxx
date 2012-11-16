//:
// \file
// \brief  First attempt at multi gpu update
// \author Andy Miller
// \date 13-Nov-2011

#include <boxm2_multi_cache.h>
#include <algo/boxm2_multi_render.h>
#include <algo/boxm2_multi_update.h>
#include <algo/boxm2_multi_refine.h>

#include <vcl_where_root_dir.h>
#include <vcl_algorithm.h>

//executable args
#include <vil/vil_image_view_base.h>
#include <vil/vil_save.h>
#include <vul/vul_timer.h>
#include <vul/vul_arg.h>

#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>

#include <bocl/bocl_manager.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>

int main(int argc,  char** argv)
{
  //init vgui (should choose/determine toolkit)
  vul_arg<vcl_string> scene_file("-scene", "scene filename", "");
  vul_arg<vcl_string> img_dir("-imgdir", "update image directory", "nvm_out/imgs/");
  vul_arg<vcl_string> cam_dir("-camdir", "update camera directory", "nvm_out/cams_krt/");
  vul_arg<unsigned>   num_updates("-num", "Number of updates", 10);
  vul_arg<int>        inFrame("-frame", "Single frame to use", -1);
  vul_arg<int>        renderInt("-renderInt", "Interval to render progress", -1);
  vul_arg<unsigned>   numGPU("-numGPU", "Number of GPUs to use", 2);
  vul_arg_parse(argc, argv);

  //create scene
  boxm2_scene_sptr scene = new boxm2_scene(scene_file());

  //make bocl manager (handles a lot of OpenCL stuff)
  bocl_manager_child_sptr mgr = bocl_manager_child::instance();

  //create cpu cache (lru), and create opencl_cache on the device
  boxm2_lru_cache::create(scene);

  //make a multicache
  if ( numGPU() > mgr->gpus_.size() ) {
    vcl_cout<<"-numGPU ("<<numGPU()<<") is too big, only "<<mgr->gpus_.size()<<" available"<<vcl_endl;
    return -1;
  }

  //grab the number of devices specified
  vcl_vector<bocl_device*> gpus;
  for (unsigned int i=0; i<numGPU(); ++i)
    gpus.push_back(mgr->gpus_[i]);
  boxm2_multi_cache mcache(scene, gpus);
  vcl_cout<<"Multi Cache:\n"<<mcache.to_string()<<vcl_endl;

  //-- GET UPDATE IMG/CAMS ---
  vcl_vector<vcl_string> imgs = boxm2_util::images_from_directory( img_dir() );
  vcl_vector<vcl_string> cams = boxm2_util::camfiles_from_directory( cam_dir() );

  //-----------------------------------------------------------
  //render setup
  //-----------------------------------------------------------
  boxm2_multi_render renderer;
  int ni=1280, nj=720;

  //-----------------------------------------------------------
  //run update/renders
  //-----------------------------------------------------------
  int numUpdates = vcl_min( (int) num_updates(), (int) imgs.size());

  //shuffle frames
  int* frames = new int[cams.size()];
  for (unsigned int i=0; i<cams.size(); ++i) frames[i] = (int)i;
  boxm2_util::random_permutation(frames, cams.size());

  //timing vars
  vul_timer t;
  float total_gpu_time = 0.0f;
  float total_update_time = 0.0f;

  //update with each frame
  for (int i=0; i<numUpdates; ++i)
  {
    //update with random frame (or user chosen frame)
    int frame = (inFrame() >= 0) ? inFrame() : frames[i];


    vcl_cout<<"===========================================\n"
            <<"Update with frame: "<<frame<<", num "<<i<<" of "<<numUpdates<<vcl_endl;
    vil_image_view_base_sptr inImg = boxm2_util::prepare_input_image(imgs[frame], true);
    vpgl_camera_double_sptr  inCam = boxm2_util::camera_from_file( cams[frame] );
    vil_image_view<float>* inImgPtr = dynamic_cast<vil_image_view<float>* >(inImg.ptr());

    vul_timer t; t.mark();
    float gpu_time = boxm2_multi_update::update(mcache, *inImgPtr, inCam);
    float total = t.all();
    vcl_cout<<"  ===> Total update time: "<<total<<" ms\n"
            <<"  ===> total GPU time   : "<<gpu_time<<" ms\n"
            <<"  ===> total gpu / total: "<<gpu_time/total<<vcl_endl;
    total_gpu_time += gpu_time;
    total_update_time += total;

    //handle rendering
    if (renderInt() > 0 && i%renderInt()==(renderInt()-1))
    {
      //create cam
      double currInc    = 45.0;
      double currRadius = scene->bounding_box().height();
      double currAz     = i*30.0f;
      vpgl_perspective_camera<double>* pcam;
      pcam = boxm2_util::construct_camera(currInc, currAz, currRadius, ni, nj, scene->bounding_box(), false);
      vpgl_camera_double_sptr cam = new vpgl_perspective_camera<double>(*pcam);

      //render/save image
      vil_image_view<float> out(ni,nj);
      float rtime = renderer.render(mcache, out, cam);
      vcl_cout<<"Render "<<i<<" time: "<<rtime<<vcl_endl;
      vcl_stringstream s; s<<"out_"<<i<<".tiff";
      vil_save(out, s.str().c_str());
    }
  }

  float totalTime = t.all();
  vcl_cout<<"----------------------------------------\n"
          <<" total time:        "<<totalTime/1000.0f<<" seconds\n"
          <<" total update time: "<<total_update_time/1000.0f<<" seconds\n"
          <<" total GPU time:    "<<total_gpu_time / 1000.0f<<" seconds\n"
          <<" GPU Time/Total Time:"<< total_gpu_time / total_update_time << '\n'
          <<" ave   update time: "<<total_update_time/numUpdates/1000.0f<<" seconds\n"
          <<" ave gpu update time:"<<total_gpu_time/numUpdates/1000.0f<<" seconds\n"
          <<"------------------------------------------"<<vcl_endl;

  //clear ocl caches (writes to CPU cache)
  mcache.clear();

  //write cache
  boxm2_lru_cache::instance()->write_to_disk();

  return 0;
}
