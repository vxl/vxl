#include <boxm2/boxm2_opencl_render_process.h>

//boxm2 data structures
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>

//cameras/images
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>

//  probably don't need this...
bool boxm2_opencl_render_process::init(){}

bool boxm2_opencl_render_process::init_kernel(cl_context& context,
                                              cl_device_id& device)
{
  context_ = &context;

  //gather all render sources... seems like a lot for rendering...
  vcl_vector<vcl_string> src_paths;
  vcl_string source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/cl/";
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "cell_utils.cl");
  src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir + "backproject.cl");
  src_paths.push_back(source_dir + "statistics_library_functions.cl");
  src_paths.push_back(source_dir + "expected_functor.cl");
  src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
  src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");
  src_paths.push_back(source_dir + "bit/render_bit_scene.cl");

  vcl_string options = "-D INTENSITY ";
  options += "-D NVIDIA ";
  options += "-D RENDER ";

  bool created =  render_kernel_.create_kernel(&context,
                                               &device,
                                               src_paths,
                                               "render_bit_scene",
                                               options,
                                               "boxm2 opencl render");

  //TODO FIGURE OUT A GOOD PLACE FOR THE COMMAND QUEUE TO LIVE -
  //seems like it should be above process (processor should have a list of command queues)
  // set up a command queue
  int status;
  command_queue_ = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &status);
  if (!check_val(status,CL_SUCCESS,"Failed in command queue creation" + error_to_string(status)))
    return false;

  return created;
}


//  NEED TO FIGURE OUT HOW TO differentiate between SCENE/BLOCK/DATA arguments and Camera/Image arguments
// the scene level stuff needs to live on the processor, other
bool boxm2_opencl_render_process::execute(vcl_vector<brdb_value_sptr>& input, vcl_vector<brdb_value_sptr>& output)
{
  vcl_cout<<"GPu RENDER!!"<<vcl_endl;

  int* test_buffer = new int[200];
  for (int i=0; i<200; i++) test_buffer[i] = 200-i;
  bocl_mem test_mem( (*context_), test_buffer, sizeof(int)*200, "test buffer");
  test_mem.create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  render_kernel_.set_arg( &test_mem );
  vcl_size_t lThreads[] = {8, 1};
  vcl_size_t gThreads[] = {16, 1};
  render_kernel_.execute(command_queue_, lThreads, gThreads);

  test_mem.read_to_buffer(command_queue_);

  for (int i=0; i<10; i++)
    vcl_cout<<"i "<<test_buffer[i]<<vcl_endl;

  return true;
}

