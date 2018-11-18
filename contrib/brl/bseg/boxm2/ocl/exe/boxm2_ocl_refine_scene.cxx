//executable args
#include <vul/vul_arg.h>

//boxm2 scene stuff
#include <boxm2/ocl/algo/boxm2_ocl_refine.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <bocl/bocl_cl.h>
#include <bocl/bocl_manager.h>


int main(int argc,  char** argv)
{
  std::cout<<"Boxm2 Hemisphere"<<std::endl;
  vul_arg<std::string> scene_file("-scene", "scene filename", "");
  vul_arg<float> thresh("-thresh", "Probability threshold", .3f);
  vul_arg<int> gpuNum("-gpu", "Device number", 0);
  vul_arg_parse(argc, argv);

  //create scene
  boxm2_scene_sptr scene = new boxm2_scene(scene_file());
  bocl_manager_child &mgr = bocl_manager_child::instance();
  bocl_device_sptr device = mgr.gpus_[gpuNum()];

  //create cache, grab singleton instance
  boxm2_lru_cache::create(scene);
  boxm2_opencl_cache_sptr opencl_cache = new boxm2_opencl_cache(device);

  //refine scene...
  boxm2_ocl_refine::refine_scene( device, scene, opencl_cache, thresh() );

  //write scene
  boxm2_lru_cache::instance()->write_to_disk();

  return 0;
}
