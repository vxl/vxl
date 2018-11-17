// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_update_histogram_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for updating the histogram given an image.
//
// \author Vishal Jain
// \date Mar 10, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/boxm2_util.h>
#include <vil/vil_image_view.h>
//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

namespace boxm2_ocl_update_histogram_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 0;
  std::size_t local_threads[]={8,8};
  void compile_kernel(const bocl_device_sptr& device,std::vector<bocl_kernel*> & vec_kernels)
  {
    std::vector<std::string> src_paths;
    std::string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "cell_utils.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "bit/batchkernels.cl");
    src_paths.push_back(source_dir + "update_functors.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    //compilation options
    std::string options = " -D INTENSITY ";
    options += " -D MOG_TYPE_8 ";
    //create all passes
    auto* seg_len = new bocl_kernel();
    std::string seg_opts = options + "-D DETERMINISTIC -D CUMLEN -D STEP_CELL=step_cell_cumlen(aux_args,data_ptr,llid,d) ";
    seg_len->create_kernel(&device->context(), device->device_id(), src_paths, "cum_len_main", seg_opts, "update::seg_len");
    vec_kernels.push_back(seg_len);


    auto* update_hist = new bocl_kernel();
    std::string hist_opts = options + "-D DETERMINISTIC -D UPDATE_HIST -D STEP_CELL=step_cell_update_hist(aux_args,data_ptr,llid,d) ";
    update_hist->create_kernel(&device->context(), device->device_id(), src_paths, "update_hist_main", hist_opts, "update::hist");
    vec_kernels.push_back(update_hist);

    std::vector<std::string> clean_seg_kernels_src;
    clean_seg_kernels_src.push_back(source_dir + "scene_info.cl");
    clean_seg_kernels_src.push_back(source_dir + "bit/batchkernels.cl");

    auto* clean_seg_len = new bocl_kernel();
    std::string clean_seg_len_opts = options + " -D CLEAN_SEG_LEN ";
    clean_seg_len->create_kernel(&device->context(), device->device_id(), clean_seg_kernels_src, "clean_seg_len_main", clean_seg_len_opts, "clean::seg_len");
    vec_kernels.push_back(clean_seg_len);
  }
  static std::map<cl_device_id*,std::vector<bocl_kernel*> > kernels;
}

bool boxm2_ocl_update_histogram_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_histogram_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "vcl_string";


  std::vector<std::string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_ocl_update_histogram_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_histogram_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  float transfer_time=0.0f;
  float gpu_time=0.0f;
  //get the inputs
  unsigned i = 0;
  bocl_device_sptr device= pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr opencl_cache= pro.get_input<boxm2_opencl_cache_sptr>(i++);
  vpgl_camera_double_sptr cam= pro.get_input<vpgl_camera_double_sptr>(i++);
  std::string in_img_name= pro.get_input<std::string>(i++);


//: create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),
                                                *(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,
                                                &status);
  if (status!=0)
    return false;

  // compile the kernel
  if (kernels.find((device->device_id()))==kernels.end())
  {
    std::cout<<"===========Compiling kernels==========="<<std::endl;
    std::vector<bocl_kernel*> ks;
    compile_kernel(device,ks);
    kernels[(device->device_id())]=ks;
  }
  // create all buffers
  cl_float cam_buffer[48];
  boxm2_ocl_util::set_ocl_camera(cam, cam_buffer);
  bocl_mem_sptr persp_cam=new bocl_mem(device->context(), cam_buffer, 3*sizeof(cl_float16), "persp cam buffer");
  persp_cam->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  vil_image_view_base_sptr in_img=boxm2_util::prepare_input_image(in_img_name, true);

  unsigned ni=in_img->ni();
  unsigned nj=in_img->nj();


  unsigned cl_ni=RoundUp(ni,local_threads[0]);
  unsigned cl_nj=RoundUp(nj,local_threads[1]);
  auto* buff = new float[cl_ni*cl_nj];
  if (auto * in_img_float=dynamic_cast<vil_image_view<float> *>(in_img.ptr()))
  {
    int count=0;
    for (unsigned j=0;j<cl_nj;j++)
      for (unsigned i=0;i<cl_ni;i++)
      {
        if (i<ni && j< nj)
          buff[count]=(*in_img_float)(i,j);
        ++count;
      }
  }
  else
    return false;
  auto* vis_buff = new float[cl_ni*cl_nj];
  for (unsigned i=0;i<cl_ni*cl_nj;i++) vis_buff[i]=1.0f;

  bocl_mem_sptr in_image=new bocl_mem(device->context(),buff,cl_ni*cl_nj*sizeof(float),"exp image buffer");
  in_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr vis_image=new bocl_mem(device->context(),vis_buff,cl_ni*cl_nj*sizeof(float),"exp image buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // Image Dimensions
  int img_dim_buff[4];
  img_dim_buff[0] = 0;
  img_dim_buff[1] = 0;
  img_dim_buff[2] = ni;
  img_dim_buff[3] = nj;
  bocl_mem_sptr exp_img_dim=new bocl_mem(device->context(), img_dim_buff, sizeof(int)*4, "image dims");
  exp_img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // Output Array
  float output_arr[100];
  for (float & i : output_arr) i = 0.0f;
  bocl_mem_sptr  cl_output=new bocl_mem(device->context(), output_arr, sizeof(float)*100, "output buffer");
  cl_output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  //2. set workgroup size
  for (unsigned kernelindex=0;kernelindex<3;kernelindex++)
  {
    // set arguments
    std::vector<boxm2_block_id> vis_order = scene->get_vis_blocks( (vpgl_perspective_camera<double>*) cam.ptr());
    std::vector<boxm2_block_id>::iterator id;
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
      //choose correct render kernel
      boxm2_block_metadata mdata = scene->get_block_metadata(*id);
      bocl_kernel* kern =  kernels[(device->device_id())][kernelindex];

      if (kernelindex==0)
      {
        //write the image values to the buffer
        vul_timer transfer;
        bocl_mem * blk = opencl_cache->get_block(scene,*id);
        bocl_mem * blk_info = opencl_cache->loaded_block_info();
        bocl_mem * aux = opencl_cache->get_data<BOXM2_AUX>(scene,*id);

        transfer_time += (float) transfer.all();

        ////3. SET args
        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( aux );
        kern->set_arg( lookup.ptr() );
        kern->set_arg( persp_cam.ptr() );
        kern->set_arg( exp_img_dim.ptr());
        kern->set_arg( cl_output.ptr() );

        //local tree , cumsum buffer
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );
        kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) );
        std::size_t lThreads[] = {8, 8};
        std::size_t gThreads[] = {cl_ni,cl_nj};
        //execute kernel
        kern->execute(queue, 2, lThreads, gThreads);
        clFinish(queue);
        gpu_time += kern->exec_time();
        aux->read_to_buffer(queue);
      }
      else if (kernelindex==1)
      {
        vul_timer transfer;
        bocl_mem * blk = opencl_cache->get_block(scene,*id);
        bocl_mem * blk_info = opencl_cache->loaded_block_info();
        bocl_mem * alpha = opencl_cache->get_data<BOXM2_ALPHA>(scene,*id);
        bocl_mem * hist = opencl_cache->get_data<BOXM2_BATCH_HISTOGRAM>(scene,*id);
        bocl_mem * aux = opencl_cache->get_data<BOXM2_AUX>(scene,*id);

        transfer_time += (float) transfer.all();

        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( aux );
        kern->set_arg( hist );
        kern->set_arg( lookup.ptr() );
        kern->set_arg( persp_cam.ptr() );
        kern->set_arg( exp_img_dim.ptr() );
        kern->set_arg( in_image.ptr() );
        kern->set_arg( vis_image.ptr() );
        kern->set_arg( cl_output.ptr() );
        //local tree , cumsum buffer
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );
        kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) );
        std::size_t gThreads[] = {cl_ni,cl_nj};
        //execute kernel
        kern->execute(queue, 2, local_threads, gThreads);
        clFinish(queue);
        gpu_time += kern->exec_time();
        aux->read_to_buffer(queue);
        hist->read_to_buffer(queue);
      }
      else if (kernelindex==2) // clean seg len
      {
        vul_timer transfer;
        bocl_mem * blk_info = opencl_cache->loaded_block_info();
        bocl_mem * aux = opencl_cache->get_data<BOXM2_AUX>(scene,*id);

        auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
        int aux_type_size = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX>::prefix());
        info_buffer->data_buffer_length = (int) (aux->num_bytes()/aux_type_size);
        blk_info->write_to_buffer(queue);

        int numbuf = info_buffer->num_buffer;
        int datlen = info_buffer->data_buffer_length;
        transfer_time += (float) transfer.all();
        kern->set_arg( blk_info );
        kern->set_arg( aux );
        kern->set_arg( cl_output.ptr() );

        std::size_t lThreads[] = { 1,1};
        std::size_t gThreads[] = { RoundUp(numbuf*datlen,64),1};
        //execute kernel
        kern->execute(queue, 2, lThreads, gThreads);
        clFinish(queue);
        gpu_time += kern->exec_time();

        aux->read_to_buffer(queue);
      }
      //clear render kernel args so it can reset em on next execution
      kern->clear_args();
    }
  }
  clReleaseCommandQueue(queue);
  return true;
}
