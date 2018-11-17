// This is brl/bseg/bstm/ocl/pro/processes/bstm_ocl_label_along_ray_process.cxx
//:
// \file
// \brief  A process for labeling voxels along ray
//
// \author Ali Osman Ulusoy
// \date May 15, 2013

#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <bprb/bprb_func_process.h>
#include <bstm/ocl/bstm_opencl_cache.h>
#include <bstm/bstm_scene.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_data_base.h>
#include <bstm/ocl/bstm_ocl_util.h>
#include <bstm/bstm_util.h>
#include <vil/vil_image_view.h>
#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>

// brdb stuff
#include <brdb/brdb_value.h>

// directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

namespace bstm_ocl_label_along_ray_process_globals
{
  constexpr unsigned n_inputs_ = 8;
  constexpr unsigned n_outputs_ = 0;

  bocl_kernel* compile_kernel(const bocl_device_sptr& device,const std::string& opts)
  {
    //gather all render sources... seems like a lot for rendering...
    std::vector<std::string> src_paths;
    std::string source_dir = std::string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/bstm/ocl/cl/";
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "bit/time_tree_library_functions.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "change/change_detection.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    //compilation options
    std::string options = opts+ "-D LABELING ";

    auto* compute_vis = new bocl_kernel();
    std::string seg_opts = options + " -D STEP_CELL=step_cell_label(aux_args,data_ptr,data_ptr_tt,d)";
    compute_vis->create_kernel(&device->context(),device->device_id(), src_paths, "label_scene", seg_opts, "label_scene");
    return compute_vis;
  }

  static std::map<std::string, bocl_kernel* > kernels_;
}

bool bstm_ocl_label_along_ray_process_cons(bprb_func_process& pro)
{
  using namespace bstm_ocl_label_along_ray_process_globals;

  // process takes 9 inputs and two outputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "bstm_scene_sptr";
  input_types_[2] = "bstm_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "vil_image_view_base_sptr"; //change image
  input_types_[5] = "float";        // change prob threshold
  input_types_[6] = "float";        // time
  input_types_[7] = "int";          // label


  std::vector<std::string>  output_types_(n_outputs_);

  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  return good;
}

bool bstm_ocl_label_along_ray_process(bprb_func_process& pro)
{
  using namespace bstm_ocl_label_along_ray_process_globals;
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  bocl_device_sptr         device = pro.get_input<bocl_device_sptr>(i++);
  bstm_scene_sptr          scene = pro.get_input<bstm_scene_sptr>(i++);
  bstm_opencl_cache_sptr   opencl_cache = pro.get_input<bstm_opencl_cache_sptr>(i++);
  vpgl_camera_double_sptr  cam = pro.get_input<vpgl_camera_double_sptr>(i++);
  vil_image_view_base_sptr change_img = pro.get_input<vil_image_view_base_sptr>(i++);
  auto                   change_p = pro.get_input<float>(i++);
  auto                   time = pro.get_input<float>(i++);
  unsigned char          label = pro.get_input<int>(i++);

  ///////////////////////
  float transfer_time=0.0f;
  float gpu_time=0.0f;

  unsigned ni=change_img->ni();
  unsigned nj=change_img->nj();
  std::size_t  local_threads [2] = {8,8};
  std::size_t  global_threads[2] = {8,8};

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue( device->context(), *(device->device_id()), CL_QUEUE_PROFILING_ENABLE, &status);
  if (status!=0)
   return false;

  // compile the kernel if not already compiled
  std::string identifier=device->device_identifier();
  if (kernels_.find(identifier)==kernels_.end()) {
   std::cout<<"===========Compiling kernels==========="<<std::endl;
   kernels_[identifier]=  compile_kernel(device,"");
  }


  //----- PREP INPUT BUFFERS -------------
  //prepare input images
  auto*   img_view = static_cast<vil_image_view<float>* >(change_img.ptr());

  //prepare workspace size
  unsigned cl_ni = RoundUp(img_view->ni(),local_threads[0]);
  unsigned cl_nj = RoundUp(img_view->nj(),local_threads[1]);
  global_threads[0] = cl_ni;
  global_threads[1] = cl_nj;

  // create all buffers
  auto* ray_origins = new cl_float[4*cl_ni*cl_nj];
  auto* ray_directions = new cl_float[4*cl_ni*cl_nj];
  bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_origins, "ray_origins buffer");
  bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_directions, "ray_directions buffer");
  boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);

  //prepare image buffers (cpu)
  auto* input_buff = new float[cl_ni*cl_nj];
  auto* vis_buff = new float[cl_ni*cl_nj];

  int count=0;
  for (unsigned int j=0;j<cl_nj;++j) {
      for (unsigned int i=0;i<cl_ni;++i) {
          input_buff[count] = 0.0f;
          vis_buff[count]=1.0f;
          if (i<img_view->ni() && j< img_view->nj())
              input_buff[count] = (*img_view)(i,j);
          ++count;
      }
  }

  bocl_mem_sptr vis_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), vis_buff, "vis image buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //prepare image buffers (GPU)
  bocl_mem_sptr in_image = opencl_cache->alloc_mem(4*cl_ni*cl_nj*sizeof(float), input_buff, "input image buffer");
  in_image->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  // Image Dimensions
  unsigned int img_dim_buff[] = { 0, 0, img_view->ni(), img_view->nj() };
  bocl_mem_sptr img_dim=new bocl_mem(device->context(), img_dim_buff, sizeof(unsigned int)*4, "image dims");
  img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // bit lookup buffer
  cl_uchar lookup_arr[256];
  bstm_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  cl_float cl_time = 0;
  bocl_mem_sptr time_mem =new bocl_mem(device->context(), &cl_time, sizeof(cl_float), "time instance buffer");
  time_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  cl_float cl_change_prob_t = change_p;
  bocl_mem_sptr change_prob_t_mem =new bocl_mem(device->context(), &cl_change_prob_t, sizeof(cl_change_prob_t), "change probability threshold buffer");
  change_prob_t_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  cl_uchar cl_label = label;
  bocl_mem_sptr label_mem =new bocl_mem(device->context(), &cl_label, sizeof(cl_label), "label buffer");
  label_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);



  //For each ID in the visibility order, grab that block
  std::vector<bstm_block_id> vis_order = scene->get_vis_blocks(cam);
  std::vector<bstm_block_id>::iterator id;
  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
     //choose correct render kernel
     bstm_block_metadata mdata = scene->get_block_metadata(*id);
     bocl_kernel* kern =  kernels_[identifier];

     //if the current blk does not contain the queried time, no need to ray cast
     double local_time;
     if(!mdata.contains_t(time,local_time))
       continue;
     //write cl_time
     cl_time = (cl_float)local_time;
     time_mem->write_to_buffer(queue);

     //write the image values to the buffer
     vul_timer transfer;
     bocl_mem* blk = opencl_cache->get_block(*id);
     bocl_mem* blk_t = opencl_cache->get_time_block(*id);
     bocl_mem* blk_info = opencl_cache->loaded_block_info();
     bocl_mem *alpha = opencl_cache->get_data<BSTM_ALPHA>(*id,0);


     bocl_mem* blk_t_info= opencl_cache->loaded_time_block_info();
     auto* info_buffer_t = (bstm_scene_info*) blk_t_info->cpu_buffer();
     int num_time_trees = info_buffer_t->tree_buffer_length;

     int auxTypeSize = (int) bstm_data_info::datasize(bstm_data_traits<BSTM_LABEL>::prefix());
     bocl_mem *label_buff = opencl_cache->get_data<BSTM_LABEL>(*id, num_time_trees*auxTypeSize);

     transfer_time += (float) transfer.all();

     ////3. SET args
     kern->set_arg( blk_info );
     kern->set_arg( blk );
     kern->set_arg( blk_t );
     kern->set_arg( alpha );
     kern->set_arg( label_buff);
     kern->set_arg( ray_o_buff.ptr() );
     kern->set_arg( ray_d_buff.ptr() );
     kern->set_arg( in_image.ptr() );
     kern->set_arg( vis_image.ptr() );
     kern->set_arg( img_dim.ptr());
     kern->set_arg( lookup.ptr() );
     kern->set_arg( time_mem.ptr() );
     kern->set_arg( change_prob_t_mem.ptr());
     kern->set_arg( label_mem.ptr());

     //local tree , cumsum buffer, imindex buffer
     kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );
     kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar8) );
     kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) );
     kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_int) );


     //execute kernel
     kern->execute(queue, 2, local_threads, global_threads);
     clFinish(queue);
     gpu_time += kern->exec_time();

     label_buff->read_to_buffer(queue);

     //clear render kernel args so it can reset em on next execution
     kern->clear_args();
     kern->release_current_event();
  }

  //cleanup the image buffers
  delete [] ray_origins;
  delete [] ray_directions;
  delete [] input_buff;
  delete [] vis_buff;
  opencl_cache->unref_mem(in_image.ptr());
  opencl_cache->unref_mem(vis_image.ptr());
  opencl_cache->unref_mem(ray_o_buff.ptr());
  opencl_cache->unref_mem(ray_d_buff.ptr());
  clReleaseCommandQueue(queue);


  return true;
}
